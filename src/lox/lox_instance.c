#include "lox/lox_instance.h"
#include "util/xmalloc.h"

#include <stdio.h>

LoxInstance *lox_instance_construct(LoxClass *class) {
        LoxInstance *instance = xmalloc(sizeof(LoxInstance));
        instance->class = class;
        return instance;
}

const char *lox_instance_to_string(const LoxInstance *instance) {
        static char str[256];
        snprintf(str, sizeof(str), "%s instance", instance->class->name);
        return str;
}
