#include "lox/object.h"
#include "lox/xmalloc.h"

static Object *createObject(ObjectType type, const void *data) {
        Object *object = xmalloc(sizeof(Object));
        object->type = type;
        object->data = data;
        return object;
}

Object *createStringObject(const char *str) {
        return createObject(OBJECT_STRING, str);
}

const char *objectToString(const Object *object) {
        switch (object->type) {
        case OBJECT_STRING:
                return (const char *)object->data;
        }
}
