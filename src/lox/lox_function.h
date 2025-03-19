#ifndef CODECRAFTERS_INTERPRETER_LOX_FUNCTION_H
#define CODECRAFTERS_INTERPRETER_LOX_FUNCTION_H

#include "lox/environment.h"
#include "lox/lox_callable.h"
#include "lox/object.h"
#include "lox/stmt.h"
#include "util/vector.h"

typedef struct {
        LoxCallable base;
        const FunctionStmt *declaration;
        Environment *closure;
        bool is_initializer;
} LoxFunction;

LoxFunction *lox_function_construct(const FunctionStmt *declaration, Environment *closure, bool is_initializer);
const char *lox_function_to_string(const LoxFunction *function);
size_t lox_function_arity(const LoxFunction *function);
Object *lox_function_call(LoxFunction *function, Vector *arguments);

#endif
