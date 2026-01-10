#include "vm.h"
#include<stdio.h>
#include "loader.h"
#include<stdlib.h>
#include<time.h>
#include "value.h"

int main(int argc, char *argv[]){
    if(argc<2){
        printf("Usage: %s <bytecode_file>\n", argv[0]);
        return 1;
    }

    int code_size;
    int *bytecode = load_bytecode(argv[1],&code_size);
    if(!bytecode) return 1;

    VM vm;
    vm_init(&vm,bytecode);
    clock_t start = clock();
    vm_run(&vm);
    clock_t end = clock();
    double exec_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %.6f seconds\n", exec_time);
    printf("Instructions executed: %ld\n", vm.instruction_count);
    if(vm.stack.sp>=0){
        Value result = pop(&vm.stack);
        printf("Result: %d\n",result.as.i);
    }
    else{
        printf("Stack empty at the execution\n");
    }

    free(bytecode);
    return 0;
}