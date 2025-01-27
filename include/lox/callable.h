#ifndef CODECRAFTERS_INTERPRETER_CALLABLE_H
#define CODECRAFTERS_INTERPRETER_CALLABLE_H

#include "lox/ptr_vector.h"
#include <stddef.h>

typedef enum {
        CALLABLE_CLOCK,
} CallableType;

typedef struct {
        CallableType type;
        const void *data;
} Callable;

Callable *clock_create(void);
Callable *callable_copy(const Callable *callable);
size_t callable_arity(const Callable *callable);
void *callable_call(Callable *callable, const PtrVector *arguments);

#endif
