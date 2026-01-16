#include "vm.h"
#include<stdio.h>
#include "value.h"
#include "object.h"

#define OP_PUSH 0x01
#define OP_POP 0x02
#define OP_DUP 0x03
#define OP_ADD 0x10
#define OP_SUB 0x11
#define OP_MUL 0x12
#define OP_DIV 0x13
#define OP_CMP 0x14
#define OP_HALT 0xff
#define OP_JMP  0x20
#define OP_JZ   0x21
#define OP_JNZ  0x22
#define OP_STORE 0x30
#define OP_LOAD  0x31
#define OP_CALL 0x40
#define OP_RET 0x41
#define OP_NEW_PAIR 0x50    // New: create a pair from top two stack values
#define OP_PAIR_LEFT 0x51   // New: get left value from pair
#define OP_PAIR_RIGHT 0x52  // New: get right value from pair
#define OP_SET_LEFT 0x53    // New: set left value of pair
#define OP_SET_RIGHT 0x54   // New: set right value of pair
#define OP_GC 0x60          // New: explicit GC trigger


void vm_init(VM *vm,int *bytecode){
    init_stack(&vm->stack);
    vm->bytecode = bytecode;
    vm->pc = 0;
    vm->running = 1;
    vm->rsp = -1;
    vm->instruction_count = 0;
    vm->heap_head = NULL;
    vm->heap_size = 0;
    vm->gc_threshold = 100;
    
    // Initialize performance statistics
    vm->gc_stats.total_gc_calls = 0;
    vm->gc_stats.total_objects_allocated = 0;
    vm->gc_stats.total_objects_freed = 0;
    vm->gc_stats.total_gc_time = 0.0;
    vm->gc_stats.max_gc_pause = 0.0;
    vm->gc_stats.min_gc_pause = 0.0;
    vm->gc_stats.max_heap_size = 0;
    vm->gc_stats.bytes_allocated = 0;
    
    for(int i=0;i<MEM_SIZE;i++){
        vm->memory[i] = make_int_value(0); //clear the memory with Value type
        vm->valid[i] = 0; //nothing is valid initially
    }
}

