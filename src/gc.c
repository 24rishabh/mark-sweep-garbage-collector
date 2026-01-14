#include "vm.h"
#include "object.h"
#include <stdio.h>

static void mark_object(Obj *obj);
static void mark_value(Value val);

void gc(VM *vm){
    mark_roots(vm);
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