# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -g -I./src

# Directories
SRC_DIR = src

# Targets
VM  = vm
ASM = asm

# Source files
VM_SRC  = $(SRC_DIR)/main.c \
          $(SRC_DIR)/vm.c \
          $(SRC_DIR)/stack.c \
          $(SRC_DIR)/loader.c \
          $(SRC_DIR)/value.c

ASM_SRC = $(SRC_DIR)/asm.c

# Default target
all: $(VM) $(ASM)

# Build virtual machine
$(VM): $(VM_SRC)
	$(CC) $(CFLAGS) $^ -o $@

# Build assembler
$(ASM): $(ASM_SRC)
	$(CC) $(CFLAGS) $^ -o $@

# Run VM with full test
run: $(VM) $(ASM)
	./$(ASM) full_test.asm full_test.bc
	./$(VM) full_test.bc

# Clean build files
clean:
	rm -f $(VM) $(ASM) *.bc *.o

.PHONY: all clean run
