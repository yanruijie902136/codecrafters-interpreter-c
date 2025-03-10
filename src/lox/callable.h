#ifndef CODECRAFTERS_INTERPRETER_CALLABLE_H
#define CODECRAFTERS_INTERPRETER_CALLABLE_H

#include <stddef.h>

#include "lox/environment.h"
#include "lox/ptr_vector.h"
#include "lox/stmt.h"

typedef enum {
        CALLABLE_CLOCK,
        CALLABLE_FUNCTION,
} CallableType;

typedef struct {
        CallableType type;
        const void *data;
} Callable;

typedef struct {
        Environment *closure;
        const FunctionStmt *declaration;
} Function;

Callable *clock_create(void);
Callable *function_create(Environment *closure, const FunctionStmt *declaration);
Callable *callable_copy(const Callable *callable);
size_t callable_arity(const Callable *callable);
void *callable_call(Callable *callable, const PtrVector *arguments);
const char *callable_stringify(const Callable *callable);

#endif
