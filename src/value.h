#ifndef VALUE_H
#define VALUE_H

typedef struct Obj Obj;

typedef enum{
    VAL_INT,
    VAL_OBJ
} ValueType;

typedef struct{
    ValueType type;
    union{
        int i;
        Obj *obj;
    }as;
}Value;

Value make_int_value(int val);
Value make_obj_value(Obj *obj);

#endif