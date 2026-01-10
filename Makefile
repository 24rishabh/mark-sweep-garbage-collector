# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets
VM = vm
ASM = asm

# Source files
VM_SRC = main.c vm.c stack.c loader.c value.c
ASM_SRC = asm.c

# Default target
all: $(VM) $(ASM)

# Build virtual machine
$(VM): $(VM_SRC)
	$(CC) $(CFLAGS) $(VM_SRC) -o $(VM)

# Build assembler
$(ASM): $(ASM_SRC)
	$(CC) $(CFLAGS) $(ASM_SRC) -o $(ASM)

# Run VM with full test
run: $(VM) $(ASM)
	./asm full_test.asm full_test.bc
	./$(VM) full_test.bc

# Clean build files
clean:
	rm -f $(VM) $(ASM) *.bc *.o

.PHONY: all clean run
