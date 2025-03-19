#ifndef CODECRAFTERS_INTERPRETER_LOX_CLASS_H
#define CODECRAFTERS_INTERPRETER_LOX_CLASS_H

#include "lox/lox_callable.h"

typedef struct {
        LoxCallable base;
        const char *name;
} LoxClass;

LoxClass *lox_class_construct(const char *name);

#endif
