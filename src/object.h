#ifndef OBJECT_H
#define OBJECT_H

#include "value.h"

typedef enum{
    OBJ_PAIR
}ObjType;

typedef struct Obj{
    ObjType type;
    struct Obj *next;
    union{
        struct{
            value left;
            value right;
        }pair;
    }as;
}obj;

#endif