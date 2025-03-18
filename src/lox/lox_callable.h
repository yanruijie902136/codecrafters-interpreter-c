#ifndef CODECRAFTERS_INTERPRETER_LOX_LOX_CALLABLE_H
#define CODECRAFTERS_INTERPRETER_LOX_LOX_CALLABLE_H

#include <stddef.h>

#include "lox/environment.h"
#include "lox/object.h"
#include "lox/stmt.h"
#include "util/vector.h"

typedef enum {
        LOX_CALLABLE_CLOCK,
        LOX_CALLABLE_FUNCTION,
} LoxCallableType;

typedef struct {
        LoxCallableType type;
} LoxCallable;

typedef struct {
        LoxCallable base;
} LoxClock;

typedef struct {
        LoxCallable base;
        const FunctionStmt *declaration;
        Environment *closure;
} LoxFunction;

LoxClock *lox_clock_construct(void);
LoxFunction *lox_function_construct(const FunctionStmt *declaration, Environment *closure);
const char *lox_callable_to_string(const LoxCallable *callable);
size_t lox_callable_arity(const LoxCallable *callable);
Object *lox_callable_call(LoxCallable *callable, Vector *arguments);

Object *lox_callable_object_construct(LoxCallable *callable);
bool object_is_lox_callable(const Object *object);
LoxCallable *object_as_lox_callable(const Object *object);

#endif
