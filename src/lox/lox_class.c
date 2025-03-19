#include "lox/lox_class.h"
#include "lox/lox_callable.h"
#include "lox/lox_instance.h"
#include "util/set.h"
#include "util/xmalloc.h"

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
        return 0;
}

Object *lox_class_call(LoxClass *class, Vector *arguments) {
        LoxInstance *instance = lox_instance_construct(class);
        return lox_instance_object_construct(instance);
}

Method *method_construct(const char *name, LoxFunction *function) {
        Method *method = xmalloc(sizeof(Method));
        method->name = name;
        method->function = function;
        return method;
}
