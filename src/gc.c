#include "vm.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD
=======
#include <time.h>
>>>>>>> dev_rishabh

static void mark_object(Obj *obj);
static void mark_value(Value val);
static void sweep(VM *vm);

void gc(VM *vm){
    // Start timing
    clock_t start = clock();
    
    int before = vm->heap_size;
    mark_roots(vm);
    sweep(vm);
<<<<<<< HEAD
=======
    int after = vm->heap_size;
    int collected = before - after;
    
    // End timing
    clock_t end = clock();
    double gc_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    // Update statistics
    vm->gc_stats.total_gc_calls++;
    vm->gc_stats.total_objects_freed += collected;
    vm->gc_stats.total_gc_time += gc_time;
    
    // Track min/max pause times
    if (vm->gc_stats.total_gc_calls == 1) {
        vm->gc_stats.min_gc_pause = gc_time;
        vm->gc_stats.max_gc_pause = gc_time;
    } else {
        if (gc_time < vm->gc_stats.min_gc_pause) {
            vm->gc_stats.min_gc_pause = gc_time;
        }
        if (gc_time > vm->gc_stats.max_gc_pause) {
            vm->gc_stats.max_gc_pause = gc_time;
        }
    }
    
>>>>>>> dev_rishabh
}

void mark_roots(VM *vm){
    // Mark all values on the stack
    for(int i=0;i<=vm->stack.sp;i++){
        mark_value(vm->stack.data[i]);
    }
    
    // Mark all values in VM memory (important for objects stored via STORE)
    for(int i=0;i<MEM_SIZE;i++){
        if(vm->valid[i]){
            mark_value(vm->memory[i]);
        }
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
            
        case OBJ_FUNCTION:
            // Functions have no references to mark
            break;
            
        case OBJ_CLOSURE:
            // Mark the function
            mark_object(object->as.closure.function);
            // Mark the environment
            mark_object(object->as.closure.env);
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
<<<<<<< HEAD
=======
            vm->heap_size--;
>>>>>>> dev_rishabh
        }
        else{
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
<<<<<<< HEAD
}
=======
}

// Performance reporting function
void print_gc_stats(VM *vm) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║           Garbage Collection Performance Report           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("Memory Statistics:\n");
    printf("  Total objects allocated:    %ld\n", vm->gc_stats.total_objects_allocated);
    printf("  Total objects freed:        %ld\n", vm->gc_stats.total_objects_freed);
    printf("  Objects still alive:        %d\n", vm->heap_size);
    printf("  Peak heap size:             %d objects\n", vm->gc_stats.max_heap_size);
    printf("  Memory per object:          %zu bytes\n", sizeof(Obj));
    printf("  Peak memory usage:          ~%zu bytes\n", 
           vm->gc_stats.max_heap_size * sizeof(Obj));
    printf("\n");
    
    printf("Garbage Collection Statistics:\n");
    printf("  Total GC invocations:       %ld\n", vm->gc_stats.total_gc_calls);
    printf("  Total GC time:              %.6f seconds\n", vm->gc_stats.total_gc_time);
    
    if (vm->gc_stats.total_gc_calls > 0) {
        double avg_pause = vm->gc_stats.total_gc_time / vm->gc_stats.total_gc_calls;
        printf("  Average GC pause:           %.6f seconds\n", avg_pause);
        printf("  Min GC pause:               %.6f seconds\n", vm->gc_stats.min_gc_pause);
        printf("  Max GC pause:               %.6f seconds\n", vm->gc_stats.max_gc_pause);
        
        double avg_collected = (double)vm->gc_stats.total_objects_freed / vm->gc_stats.total_gc_calls;
        printf("  Average objects/collection: %.1f\n", avg_collected);
    }
    printf("\n");
    
    printf("Collection Efficiency:\n");
    if (vm->gc_stats.total_objects_allocated > 0) {
        double collection_rate = (double)vm->gc_stats.total_objects_freed / 
                                 vm->gc_stats.total_objects_allocated * 100.0;
        printf("  Collection rate:            %.1f%%\n", collection_rate);
        printf("  Survival rate:              %.1f%%\n", 100.0 - collection_rate);
    }
    printf("\n");
    
    printf("Performance Impact:\n");
    printf("  Total instructions:         %ld\n", vm->instruction_count);
    if (vm->instruction_count > 0) {
        double gc_overhead = (vm->gc_stats.total_gc_time / 
                             (vm->gc_stats.total_gc_time + 0.0001)) * 100.0;
        printf("  GC overhead:                %.2f%% of execution time\n", gc_overhead);
        
        if (vm->gc_stats.total_gc_calls > 0) {
            double instrs_per_gc = (double)vm->instruction_count / vm->gc_stats.total_gc_calls;
            printf("  Instructions per GC:        %.0f\n", instrs_per_gc);
        }
    }
    
    printf("\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
}
>>>>>>> dev_rishabh
