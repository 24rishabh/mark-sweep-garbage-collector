# Makefile for Lab 5: Mark-Sweep Garbage Collector

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Isrc

# Source directory
SRC_DIR = src
BENCH_DIR = benchmark

# Targets
VM = vm
ASM = asm

# Source files
VM_SRC = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/vm.c \
	$(SRC_DIR)/stack.c \
	$(SRC_DIR)/loader.c \
	$(SRC_DIR)/value.c \
	$(SRC_DIR)/object.c

ASM_SRC = $(SRC_DIR)/asm.c

# Default target
all: $(VM) $(ASM)

# Build Virtual Machine
$(VM): $(VM_SRC)
	$(CC) $(CFLAGS) $(VM_SRC) -o $(VM)

# Assembler (converts .asm to .bc)
$(ASSEMBLER): $(SRC_DIR)/asm.c
	$(CC) $(CFLAGS) -o $(ASSEMBLER) $(SRC_DIR)/asm.c

# Comprehensive test suite (all 9 tests in one file)
$(TEST_ALL): $(SRC_DIR)/test_all_comprehensive.c $(CORE_SOURCES)
	$(CC) $(CFLAGS) -o $(TEST_ALL) $(SRC_DIR)/test_all_comprehensive.c $(CORE_SOURCES)

# Performance benchmark
$(PERF_BENCH): $(SRC_DIR)/performance_benchmark.c $(CORE_SOURCES)
	$(CC) $(CFLAGS) -o $(PERF_BENCH) $(SRC_DIR)/performance_benchmark.c $(CORE_SOURCES)

# Optional: Individual test programs (if you want them)
$(SRC_DIR)/test_gc_suite: $(SRC_DIR)/test_gc_suite.c $(CORE_SOURCES)
	$(CC) $(CFLAGS) -o $(SRC_DIR)/test_gc_suite $(SRC_DIR)/test_gc_suite.c $(CORE_SOURCES)

$(SRC_DIR)/test_closure: $(SRC_DIR)/test_closure_capture.c $(CORE_SOURCES)
	$(CC) $(CFLAGS) -o $(SRC_DIR)/test_closure $(SRC_DIR)/test_closure_capture.c $(CORE_SOURCES)

$(SRC_DIR)/test_memory: $(SRC_DIR)/test_memory_roots.c $(CORE_SOURCES)
	$(CC) $(CFLAGS) -o $(SRC_DIR)/test_memory $(SRC_DIR)/test_memory_roots.c $(CORE_SOURCES)

# Run all tests
test: $(TEST_ALL)
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║  Running Comprehensive Test Suite             ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	cd $(SRC_DIR) && ./test_all
	@echo ""
	@echo "✅ Test suite completed!"

# Run performance benchmarks
benchmark: $(PERF_BENCH)
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║  Running Performance Benchmarks               ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	cd $(SRC_DIR) && ./performance_benchmark
	@echo ""
	@echo "✅ Benchmarks completed!"

# Run both tests and benchmarks
evaluate: test benchmark
	@echo ""
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║  Complete Evaluation Finished                 ║"
	@echo "╚════════════════════════════════════════════════╝"

# Generate report data (output in src/report_output/)
report: $(TEST_ALL) $(PERF_BENCH)
	@echo "Generating report data..."
	@mkdir -p $(SRC_DIR)/report_output
	cd $(SRC_DIR) && ./test_all > report_output/test_results.txt 2>&1
	cd $(SRC_DIR) && ./performance_benchmark > report_output/benchmark_results.txt 2>&1
	@echo "✅ Report data saved to src/report_output/"
	@echo "   - test_results.txt"
	@echo "   - benchmark_results.txt"

# Clean up compiled files
clean:
	rm -f $(SRC_DIR)/*.o
	rm -f $(EXECUTABLES)
	rm -f $(SRC_DIR)/test_gc_suite $(SRC_DIR)/test_closure $(SRC_DIR)/test_memory
	rm -f $(SRC_DIR)/*.bc
	rm -rf $(SRC_DIR)/report_output
	@echo "✅ Cleaned up all compiled files"

# Clean and rebuild everything
rebuild: clean all

# Show help
help:
	@echo "Lab 5 Garbage Collector - Makefile Commands"
	@echo ""
	@echo "Build Commands:"
	@echo "  make             - Compile all programs"
	@echo "  make all         - Same as 'make'"
	@echo "  make clean       - Remove compiled files"
	@echo "  make rebuild     - Clean and rebuild everything"
	@echo ""
	@echo "Test Commands:"
	@echo "  make test        - Run all test cases"
	@echo "  make benchmark   - Run performance benchmarks"
	@echo "  make evaluate    - Run tests + benchmarks"
	@echo "  make report      - Generate report data files"
	@echo "Note: All source files are in src/"
	@echo "      All executables output to src/"
	@echo "      Report data goes to src/report_output/"

# Phony targets (not actual files)
.PHONY: all test benchmark evaluate report asm_test clean rebuild help