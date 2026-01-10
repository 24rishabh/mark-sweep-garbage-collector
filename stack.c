#include "stack.h"
#include <stdio.h>
#include<stdlib.h>  // exit(int status)
#include "value.h"

void init_stack(Stack *s){
    s->sp = -1;
}

void push(Stack *s,Value value){
    if(s->sp>=STACK_SIZE-1){
        printf("Stack Overflow\n");
        exit(1);
    }
    s->data[++s->sp] = value;
}

Value pop(Stack *s){
    if(s->sp<0){
        printf("Stack underflow\n");
        exit(1);
    }
    return s->data[s->sp--];
}

Value peek(Stack *s){
    if(s->sp<0){
        printf("Stack is empty\n");
        exit(1);
    }

    return s->data[s->sp];
}