#include "lox/lox_class.h"
#include "lox/lox_callable.h"
#include "lox/lox_function.h"
#include "lox/lox_instance.h"
#include "util/map.h"
#include "util/xmalloc.h"

#include <string.h>

LoxClass *lox_class_construct(const char *name, LoxClass *superclass, Map *methods) {
        LoxClass *class = xmalloc(sizeof(LoxClass));
        class->base.type = LOX_CALLABLE_CLASS;
        class->name = name;
        class->superclass = superclass;
        class->methods = methods;
        return class;
}

const char *lox_class_to_string(const LoxClass *class) {
        return class->name;
}

size_t lox_class_arity(const LoxClass *class) {
        LoxFunction *initializer = lox_class_find_method(class, "init");
        return initializer == NULL ? 0 : lox_function_arity(initializer);
}

Object *lox_class_call(LoxClass *class, Vector *arguments) {
        LoxInstance *instance = lox_instance_construct(class);
        LoxFunction *initializer = lox_class_find_method(class, "init");
        if (initializer != NULL) {
                LoxFunction *function = lox_function_bind(initializer, instance);
                lox_function_call(function, arguments);
        }
        return lox_instance_object_construct(instance);
}

LoxFunction *lox_class_find_method(const LoxClass *class, const char *name) {
        if (map_contains(class->methods, name)) {
                return map_get(class->methods, name);
        } else if (class->superclass != NULL) {
                return lox_class_find_method(class->superclass, name);
        }
        return NULL;
}
