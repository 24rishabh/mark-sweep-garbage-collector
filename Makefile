# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
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

# Build Assembler
$(ASM): $(ASM_SRC)
	$(CC) $(CFLAGS) $(ASM_SRC) -o $(ASM)

# Run target with input ASM file
# Usage: make run FILE=benchmark/program1.asm
run: $(VM) $(ASM)
ifndef FILE
	$(error Usage: make run FILE=path/to/program.asm)
endif
	./$(ASM) $(FILE) program.bc
	./$(VM) program.bc

# Clean generated files
clean:
	rm -f $(VM) $(ASM) *.bc *.o

.PHONY: all clean run
