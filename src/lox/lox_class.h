#ifndef CODECRAFTERS_INTERPRETER_LOX_CLASS_H
#define CODECRAFTERS_INTERPRETER_LOX_CLASS_H

#include "lox/lox_callable.h"
#include "lox/lox_function.h"
#include "lox/object.h"
#include "util/set.h"
#include "util/vector.h"

typedef struct {
        LoxCallable base;
        const char *name;
        Set *methods;
} LoxClass;

LoxClass *lox_class_construct(const char *name, Set *methods);
const char *lox_class_to_string(const LoxClass *class);
size_t lox_class_arity(const LoxClass *class);
Object *lox_class_call(LoxClass *class, Vector *arguments);

typedef struct {
        const char *name;
        LoxFunction *function;
} Method;

int method_compare(const void *m1, const void *m2);
Method *method_construct(const char *name, LoxFunction *function);

LoxFunction *lox_class_find_method(const LoxClass *class, const char *name);

#endif
