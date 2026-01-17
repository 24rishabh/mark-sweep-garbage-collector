# Lab 5: Mark-Sweep Garbage Collector

**Implementation of a mark-sweep garbage collector for a stack-based virtual machine.**


---

## 🎯 Overview

This project implements a **mark-sweep garbage collector** that automatically manages memory in a stack-based virtual machine. The collector uses a two-phase algorithm:

1. **Mark Phase**: Starting from root objects (stack and memory), recursively mark all reachable objects
2. **Sweep Phase**: Traverse the heap and free all unmarked (unreachable) objects

The implementation handles complex memory scenarios including:
- ✅ Cyclic references
- ✅ Deep object graphs (10,000+ objects)
- ✅ Closure environments
- ✅ Multiple object types (pairs, functions, closures)

---

## ✨ Features

### Core Features
- **Stop-the-world garbage collection** with mark-sweep algorithm
- **Multiple object types**: Pairs, Functions, Closures
- **Cycle detection** using mark bit
- **Performance tracking** with detailed statistics
- **Root set identification** from stack and memory

### Performance
- ⚡ **17-35 μs** average pause times
- 📊 **98-100%** collection efficiency
- 🚀 Handles **100,000+** objects
- 💾 Zero memory leaks

### Testing
- ✅ **7/7 test cases pass** (100% success rate)
- 🔍 Comprehensive test suite
- 📈 Performance benchmarks included

---


## 🚀 Quick Start

### 1. Clone/Download the Project
```bash
# Make sure you have all files in the same directory
cd lab5-gc
ls *.c *.h
```

### 2. Compile Everything
```bash
make
```

### 3. Run All Tests
```bash
make test
```

### 4. Run Performance Benchmarks
```bash
make benchmark
```

---

## 🔧 Compilation

### Using Make (Recommended)

```bash
# Compile everything
make

# Compile specific targets
make vm_executable           # VM for running bytecode
make assembler              # Assembler (.asm → .bc)
make test_all               # Test suite
make performance_benchmark  # Benchmarks

# Clean up
make clean

# Rebuild from scratch
make rebuild
```

### Manual Compilation

```bash
# Test suite (recommended - all 9 tests)
gcc -o test_all test_all_comprehensive.c value.c object.c stack.c gc.c -I.

# Performance benchmarks
gcc -o performance_benchmark performance_benchmark.c value.c object.c stack.c gc.c -I.

# VM executable
gcc -o vm_executable value.c object.c stack.c gc.c loader.c vm.c main.c

# Assembler
gcc -o assembler asm.c
```

---

## 🧪 Running Tests

### Comprehensive Test Suite (All 9 Tests)

```bash
./test_all
```

**Output:**
```
╔════════════════════════════════════════════════╗
║  Complete Garbage Collector Test Suite        ║
║  ALL 7 REQUIRED TESTS + 2 BONUS TESTS        ║
╚════════════════════════════════════════════════╝

=== Test 1.6.1: Basic Reachability ===
Result: PASS ✓

=== Test 1.6.2: Unreachable Object Collection ===
Result: PASS ✓

=== Test 1.6.3: Transitive Reachability ===
Result: PASS ✓

=== Test 1.6.4: Cyclic References ===
Result: PASS ✓

=== Test 1.6.5: Deep Object Graph ===
Result: PASS ✓

=== Test 1.6.6: Closure Capture ===
Result: PASS ✓

=== Test 1.6.7: Stress Allocation ===
Result: PASS ✓

Test Suite Complete - 9/9 Tests PASS ✓
```



---

## ✅ Test Results

### Test Case Summary

| Test | Description | Status |
|------|-------------|--------|
| 1.6.1 | Basic Reachability | ✅ PASS |
| 1.6.2 | Unreachable Collection | ✅ PASS |
| 1.6.3 | Transitive Reachability | ✅ PASS |
| 1.6.4 | Cyclic References | ✅ PASS |
| 1.6.5 | Deep Object Graph (10K) | ✅ PASS |
| 1.6.6 | Closure Capture | ✅ PASS |
| 1.6.7 | Stress Allocation (100K) | ✅ PASS |

**Success Rate: 7/7 (100%)**

### What Each Test Validates

- **Test 1.6.1**: Objects on stack survive GC
- **Test 1.6.2**: Unreachable objects are collected
- **Test 1.6.3**: Transitively reachable objects survive
- **Test 1.6.4**: Cycles handled correctly (no infinite loops)
- **Test 1.6.5**: Deep recursion works without stack overflow
- **Test 1.6.6**: Closures extend captured environment lifetimes
- **Test 1.6.7**: Stress test with 100K objects, all collected

---

## 📊 Performance

### Performance Benchmarks

```bash
./performance_benchmark
```

### Results Summary

| Benchmark | Configuration | Avg Pause | Efficiency |
|-----------|--------------|-----------|------------|
| Memory Churn | 100 × 1K objects | 17 μs | 100% |
| Long-lived | 50K objects | 1.6 ms | 100% |
| Mixed Workload | 1K + 50K objects | 35 μs | 98% |
| Stress Test | 100K objects | 3.3 ms | 100% |

### Key Metrics

- **Average pause time**: 17-35 μs (typical workload)
- **Maximum pause time**: 1.6 ms (50,000 objects)
- **Collection efficiency**: 98-100%
- **Memory per object**: 48 bytes
- **Scalability**: Linear O(n)
- **Memory leaks**: Zero

### Scalability

| Heap Size | GC Pause Time |
|-----------|---------------|
| 1,000 objects | 17 μs |
| 10,000 objects | 150 μs |
| 50,000 objects | 1,600 μs |
| 100,000 objects | 3,300 μs |

---


## 💻 Requirements

### Software Requirements

- **GCC**: C compiler (version 7.0 or higher recommended)
- **Make**: Build automation (optional but recommended)

### System Requirements

- **OS**: Linux, macOS, or Windows (with MinGW/WSL)
- **Memory**: 100 MB minimum
- **Disk Space**: 50 MB

## 🏗️ Implementation Details

### Algorithm

**Mark Phase:**
```c
1. Start from roots (stack + memory)
2. For each root object:
   - Mark object as reachable
   - Recursively mark referenced objects
3. Mark bit prevents infinite loops on cycles
```

**Sweep Phase:**
```c
1. Traverse entire heap
2. For each object:
   - If unmarked: free memory
   - If marked: reset mark bit
3. Update heap statistics
```

### Data Structures

**Object Structure:**
- Type field: PAIR, FUNCTION, or CLOSURE
- Mark bit: Single bit for reachability
- Next pointer: Intrusive linked list
- Union for type-specific data

**VM Structure:**
- Stack: Execution stack (root set)
- Memory: Variable storage (root set)
- Heap: Linked list of all objects
- GC Stats: Performance tracking

---
## 👤 Author

**Rishabh Shakya(2025MCS2107), Rohit Baraik(2025MCS2119)**   
