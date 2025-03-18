#include "lox/lox_callable.h"
#include "lox/environment.h"
#include "lox/interpreter.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <time.h>

LoxClock *lox_clock_construct(void) {
        LoxClock *lox_clock = xmalloc(sizeof(LoxClock));
        lox_clock->base.type = LOX_CALLABLE_CLOCK;
        return lox_clock;
}

LoxFunction *lox_function_construct(const FunctionStmt *declaration) {
        LoxFunction *lox_function = xmalloc(sizeof(LoxFunction));
        lox_function->base.type = LOX_CALLABLE_FUNCTION;
        lox_function->declaration = declaration;
        return lox_function;
}

const char *lox_callable_to_string(const LoxCallable *callable) {
        static char str[256];
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return "<native fn>";
        case LOX_CALLABLE_FUNCTION:
                snprintf(str, sizeof(str), "<fn %s>", ((LoxFunction *)callable)->declaration->name->lexeme);
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
        Environment *environment = environment_construct(get_globals());

        size_t num_params = vector_size(lox_function->declaration->params);
        for (size_t i = 0; i < num_params; i++) {
                Token *param = vector_at(lox_function->declaration->params, i);
                environment_define(environment, param->lexeme, vector_at(arguments, i));
        }

        execute_block(lox_function->declaration->body, environment);
        return nil_object_construct();
}

Object *lox_callable_call(LoxCallable *callable, Vector *arguments) {
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return number_object_construct(time(NULL));
        case LOX_CALLABLE_FUNCTION:
                return lox_function_call((LoxFunction *)callable, arguments);
        }
}
