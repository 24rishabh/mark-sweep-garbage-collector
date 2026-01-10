#ifndef OBJECT_H
#define OBJECT_H

#include "value.h"

typedef enum{
    OBJ_PAIR
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
    }as;
}obj;

#endif