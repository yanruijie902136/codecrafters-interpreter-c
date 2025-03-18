#include "lox/lox_callable.h"
#include "lox/object.h"
#include "util/xmalloc.h"

#include <time.h>

LoxClock *lox_clock_construct(void) {
        LoxClock *lox_clock = xmalloc(sizeof(LoxClock));
        lox_clock->base.type = LOX_CALLABLE_CLOCK;
        return lox_clock;
}

const char *lox_callable_to_string(const LoxCallable *callable) {
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return "<native fn>";
        }
}

size_t lox_callable_arity(const LoxCallable *callable) {
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return 0;
        }
}

Object *lox_callable_call(LoxCallable *callable) {
        switch (callable->type) {
        case LOX_CALLABLE_CLOCK:
                return number_object_construct(time(NULL));
        }
}
