#include "lox/lox_function.h"
#include "lox/environment.h"
#include "lox/interpreter.h"
#include "lox/lox_callable.h"
#include "lox/object.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <stdio.h>

LoxFunction *lox_function_construct(const FunctionStmt *declaration, Environment *closure, bool is_initializer) {
        LoxFunction *function = xmalloc(sizeof(LoxFunction));
        function->base.type = LOX_CALLABLE_FUNCTION;
        function->declaration = declaration;
        function->closure = closure;
        function->is_initializer = is_initializer;
        return function;
}

const char *lox_function_to_string(const LoxFunction *function) {
        static char str[256];
        snprintf(str, sizeof(str), "<fn %s>", function->declaration->name->lexeme);
        return str;
}

size_t lox_function_arity(const LoxFunction *function) {
        return vector_size(function->declaration->params);
}

Object *lox_function_call(LoxFunction *lox_function, Vector *arguments) {
        Environment *environment = environment_construct(lox_function->closure);

        size_t num_params = vector_size(lox_function->declaration->params);
        for (size_t i = 0; i < num_params; i++) {
                Token *param = vector_at(lox_function->declaration->params, i);
                environment_define(environment, param->lexeme, vector_at(arguments, i));
        }

        Object *result = execute_block(lox_function->declaration->body, environment);
        if (lox_function->is_initializer) {
                return environment_get_at(lox_function->closure, "this", 0);
        }
        return result == NULL ? nil_object_construct() : result;
}
