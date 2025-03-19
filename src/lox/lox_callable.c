#include "lox/lox_callable.h"
#include "lox/environment.h"
#include "lox/interpreter.h"
#include "lox/lox_class.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <time.h>

LoxClass *lox_class_construct(const char *name) {
        LoxClass *lox_class = xmalloc(sizeof(LoxClass));
        lox_class->base.type = LOX_CALLABLE_CLASS;
        lox_class->name = name;
        return lox_class;
}

LoxClock *lox_clock_construct(void) {
        LoxClock *lox_clock = xmalloc(sizeof(LoxClock));
        lox_clock->base.type = LOX_CALLABLE_CLOCK;
        return lox_clock;
}

LoxFunction *lox_function_construct(const FunctionStmt *declaration, Environment *closure) {
        LoxFunction *lox_function = xmalloc(sizeof(LoxFunction));
        lox_function->base.type = LOX_CALLABLE_FUNCTION;
        lox_function->declaration = declaration;
        lox_function->closure = closure;
        return lox_function;
}

const char *lox_callable_to_string(const LoxCallable *callable) {
        static char str[256];
        switch (callable->type) {
        case LOX_CALLABLE_CLASS:
                return ((const LoxClass *)callable)->name;
        case LOX_CALLABLE_CLOCK:
                return "<native fn>";
        case LOX_CALLABLE_FUNCTION:
                snprintf(str, sizeof(str), "<fn %s>", ((const LoxFunction *)callable)->declaration->name->lexeme);
                return str;
        }
}

size_t lox_callable_arity(const LoxCallable *callable) {
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return 0;
        case LOX_CALLABLE_FUNCTION:
                return vector_size(((LoxFunction *)callable)->declaration->params);
        }
}

static Object *lox_function_call(LoxFunction *lox_function, Vector *arguments) {
        Environment *environment = environment_construct(lox_function->closure);

        size_t num_params = vector_size(lox_function->declaration->params);
        for (size_t i = 0; i < num_params; i++) {
                Token *param = vector_at(lox_function->declaration->params, i);
                environment_define(environment, param->lexeme, vector_at(arguments, i));
        }

        Object *result = execute_block(lox_function->declaration->body, environment);
        if (result == NULL) {
                result = nil_object_construct();
        }
        return result;
}

Object *lox_callable_call(LoxCallable *callable, Vector *arguments) {
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return number_object_construct(time(NULL));
        case LOX_CALLABLE_FUNCTION:
                return lox_function_call((LoxFunction *)callable, arguments);
        }
}
