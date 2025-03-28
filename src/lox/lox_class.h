#ifndef CODECRAFTERS_INTERPRETER_LOX_CLASS_H
#define CODECRAFTERS_INTERPRETER_LOX_CLASS_H

#include "lox/lox_callable.h"
#include "lox/lox_function.h"
#include "lox/object.h"
#include "util/map.h"
#include "util/vector.h"

typedef struct LoxClass LoxClass;
struct LoxClass {
        LoxCallable base;
        const char *name;
        LoxClass *superclass;
        Map *methods;
};

LoxClass *lox_class_construct(const char *name, LoxClass *superclass, Map *methods);
const char *lox_class_to_string(const LoxClass *class);
size_t lox_class_arity(const LoxClass *class);
Object *lox_class_call(LoxClass *class, Vector *arguments);

LoxFunction *lox_class_find_method(const LoxClass *class, const char *name);

#endif
