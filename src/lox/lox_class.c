#include "lox/lox_class.h"
#include "lox/lox_callable.h"
#include "lox/lox_function.h"
#include "lox/lox_instance.h"
#include "util/set.h"
#include "util/xmalloc.h"

#include <string.h>

LoxClass *lox_class_construct(const char *name, Set *methods) {
        LoxClass *class = xmalloc(sizeof(LoxClass));
        class->base.type = LOX_CALLABLE_CLASS;
        class->name = name;
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

int method_compare(const void *m1, const void *m2) {
        const char *name1 = ((const Method *)m1)->name;
        const char *name2 = ((const Method *)m2)->name;
        return strcmp(name1, name2);
}

Method *method_construct(const char *name, LoxFunction *function) {
        Method *method = xmalloc(sizeof(Method));
        method->name = name;
        method->function = function;
        return method;
}

LoxFunction *lox_class_find_method(const LoxClass *class, const char *name) {
        Method method = {
                .name = name,
        };
        Method *p = set_search(class->methods, &method);
        return p == NULL ? NULL : p->function;
}
