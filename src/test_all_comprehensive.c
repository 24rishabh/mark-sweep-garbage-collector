#include "vm.h"
#include "object.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

// Helper to initialize VM for testing (without bytecode)
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

// Helper to print test results
void print_test_result(const char *test_name, int passed) {
    printf("\n=== %s ===\n", test_name);
    printf("Result: %s\n", passed ? "PASS ✓" : "FAIL ✗");
}

void test_basic_reachability() {
    VM vm;
    test_vm_init(&vm);
    
    // Create pair and push to stack
    Obj *a = new_pair(&vm, make_int_value(10), make_int_value(20));
    push(&vm.stack, make_obj_value(a));
    
    int heap_before = vm.heap_size;
    gc(&vm);
    int heap_after = vm.heap_size;
    
    // Object should survive
    int passed = (heap_after == 1) && (heap_before == heap_after);
    print_test_result("Test 1.6.1: Basic Reachability", passed);
    printf("Heap before: %d, after: %d\n", heap_before, heap_after);
    printf("Expected: Object 'a' survives, heap remains at 1 object\n");
}

void test_unreachable_collection() {
    VM vm;
    test_vm_init(&vm);
    
    // Create pair but don't keep reference
    new_pair(&vm, make_int_value(10), make_int_value(20));
    
    int heap_before = vm.heap_size;
    gc(&vm);
    int heap_after = vm.heap_size;
    
    // Object should be collected
    int passed = (heap_before == 1) && (heap_after == 0);
    print_test_result("Test 1.6.2: Unreachable Object Collection", passed);
    printf("Heap before: %d, after: %d\n", heap_before, heap_after);
    printf("Expected: Object 'a' is freed, heap is empty\n");
}

void test_transitive_reachability() {
    VM vm;
    test_vm_init(&vm);
    
    // Create pair a
    Obj *a = new_pair(&vm, make_int_value(10), make_int_value(20));
    
    // Create pair b that references a
    Obj *b = new_pair(&vm, make_obj_value(a), make_int_value(30));
    
    // Only push b to stack
    push(&vm.stack, make_obj_value(b));
    
    int heap_before = vm.heap_size;
    gc(&vm);
    int heap_after = vm.heap_size;
    
    // Both objects should survive
    int passed = (heap_before == 2) && (heap_after == 2);
    print_test_result("Test 1.6.3: Transitive Reachability", passed);
    printf("Heap before: %d, after: %d\n", heap_before, heap_after);
    printf("Expected: Both 'a' and 'b' survive (b references a)\n");
}

void test_cyclic_references() {
    VM vm;
    test_vm_init(&vm);
    
    // Create pair a
    Obj *a = new_pair(&vm, make_int_value(10), make_int_value(0));
    
    // Create pair b that references a
    Obj *b = new_pair(&vm, make_obj_value(a), make_int_value(20));
    
    // Create cycle: a.right = b
    a->as.pair.right = make_obj_value(b);
    
    // Push a to stack
    push(&vm.stack, make_obj_value(a));
    
    int heap_before = vm.heap_size;
    gc(&vm);
    int heap_after = vm.heap_size;
    
    // Both objects should survive despite cycle
    int passed = (heap_before == 2) && (heap_after == 2);
    print_test_result("Test 1.6.4: Cyclic References", passed);
    printf("Heap before: %d, after: %d\n", heap_before, heap_after);
    printf("Expected: Both objects survive (a->b->a cycle)\n");
}

void test_deep_graph() {
    VM vm;
    test_vm_init(&vm);
    
    printf("\n=== Test 1.6.5: Deep Object Graph ===\n");
    printf("Creating chain of 10,000 objects...\n");
    
    // Create root
    Obj *root = new_pair(&vm, make_int_value(0), make_int_value(0));
    Obj *current = root;
    
    // Create chain
    for (int i = 1; i < 10000; i++) {
        Obj *next = new_pair(&vm, make_int_value(0), make_int_value(0));
        current->as.pair.right = make_obj_value(next);
        current = next;
        
        if (i % 2000 == 0) {
            printf("  Created %d objects...\n", i);
        }
    }
    
    // Push root to stack
    push(&vm.stack, make_obj_value(root));
    
    int heap_before = vm.heap_size;
    printf("Running GC on %d objects...\n", heap_before);
    
    gc(&vm);
    
    int heap_after = vm.heap_size;
    
    // All objects should survive
    int passed = (heap_before == 10000) && (heap_after == 10000);
    printf("Result: %s\n", passed ? "PASS ✓" : "FAIL ✗");
    printf("Heap before: %d, after: %d\n", heap_before, heap_after);
    printf("Expected: All 10,000 objects survive, no stack overflow\n");
    printf("GC pause time: %.6f seconds\n", vm.gc_stats.max_gc_pause);
}

