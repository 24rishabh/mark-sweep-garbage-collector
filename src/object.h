#ifndef OBJECT_H
#define OBJECT_H

#include "value.h"

typedef enum{
    OBJ_PAIR,
    OBJ_FUNCTION,
    OBJ_CLOSURE
}ObjType;

typedef struct Obj{
    ObjType type;
    int marked;
    struct Obj *next;
    union{
        struct{
            Value left;
            Value right;
        }pair;
        
        struct{
            int address;        // Bytecode address
            int arity;          // Number of parameters
        }function;
        
        struct{
            struct Obj *function;  // The function object
            struct Obj *env;       // Captured environment
        }closure;
    }as;
}Obj;

#endif
