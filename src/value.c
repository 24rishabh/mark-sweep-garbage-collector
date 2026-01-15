#include "value.h"

Value make_int_value(int v){
    Value val;
    val.type = VAL_INT;
    val.as.i = v;
    return val;
}

Value make_obj_value(Obj *object){
    Value val;
    val.type = VAL_OBJ;
    val.as.obj = object;
    return val;
}