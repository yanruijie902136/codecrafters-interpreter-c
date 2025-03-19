#include "lox/object.h"
#include "lox/lox_callable.h"
#include "lox/lox_instance.h"
#include "util/xmalloc.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef enum {
        OBJECT_BOOLEAN,
        OBJECT_LOX_CALLABLE,
        OBJECT_LOX_INSTANCE,
        OBJECT_NIL,
        OBJECT_NUMBER,
        OBJECT_STRING,
} ObjectType;

struct Object {
        ObjectType type;
        union {
                bool boolean;
                double number;
                char *string;
                LoxCallable *callable;
                LoxInstance *instance;
        } data;
};

Object *boolean_object_construct(bool boolean) {
        if (boolean) {
                static Object *true_object = NULL;
                if (true_object == NULL) {
                        true_object = xmalloc(sizeof(Object));
                        true_object->type = OBJECT_BOOLEAN;
                        true_object->data.boolean = true;
                }
                return true_object;
        }

        static Object *false_object = NULL;
        if (false_object == NULL) {
                false_object = xmalloc(sizeof(Object));
                false_object->type = OBJECT_BOOLEAN;
                false_object->data.boolean = false;
        }
        return false_object;
}

Object *lox_callable_object_construct(LoxCallable *callable) {
        Object *object = xmalloc(sizeof(Object));
        object->type = OBJECT_LOX_CALLABLE;
        object->data.callable = callable;
        return object;
}

Object *lox_instance_object_construct(LoxInstance *instance) {
        Object *object = xmalloc(sizeof(Object));
        object->type = OBJECT_LOX_INSTANCE;
        object->data.instance = instance;
        return object;
}

Object *nil_object_construct(void) {
        static Object *nil_object = NULL;
        if (nil_object == NULL) {
                nil_object = xmalloc(sizeof(Object));
                nil_object->type = OBJECT_NIL;
        }
        return nil_object;
}

Object *number_object_construct(double number) {
        Object *object = xmalloc(sizeof(Object));
        object->type = OBJECT_NUMBER;
        object->data.number = number;
        return object;
}

Object *string_object_construct(char *string) {
        Object *object = xmalloc(sizeof(Object));
        object->type = OBJECT_STRING;
        object->data.string = string;
        return object;
}

static const char *number_to_string(double number) {
        static char str[256];
        snprintf(str, sizeof(str), "%lf", number);
        char *p = str + strlen(str) - 1;
        while (*p == '0' && *(p - 1) != '.') {
                *p-- = '\0';
        }
        return str;
}

const char *object_to_string(const Object *object) {
        switch (object->type) {
        case OBJECT_BOOLEAN:
                return object->data.boolean ? "true" : "false";
        case OBJECT_LOX_CALLABLE:
                return lox_callable_to_string(object->data.callable);
        case OBJECT_LOX_INSTANCE:
                return lox_instance_to_string(object->data.instance);
        case OBJECT_NIL:
                return "nil";
        case OBJECT_NUMBER:
                return number_to_string(object->data.number);
        case OBJECT_STRING:
                return object->data.string;
        }
}

bool object_is_truthy(const Object *object) {
        switch (object->type) {
        case OBJECT_BOOLEAN:
                return object->data.boolean;
        case OBJECT_NIL:
                return false;
        default:
                return true;
        }
}

bool object_equals(const Object *object, const Object *other) {
        if (object->type != other->type) {
                return false;
        }

        switch (object->type) {
        case OBJECT_BOOLEAN:
                return object->data.boolean == other->data.boolean;
        case OBJECT_NIL:
                return true;
        case OBJECT_NUMBER:
                return object->data.number == other->data.number;
        case OBJECT_STRING:
                return strcmp(object->data.string, other->data.string) == 0;
        default:
                return false;
        }
}

bool object_is_number(const Object *object) {
        return object->type == OBJECT_NUMBER;
}

double object_as_number(const Object *object) {
        assert(object_is_number(object));
        return object->data.number;
}

bool object_is_string(const Object *object) {
        return object->type == OBJECT_STRING;
}

const char *object_as_string(const Object *object) {
        assert(object_is_string(object));
        return object->data.string;
}

bool object_is_lox_callable(const Object *object) {
        return object->type == OBJECT_LOX_CALLABLE;
}

LoxCallable *object_as_lox_callable(const Object *object) {
        assert(object_is_lox_callable(object));
        return object->data.callable;
}

bool object_is_lox_instance(const Object *object) {
        return object->type == OBJECT_LOX_INSTANCE;
}

LoxInstance *object_as_lox_instance(const Object *object) {
        assert(object_is_lox_instance(object));
        return object->data.instance;
}
