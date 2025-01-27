#include "lox/callable.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/xmalloc.h"

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
callable_copy(const Callable *callable)
{
        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return callable_create(CALLABLE_CLOCK, NULL);
        }
}

size_t
callable_arity(const Callable *callable)
{
        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return 0;
        }
}

static void *
clock_call(void)
{
        struct timeval timeval;
        gettimeofday(&timeval, NULL);
        return object_create_number(timeval.tv_sec);
}

void *
callable_call(Callable *callable, const PtrVector *arguments)
{
        switch (callable->type)
        {
        case CALLABLE_CLOCK:
                return clock_call();
        }
}
