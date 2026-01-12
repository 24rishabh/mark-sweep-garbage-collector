#include "vm.h"
#include "object.h"
#include <stdio.h>

static void mark_object(Obj *obj);

void gc(VM *vm){
    mark_root(vm);
}

void mark_root(VM *vm){
    for(int i=0;i<=vm->stack.sp;i++){
        Value val = vm->stack.data[i];
        if(val.type == VAL_OBJ){
            mark_object(val.as.obj);
        }
    }

}

static void mark_object(Obj *object){
    if(object == NULL) return;
    if(object->marked==1) return;
    object->marked = 1;
}