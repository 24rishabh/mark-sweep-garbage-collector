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
    vm->heap_size++;
    
    // Track allocation statistics
    vm->gc_stats.total_objects_allocated++;
    vm->gc_stats.bytes_allocated += sizeof(Obj);
    
    // Update peak heap size
    if (vm->heap_size > vm->gc_stats.max_heap_size) {
        vm->gc_stats.max_heap_size = vm->heap_size;
    }
    
    return obj;
}

Obj *new_function(VM *vm, int address, int arity){
    Obj *obj = (Obj*)malloc(sizeof(Obj));
    if(!obj){
        printf("Out of memory\n");
        exit(1);
    }

    obj->type = OBJ_FUNCTION;
    obj->marked = 0;
    obj->as.function.address = address;
    obj->as.function.arity = arity;

    obj->next = vm->heap_head;
    vm->heap_head = obj;
    vm->heap_size++;
    
    // Track allocation statistics
    vm->gc_stats.total_objects_allocated++;
    vm->gc_stats.bytes_allocated += sizeof(Obj);
    
    // Update peak heap size
    if (vm->heap_size > vm->gc_stats.max_heap_size) {
        vm->gc_stats.max_heap_size = vm->heap_size;
    }
    
    return obj;
}

Obj *new_closure(VM *vm, Obj *function, Obj *env){
    Obj *obj = (Obj*)malloc(sizeof(Obj));
    if(!obj){
        printf("Out of memory\n");
        exit(1);
    }

    obj->type = OBJ_CLOSURE;
    obj->marked = 0;
    obj->as.closure.function = function;
    obj->as.closure.env = env;

    obj->next = vm->heap_head;
    vm->heap_head = obj;
    vm->heap_size++;
    
    // Track allocation statistics
    vm->gc_stats.total_objects_allocated++;
    vm->gc_stats.bytes_allocated += sizeof(Obj);
    
    // Update peak heap size
    if (vm->heap_size > vm->gc_stats.max_heap_size) {
        vm->gc_stats.max_heap_size = vm->heap_size;
    }
    
    return obj;
}
