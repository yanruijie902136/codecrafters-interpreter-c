#include "lox/object.h"
#include "util/xmalloc.h"

typedef enum {
        OBJECT_STRING,
} ObjectType;

struct Object {
        ObjectType type;
        union {
                char *string;
        } data;
};

Object *string_object_construct(char *string) {
        Object *object = xmalloc(sizeof(Object));
        object->type = OBJECT_STRING;
        object->data.string = string;
        return object;
}

const char *object_to_string(const Object *object) {
        switch (object->type) {
        case OBJECT_STRING:
                return object->data.string;
        }
}
