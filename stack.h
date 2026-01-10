#ifndef STACK_H
#define STACK_H

#include "value.h"

#define STACK_SIZE 1024

typedef struct{
    Value data[STACK_SIZE];
    int sp; // Stack-Pointer
}Stack;

void init_stack(Stack *s);
void push(Stack *s,Value value);
Value pop(Stack *s);
Value peek(Stack *s);

#endif