void vm_run(VM *vm){
    while(vm->running){
        int instruction = vm->bytecode[vm->pc++];
        vm->instruction_count++;
        
        // Trigger GC when heap size exceeds threshold
        if(vm->heap_size >= vm->gc_threshold) {
            gc(vm);
            vm->gc_threshold = vm->heap_size * 2 + 100; // Grow threshold
        }
        
        switch(instruction){
            case OP_PUSH:{
                int value = vm->bytecode[vm->pc++];
                push(&vm->stack,make_int_value(value));
                break;
            }
            case OP_POP: {
                pop(&vm->stack);
                break;
            }
            case OP_DUP:{
                Value value = peek(&vm->stack);
                push(&vm->stack,value);
                break;
            }
            case OP_ADD:{
                Value b = pop(&vm->stack);
                Value a = pop(&vm->stack);
                push(&vm->stack,make_int_value(a.as.i + b.as.i));
                break;
            }
            case OP_SUB:{
                Value b = pop(&vm->stack);
                Value a = pop(&vm->stack);
                push(&vm->stack,make_int_value(a.as.i - b.as.i));
                break;
            }
            case OP_MUL:{
                Value b = pop(&vm->stack);
                Value a = pop(&vm->stack);
                push(&vm->stack,make_int_value(a.as.i * b.as.i));
                break;
            }
            case OP_DIV:{
                Value b = pop(&vm->stack);
                Value a = pop(&vm->stack);
                if(b.as.i==0){
                    printf("Runtime error: division by zero\n");
                    vm->running = 0;
                    break;
                }
                push(&vm->stack,make_int_value(a.as.i/b.as.i));
                break;
            }
            case OP_CMP:{
                Value b = pop(&vm->stack);
                Value a = pop(&vm->stack);
                if(a.as.i < b.as.i) push(&vm->stack,make_int_value(1));
                else push(&vm->stack,make_int_value(0));
                break;
            }
            case OP_HALT:{
                vm->running = 0;
                break;
            }
            case OP_JMP:{
                int address = vm->bytecode[vm->pc++];
                vm->pc = address;
                break;
            }
            case OP_JZ:{
                int address = vm->bytecode[vm->pc++];
                Value condition = pop(&vm->stack);
                if(condition.as.i==0) vm->pc = address;
                break;
            }
            case OP_JNZ:{
                int address = vm->bytecode[vm->pc++];
                Value condition = pop(&vm->stack);
                if(condition.as.i!=0) vm->pc = address;
                break;
            }
            case OP_STORE:{
                int index = vm->bytecode[vm->pc++];
                Value top = pop(&vm->stack);
                if(index>=MEM_SIZE){
                    printf("Memory Overflow\n");
                    vm->running = 0;
                    break;
                }
                vm->memory[index] = top;  // Store the whole Value (int or object)
                vm->valid[index] = 1;
                break;
            }
            case OP_LOAD:{
                int index = vm->bytecode[vm->pc++];
                if(vm->valid[index]==0){
                    printf("your program trying to load the invalid data\n");
                    vm->running = 0;
                    break;
                }
                Value value = vm->memory[index];  // Load the whole Value
                push(&vm->stack, value);
                break;
            }
            case OP_CALL:{
                int address = vm->bytecode[vm->pc++];
                if(vm->rsp>=RET_STACK_SIZE){
                    printf("Return Stack Overflow\n");
                    vm->running = 0;
                    break;
                }
                vm->ret_stack[++vm->rsp] = vm->pc;
                vm->pc = address;
                break;
            }
            case OP_RET:{
                if(vm->rsp < 0){
                    printf("Return Stack Underflow\n");
                    vm->running = 0;
                    break;
                }
                vm->pc = vm->ret_stack[vm->rsp--];
                break;
            }
            case OP_NEW_PAIR:{
                // Pop right, then left from stack
                Value right = pop(&vm->stack);
                Value left = pop(&vm->stack);
                Obj *pair = new_pair(vm, left, right);
                push(&vm->stack, make_obj_value(pair));
                break;
            }
            case OP_PAIR_LEFT:{
                Value val = pop(&vm->stack);
                if(val.type != VAL_OBJ){
                    printf("Runtime error: PAIR_LEFT expects object\n");
                    vm->running = 0;
                    break;
                }
                if(val.as.obj->type != OBJ_PAIR){
                    printf("Runtime error: PAIR_LEFT expects pair object\n");
                    vm->running = 0;
                    break;
                }
                push(&vm->stack, val.as.obj->as.pair.left);
                break;
            }
            case OP_PAIR_RIGHT:{
                Value val = pop(&vm->stack);
                if(val.type != VAL_OBJ){
                    printf("Runtime error: PAIR_RIGHT expects object\n");
                    vm->running = 0;
                    break;
                }
                if(val.as.obj->type != OBJ_PAIR){
                    printf("Runtime error: PAIR_RIGHT expects pair object\n");
                    vm->running = 0;
                    break;
                }
                push(&vm->stack, val.as.obj->as.pair.right);
                break;
            }
            case OP_SET_LEFT:{
                Value new_val = pop(&vm->stack);
                Value pair_val = pop(&vm->stack);
                if(pair_val.type != VAL_OBJ){
                    printf("Runtime error: SET_LEFT expects object\n");
                    vm->running = 0;
                    break;
                }
                if(pair_val.as.obj->type != OBJ_PAIR){
                    printf("Runtime error: SET_LEFT expects pair object\n");
                    vm->running = 0;
                    break;
                }
                pair_val.as.obj->as.pair.left = new_val;
                push(&vm->stack, pair_val); // Push pair back
                break;
            }
            case OP_SET_RIGHT:{
                Value new_val = pop(&vm->stack);
                Value pair_val = pop(&vm->stack);
                if(pair_val.type != VAL_OBJ){
                    printf("Runtime error: SET_RIGHT expects object\n");
                    vm->running = 0;
                    break;
                }
                if(pair_val.as.obj->type != OBJ_PAIR){
                    printf("Runtime error: SET_RIGHT expects pair object\n");
                    vm->running = 0;
                    break;
                }
                pair_val.as.obj->as.pair.right = new_val;
                push(&vm->stack, pair_val); // Push pair back
                break;
            }
            case OP_GC:{
                gc(vm);
                break;
            }
            default:
               printf("Unknown Instruction %d\n",instruction);
               vm->running = 0;
        }
    }
}
