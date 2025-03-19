#include "lox/lox_instance.h"
#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/lox_callable.h"
#include "lox/lox_class.h"
#include "lox/lox_function.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/set.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <string.h>

typedef struct {
        const char *name;
        Object *value;
} Element;

static int element_compare(const void *element1, const void *element2) {
        const char *name1 = ((const Element *)element1)->name;
        const char *name2 = ((const Element *)element2)->name;
        return strcmp(name1, name2);
}

static Element *element_construct(const char *name, Object *value) {
        Element *element = xmalloc(sizeof(Element));
        element->name = name;
        element->value = value;
        return element;
}

struct LoxInstance {
        LoxClass *class;
        Set *fields;
};

LoxInstance *lox_instance_construct(LoxClass *class) {
        LoxInstance *instance = xmalloc(sizeof(LoxInstance));
        instance->class = class;
        instance->fields = set_construct(element_compare);
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
        Element element = {
                .name = name->lexeme,
        };
        Element *p = set_search(instance->fields, &element);
        if (p != NULL) {
                return p->value;
        }

        LoxFunction *method = lox_class_find_method(instance->class, name->lexeme);
        if (method != NULL) {
                return lox_callable_object_construct((LoxCallable *)lox_function_bind(method, instance));
        }

        interpret_error(name, "Undefined property '%s'.", name->lexeme);
}

void lox_instance_set(LoxInstance *instance, const Token *name, Object *value) {
        set_insert(instance->fields, element_construct(name->lexeme, value));
}