void test_closure_capture() {
    VM vm;
    test_vm_init(&vm);
    
    printf("\n=== Test 1.6.6: Closure Capture ===\n");
    
    // Create environment (captured variable)
    Obj *env = new_pair(&vm, make_int_value(42), make_int_value(0));
    printf("Created environment (pair with value 42)\n");
    
    // Create function
    Obj *fn = new_function(&vm, 100, 1);  // address=100, arity=1
    printf("Created function (address=100, arity=1)\n");
    
    // Create closure that captures environment
    Obj *cl = new_closure(&vm, fn, env);
    printf("Created closure (captures function and environment)\n");
    
    // Push closure to stack
    push(&vm.stack, make_obj_value(cl));
    
    int heap_before = vm.heap_size;
    printf("Heap before GC: %d objects (env, fn, closure)\n", heap_before);
    
    // Run GC - all three objects should survive
    gc(&vm);
    
    int heap_after = vm.heap_size;
    printf("Heap after GC: %d objects\n", heap_after);
    
    // All three objects should survive (env, function, closure)
    int passed = (heap_before == 3) && (heap_after == 3);
    printf("Result: %s\n", passed ? "PASS ✓" : "FAIL ✗");
    printf("Expected: Closure, function, and environment survive\n");
    
    if (!passed) {
        printf("ERROR: Expected 3 objects before and after GC\n");
        printf("Got: %d before, %d after\n", heap_before, heap_after);
    }
}

void test_stress_allocation() {
    VM vm;
    test_vm_init(&vm);
    
    printf("\n=== Test 1.6.7: Stress Allocation ===\n");
    printf("Allocating 100,000 objects without references...\n");
    
    // Allocate many objects without keeping references
    for (int i = 0; i < 100000; i++) {
        new_pair(&vm, make_int_value(0), make_int_value(0));
        
        if (i % 20000 == 0 && i > 0) {
            printf("  Allocated %d objects, heap size: %d\n", i, vm.heap_size);
        }
    }
    
    int heap_before = vm.heap_size;
    printf("Running GC on %d objects...\n", heap_before);
    
    gc(&vm);
    
    int heap_after = vm.heap_size;
    
    // All objects should be collected
    int passed = (heap_after == 0);
    printf("Result: %s\n", passed ? "PASS ✓" : "FAIL ✗");
    printf("Heap before: %d, after: %d\n", heap_before, heap_after);
    printf("Expected: Heap is empty (all objects collected)\n");
    printf("GC pause time: %.6f seconds\n", vm.gc_stats.max_gc_pause);
    
}


void test_memory_stored_objects() {
    VM vm;
    test_vm_init(&vm);
    
    printf("\n=== BONUS: Memory-Stored Objects ===\n");
    
    // Create an object
    Obj *obj = new_pair(&vm, make_int_value(42), make_int_value(99));
    printf("Created object: pair(42, 99)\n");
    
    // Store it in memory (not on stack!)
    vm.memory[0] = make_obj_value(obj);
    vm.valid[0] = 1;
    printf("Stored object in memory[0]\n");
    
    // Stack is empty - object only referenced from memory
    printf("Stack is empty (sp = %d)\n", vm.stack.sp);
    
    int heap_before = vm.heap_size;
    printf("Heap before GC: %d objects\n", heap_before);
    
    // Run GC - object should survive because it's in memory
    gc(&vm);
    
    int heap_after = vm.heap_size;
    printf("Heap after GC: %d objects\n", heap_after);
    
    // Check result
    int passed = (heap_before == 1) && (heap_after == 1);
    printf("Result: %s\n", passed ? "PASS ✓" : "FAIL ✗");
    printf("Expected: Object survives (referenced from memory)\n");
    
    if (!passed) {
        printf("ERROR: Object should survive when stored in memory!\n");
        printf("This means memory is not being marked as a root.\n");
    }
}

void test_multiple_memory_objects() {
    VM vm;
    test_vm_init(&vm);
    
    printf("\n=== BONUS: Multiple Objects in Memory ===\n");
    
    // Create multiple objects
    Obj *obj1 = new_pair(&vm, make_int_value(1), make_int_value(2));
    Obj *obj2 = new_pair(&vm, make_int_value(3), make_int_value(4));
    Obj *obj3 = new_pair(&vm, make_int_value(5), make_int_value(6));
    
    printf("Created 3 objects\n");
    
    // Store two in memory
    vm.memory[0] = make_obj_value(obj1);
    vm.valid[0] = 1;
    vm.memory[5] = make_obj_value(obj2);
    vm.valid[5] = 1;
    // obj3 is not stored anywhere - should be collected
    
    printf("Stored obj1 in memory[0]\n");
    printf("Stored obj2 in memory[5]\n");
    printf("obj3 is unreferenced\n");
    
    int heap_before = vm.heap_size;
    printf("Heap before GC: %d objects\n", heap_before);
    
    gc(&vm);
    
    int heap_after = vm.heap_size;
    printf("Heap after GC: %d objects\n", heap_after);
    
    // Two should survive, one collected
    int passed = (heap_before == 3) && (heap_after == 2);
    printf("Result: %s\n", passed ? "PASS ✓" : "FAIL ✗");
    printf("Expected: 2 objects survive (obj1, obj2), 1 collected (obj3)\n");
    
    if (!passed) {
        printf("ERROR: Expected 3 before, 2 after. Got %d before, %d after\n", 
               heap_before, heap_after);
    }
}

int main() {
    
    test_basic_reachability();
    test_unreachable_collection();
    test_transitive_reachability();
    test_cyclic_references();
    test_deep_graph();
    test_closure_capture();
    test_stress_allocation();
    
    printf("\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  BONUS TEST CASES (Memory Root Set)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    test_memory_stored_objects();
    test_multiple_memory_objects();
    
    
    return 0;
}
