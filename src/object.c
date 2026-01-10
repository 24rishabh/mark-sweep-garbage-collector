#include "vm.h"
#include "object.h"
#include<stdio.h>
#include<stdlib.h>

Obj *new_pair(VM *vm, Value left, Value right){
    Obj *obj = (Obj*)malloc(sizeof(Obj));
    if(!obj){
        printf("Out of memory\n");
        exit(1);
    }

    obj->type = OBJ_PAIR;
    obj->marked = 0;
    obj->as.pair.left = left;
    obj->as.pair.right = right;

    obj->next = vm->heap_head;
    vm->heap_head = obj;
    
    return obj;
}