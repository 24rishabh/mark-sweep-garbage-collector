# Makefile for Lab 5: Mark-Sweep Garbage Collector
CC = gcc
CFLAGS = -Wall -Wextra -I.
DEBUG_FLAGS = -g -DGC_DEBUG

# Source files
CORE_SOURCES = value.c object.c stack.c gc.c
VM_SOURCES = $(CORE_SOURCES) loader.c vm.c main.c
TEST_SOURCES = $(CORE_SOURCES)
HEADERS = value.h object.h stack.h vm.h loader.h

# Executables
VM_EXEC = vm_executable
ASSEMBLER = assembler
TEST_ALL = test_all
TEST_PERFORMANCE = performance_benchmark

# Default: compile everything
all: $(VM_EXEC) $(ASSEMBLER) $(TEST_ALL) $(TEST_PERFORMANCE)
	@echo "✅ All programs compiled successfully!"
	@echo "Run 'make test' to execute all tests"

# VM executable
$(VM_EXEC): $(VM_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(VM_EXEC) $(VM_SOURCES)

# Assembler
$(ASSEMBLER): asm.c
	$(CC) $(CFLAGS) -o $(ASSEMBLER) asm.c

# Comprehensive test suite (ALL 9 TESTS)
$(TEST_ALL): test_all_comprehensive.c $(TEST_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TEST_ALL) test_all_comprehensive.c $(TEST_SOURCES)

# Performance benchmarks
$(TEST_PERFORMANCE): performance_benchmark.c $(TEST_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TEST_PERFORMANCE) performance_benchmark.c $(TEST_SOURCES)

# Run all tests
test: $(TEST_ALL) $(TEST_PERFORMANCE)
	@echo "Running comprehensive test suite..."
	@./$(TEST_ALL)
	@echo ""
	@echo "Running performance benchmarks..."
	@./$(TEST_PERFORMANCE)

# Run only comprehensive tests
test-quick: $(TEST_ALL)
	@./$(TEST_ALL)

# Run only benchmarks
benchmark: $(TEST_PERFORMANCE)
	@./$(TEST_PERFORMANCE)

# Save test results to files
test-save: $(TEST_ALL) $(TEST_PERFORMANCE)
	@./$(TEST_ALL) > test_results.txt 2>&1
	@./$(TEST_PERFORMANCE) > benchmark_results.txt 2>&1
	@echo "✅ Results saved to test_results.txt and benchmark_results.txt"

# Memory leak check
valgrind: $(TEST_ALL)
	valgrind --leak-check=full ./$(TEST_ALL)

# Assembly tests
run-basic: $(VM_EXEC) $(ASSEMBLER)
	./$(ASSEMBLER) test_basic.asm test_basic.bc
	./$(VM_EXEC) test_basic.bc

run-unreachable: $(VM_EXEC) $(ASSEMBLER)
	./$(ASSEMBLER) test_unreachable.asm test_unreachable.bc
	./$(VM_EXEC) test_unreachable.bc

run-transitive: $(VM_EXEC) $(ASSEMBLER)
	./$(ASSEMBLER) test_transitive.asm test_transitive.bc
	./$(VM_EXEC) test_transitive.bc

run-cyclic: $(VM_EXEC) $(ASSEMBLER)
	./$(ASSEMBLER) test_cyclic.asm test_cyclic.bc
	./$(VM_EXEC) test_cyclic.bc

run-stress: $(VM_EXEC) $(ASSEMBLER)
	./$(ASSEMBLER) test_stress.asm test_stress.bc
	./$(VM_EXEC) test_stress.bc

# Clean
clean:
	rm -f $(VM_EXEC) $(ASSEMBLER) $(TEST_ALL) $(TEST_PERFORMANCE)
	rm -f *.o *.bc test_results.txt benchmark_results.txt

# Rebuild
rebuild: clean all

# Help
help:
	@echo "Lab 5 Makefile Commands:"
	@echo ""
	@echo "  make              - Compile all programs"
	@echo "  make test         - Run all tests"
	@echo "  make test-quick   - Run only comprehensive tests (9 tests)"
	@echo "  make benchmark    - Run only performance benchmarks"
	@echo "  make test-save    - Save test results to files"
	@echo "  make valgrind     - Run with memory leak detection"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make help         - Show this message"

.PHONY: all test test-quick benchmark test-save debug valgrind \
        run-basic run-unreachable run-transitive run-cyclic run-stress \
        clean rebuild help
