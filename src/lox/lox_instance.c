#include "lox/lox_instance.h"
#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/lox_callable.h"
#include "lox/lox_class.h"
#include "lox/lox_function.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/map.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <string.h>

struct LoxInstance {
        LoxClass *class;
        Map *fields;
};

LoxInstance *lox_instance_construct(LoxClass *class) {
        LoxInstance *instance = xmalloc(sizeof(LoxInstance));
        instance->class = class;
        instance->fields = map_construct(str_compare);
        return instance;
}

const char *lox_instance_to_string(const LoxInstance *instance) {
        static char str[256];
        snprintf(str, sizeof(str), "%s instance", instance->class->name);
        return str;
}

LoxFunction *lox_function_bind(LoxFunction *function, LoxInstance *instance) {
        Environment *environment = environment_construct(function->closure);
        environment_define(environment, "this", lox_instance_object_construct(instance));
        return lox_function_construct(function->declaration, environment, function->is_initializer);
}

Object *lox_instance_get(LoxInstance *instance, const Token *name) {
        if (map_contains(instance->fields, name->lexeme)) {
                return map_get(instance->fields, name->lexeme);
        }

        LoxFunction *method = lox_class_find_method(instance->class, name->lexeme);
        if (method != NULL) {
                return lox_callable_object_construct((LoxCallable *)lox_function_bind(method, instance));
        }

        interpret_error(name, "Undefined property '%s'.", name->lexeme);
}

void lox_instance_set(LoxInstance *instance, const Token *name, Object *value) {
        map_put(instance->fields, name->lexeme, value);
}
