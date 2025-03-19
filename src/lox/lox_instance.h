#ifndef CODECRAFTERS_INTERPRETER_LOX_INSTANCE_H
#define CODECRAFTERS_INTERPRETER_LOX_INSTANCE_H

#include "lox/lox_class.h"
#include "lox/object.h"

typedef struct {
        LoxClass *class;
} LoxInstance;

LoxInstance *lox_instance_construct(LoxClass *class);
const char *lox_instance_to_string(const LoxInstance *instance);

Object *lox_instance_object_construct(LoxInstance *instance);

#endif
