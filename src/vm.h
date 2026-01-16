#ifndef VM_H
#define VM_H

#include "stack.h"
#include "object.h"

#define MEM_SIZE 1024
#define RET_STACK_SIZE 1024

// Performance statistics structure
typedef struct {
    long total_gc_calls;           // Total number of GC invocations
    long total_objects_allocated;  // Total objects created
    long total_objects_freed;      // Total objects collected
    double total_gc_time;          // Total time spent in GC (seconds)
    double max_gc_pause;           // Longest GC pause (seconds)
    double min_gc_pause;           // Shortest GC pause (seconds)
    int max_heap_size;             // Peak heap size
    long bytes_allocated;          // Total bytes allocated
} GCStats;

typedef struct VM{
    Stack stack;
    int *bytecode;
    int pc;
    int running; // is vm running?
    Value memory[MEM_SIZE];        // Changed from int to Value!
    int valid[MEM_SIZE]; // is the current value stored is valid or not
    int ret_stack[RET_STACK_SIZE];
    int rsp;
    long instruction_count;

    Obj *heap_head;
    int heap_size;      // Current number of objects on heap
    int gc_threshold;   // Trigger GC when heap_size reaches this
    
    // Performance tracking
    GCStats gc_stats;
}VM;

void vm_init(VM *vm,int *bytecode);
void vm_run(VM *vm);
void gc(VM *vm); // GC entry point
void mark_roots(VM *vm);

// Object allocation
Obj *new_pair(VM *vm, Value left, Value right);
Obj *new_function(VM *vm, int address, int arity);
Obj *new_closure(VM *vm, Obj *function, Obj *env);

// Performance reporting
void print_gc_stats(VM *vm);

#endif
