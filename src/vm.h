#ifndef VM_H
#define VM_H

#include "stack.h"
#include "object.h"

#define MEM_SIZE 1024
#define RET_STACK_SIZE 1024

typedef struct{
    Stack stack;
    int *bytecode;
    int pc;
    int running; // is vm running?
    int memory[MEM_SIZE];
    int valid[MEM_SIZE]; // is the current value stored is valid or not
    int ret_stack[RET_STACK_SIZE];
    int rsp;
    long instruction_count;

    Obj *heap_head;
}VM;

void vm_init(VM *vm,int *bytecode);
void vm_run(VM *vm);
void gc(VM *vm); // GC entry point
void mark_roots(VM *vm);

#endif