#include "vm.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>

static void mark_object(Obj *obj);
static void mark_value(Value val);
static void sweep(VM *vm);

void gc(VM *vm){
    mark_roots(vm);
    sweep(vm);
}

void mark_roots(VM *vm){
    for(int i=0;i<=vm->stack.sp;i++){
        mark_value(vm->stack.data[i]);
    }
}

static void mark_value(Value val){
    if(val.type==VAL_OBJ){
        mark_object(val.as.obj);
    }
}



static void mark_object(Obj *object){
    if(object == NULL) return;
    if(object->marked==1) return;
    object->marked = 1;
    
    switch(object->type){
        case OBJ_PAIR:
            mark_value(object->as.pair.left);
            mark_value(object->as.pair.right);
            break;
    }
}

static void sweep(VM *vm){
    Obj **object = &vm->heap_head;
    while(*object){
        if((*object)->marked==0){
            Obj *garbage = *object;
            *object = garbage->next;
            free(garbage);
        }
        else{
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}