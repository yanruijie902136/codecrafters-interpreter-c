#include "lox/lox_callable.h"
#include "lox/lox_class.h"
#include "lox/lox_function.h"
#include "lox/object.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <time.h>

LoxClock *lox_clock_construct(void) {
        LoxClock *lox_clock = xmalloc(sizeof(LoxClock));
        lox_clock->base.type = LOX_CALLABLE_CLOCK;
        return lox_clock;
}

const char *lox_callable_to_string(const LoxCallable *callable) {
        static char str[256];
        switch (callable->type) {
        case LOX_CALLABLE_CLASS:
                return lox_class_to_string((const LoxClass *)callable);
        case LOX_CALLABLE_CLOCK:
                return "<native fn>";
        case LOX_CALLABLE_FUNCTION:
                return lox_function_to_string((const LoxFunction *)callable);
        }
}

size_t lox_callable_arity(const LoxCallable *callable) {
        switch (callable->type) {
        case LOX_CALLABLE_CLASS:
                return lox_class_arity((const LoxClass *)callable);
        case LOX_CALLABLE_CLOCK:
                return 0;
        case LOX_CALLABLE_FUNCTION:
                return lox_function_arity((const LoxFunction *)callable);
        }
}

Object *lox_callable_call(LoxCallable *callable, Vector *arguments) {
        switch (callable->type) {
        case LOX_CALLABLE_CLASS:
                return lox_class_call((LoxClass *)callable, arguments);
        case LOX_CALLABLE_CLOCK:
                return number_object_construct(time(NULL));
        case LOX_CALLABLE_FUNCTION:
                return lox_function_call((LoxFunction *)callable, arguments);
        }
}
