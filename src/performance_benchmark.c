#include "vm.h"
#include "object.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Helper to initialize VM for testing
void test_vm_init(VM *vm) {
    init_stack(&vm->stack);
    vm->bytecode = NULL;
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
    
    for(int i = 0; i < 1024; i++){
        vm->memory[i] = make_int_value(0);
        vm->valid[i] = 0;
    }
}

// Benchmark 1: Memory Churn (High allocation/collection rate)
void benchmark_memory_churn() {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark 1: Memory Churn                                ║\n");
    printf("║  Allocate and discard objects rapidly                     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    VM vm;
    test_vm_init(&vm);
    
    clock_t start = clock();
    
    // Allocate and discard in batches
    for (int batch = 0; batch < 100; batch++) {
        // Allocate 1000 objects
        for (int i = 0; i < 1000; i++) {
            new_pair(&vm, make_int_value(i), make_int_value(i*2));
        }
        
        // Trigger GC (all should be collected)
        gc(&vm);
        
        if (batch % 20 == 0) {
            printf("  Completed batch %d/100\n", batch);
        }
    }
    
    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("\nResults:\n");
    printf("  Total batches:              100\n");
    printf("  Objects per batch:          1,000\n");
    printf("  Total objects created:      %ld\n", vm.gc_stats.total_objects_allocated);
    printf("  Total execution time:       %.6f seconds\n", total_time);
    printf("  Time per batch:             %.6f seconds\n", total_time / 100);
    
    print_gc_stats(&vm);
}

// Benchmark 2: Long-lived Objects (Low collection rate)
void benchmark_long_lived_objects() {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark 2: Long-lived Objects                          ║\n");
    printf("║  Create objects and keep references                       ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    VM vm;
    test_vm_init(&vm);
    
    clock_t start = clock();
    
    printf("  Creating 50,000 long-lived objects...\n");
    
    // Create a large linked structure that stays alive
    Obj *root = new_pair(&vm, make_int_value(0), make_int_value(0));
    Obj *current = root;
    
    for (int i = 1; i < 50000; i++) {
        Obj *next = new_pair(&vm, make_int_value(i), make_int_value(0));
        current->as.pair.right = make_obj_value(next);
        current = next;
        
        if (i % 10000 == 0) {
            printf("    Created %d objects...\n", i);
        }
    }
    
    // Keep root on stack
    push(&vm.stack, make_obj_value(root));
    
    printf("  Running periodic GC...\n");
    
    // Run GC multiple times - objects should survive
    for (int i = 0; i < 10; i++) {
        gc(&vm);
        printf("    GC #%d: %d objects alive\n", i+1, vm.heap_size);
    }
    
    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("\nResults:\n");
    printf("  Objects created:            %ld\n", vm.gc_stats.total_objects_allocated);
    printf("  Objects surviving:          %d\n", vm.heap_size);
    printf("  Total execution time:       %.6f seconds\n", total_time);
    
    print_gc_stats(&vm);
}

// Benchmark 3: Mixed Workload (Some live, some garbage)
void benchmark_mixed_workload() {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark 3: Mixed Workload                              ║\n");
    printf("║  Mix of short-lived and long-lived objects                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    VM vm;
    test_vm_init(&vm);
    
    clock_t start = clock();
    
    // Create some long-lived objects
    printf("  Creating 1,000 long-lived objects...\n");
    Obj *root = new_pair(&vm, make_int_value(0), make_int_value(0));
    Obj *current = root;
    
    for (int i = 1; i < 1000; i++) {
        Obj *next = new_pair(&vm, make_int_value(i), make_int_value(0));
        current->as.pair.right = make_obj_value(next);
        current = next;
    }
    push(&vm.stack, make_obj_value(root));
    
    printf("  Running mixed allocation/collection cycles...\n");
    
    // Mix short-lived allocations with GC
    for (int cycle = 0; cycle < 50; cycle++) {
        // Allocate temporary objects
        for (int i = 0; i < 1000; i++) {
            new_pair(&vm, make_int_value(i), make_int_value(i*2));
        }
        
        // Run GC - should collect temps, keep long-lived
        gc(&vm);
        
        if (cycle % 10 == 0) {
            printf("    Cycle %d: heap=%d, collected=%ld\n", 
                   cycle, vm.heap_size, vm.gc_stats.total_objects_freed);
        }
    }
    
    clock_t end = clock();
    double total_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("\nResults:\n");
    printf("  Total objects created:      %ld\n", vm.gc_stats.total_objects_allocated);
    printf("  Objects collected:          %ld\n", vm.gc_stats.total_objects_freed);
    printf("  Objects surviving:          %d\n", vm.heap_size);
    printf("  Total execution time:       %.6f seconds\n", total_time);
    
    print_gc_stats(&vm);
}

// Benchmark 4: GC Overhead Measurement
void benchmark_gc_overhead() {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark 4: GC Overhead Analysis                        ║\n");
    printf("║  Measure time with and without GC                         ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    // Test WITH GC
    printf("  Test 1: WITH garbage collection\n");
    VM vm1;
    test_vm_init(&vm1);
    
    clock_t start1 = clock();
    for (int i = 0; i < 10000; i++) {
        new_pair(&vm1, make_int_value(i), make_int_value(i*2));
        if (i % 100 == 0) gc(&vm1);
    }
    clock_t end1 = clock();
    double time_with_gc = (double)(end1 - start1) / CLOCKS_PER_SEC;
    
    printf("    Allocated 10,000 objects with periodic GC\n");
    printf("    Time: %.6f seconds\n", time_with_gc);
    printf("    GC calls: %ld\n", vm1.gc_stats.total_gc_calls);
    printf("    Total GC time: %.6f seconds\n", vm1.gc_stats.total_gc_time);
    
    // Test WITHOUT GC (just allocation)
    printf("\n  Test 2: WITHOUT garbage collection\n");
    VM vm2;
    test_vm_init(&vm2);
    
    clock_t start2 = clock();
    for (int i = 0; i < 10000; i++) {
        new_pair(&vm2, make_int_value(i), make_int_value(i*2));
    }
    clock_t end2 = clock();
    double time_without_gc = (double)(end2 - start2) / CLOCKS_PER_SEC;
    
    printf("    Allocated 10,000 objects without GC\n");
    printf("    Time: %.6f seconds\n", time_without_gc);
    printf("    Final heap size: %d objects\n", vm2.heap_size);
    
    printf("\n  Analysis:\n");
    printf("    Allocation time:            %.6f seconds\n", time_without_gc);
    printf("    Allocation + GC time:       %.6f seconds\n", time_with_gc);
    printf("    GC overhead:                %.6f seconds (%.1f%%)\n", 
           vm1.gc_stats.total_gc_time,
           (vm1.gc_stats.total_gc_time / time_with_gc) * 100.0);
    print_gc_stats(&vm1);
}

int main() {

    
    benchmark_memory_churn();
    benchmark_long_lived_objects();
    benchmark_mixed_workload();
    benchmark_gc_overhead();

    
    return 0;
}
