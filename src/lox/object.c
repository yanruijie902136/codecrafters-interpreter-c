#include "lox/object.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <string.h>

typedef enum {
        OBJECT_NUMBER,
        OBJECT_STRING,
} ObjectType;

struct Object {
        ObjectType type;
        union {
                double number;
                char *string;
        } data;
};

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
        case OBJECT_NUMBER:
                return number_to_string(object->data.number);
        case OBJECT_STRING:
                return object->data.string;
        }
}
