#include "lox/callable.h"
#include "lox/environment.h"
#include "lox/interpreter.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/stmt.h"
#include "lox/xmalloc.h"

#include <stdio.h>
#include <sys/time.h>

static Callable *
callable_create(CallableType type, const void *data)
{
        Callable *callable = xmalloc(sizeof(Callable));
        callable->type = type;
        callable->data = data;
        return callable;
}

Callable *
clock_create(void)
{
        return callable_create(CALLABLE_CLOCK, NULL);
}

Callable *
function_create(const FunctionStmt *declaration)
{
        Function *function = xmalloc(sizeof(Function));
        function->declaration = declaration;
        return callable_create(CALLABLE_FUNCTION, function);
}

Callable *
callable_copy(const Callable *callable)
{
        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return callable_create(CALLABLE_CLOCK, NULL);
        case CALLABLE_FUNCTION:
                return callable_create(CALLABLE_FUNCTION, xmemdup(callable->data, sizeof(Function)));
        }
}

size_t
callable_arity(const Callable *callable)
{
        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return 0;
        case CALLABLE_FUNCTION:
                return ptr_vector_size(((const Function *)callable->data)->declaration->params);
        }
}

static void *
clock_call(void)
{
        struct timeval timeval;
        gettimeofday(&timeval, NULL);
        return object_create_number(timeval.tv_sec);
}

static void *
function_call(const Function *function, const PtrVector *arguments)
{
        Environment *environment = environment_create(get_globals());

        size_t num_params = ptr_vector_size(function->declaration->params);
        for (size_t i = 0; i < num_params; i++)
        {
                Token *token = ptr_vector_at(function->declaration->params, i);
                const char *name = token->lexeme;
                Object *object = ptr_vector_at(arguments, i);
                environment_define(environment, name, object);
        }

        Object *ret = execute_block(function->declaration->body, environment);
        if (ret == NULL)
                ret = object_create_nil();
        return ret;
}

void *
callable_call(Callable *callable, const PtrVector *arguments)
{
        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return clock_call();
        case CALLABLE_FUNCTION:
                return function_call(callable->data, arguments);
        }
}

const char *
callable_stringify(const Callable *callable)
{
        static char s[1024];

        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return "<native fn>";
        case CALLABLE_FUNCTION:
                snprintf(s, sizeof(s), "<fn %s>",
                        ((const Function *)callable->data)->declaration->name->lexeme);
                return s;
        }
}
