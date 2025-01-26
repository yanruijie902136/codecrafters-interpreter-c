#include "lox/object.h"
#include "lox/xmalloc.h"

#include <stdio.h>
#include <string.h>

static Object *createObject(ObjectType type, const void *data) {
        Object *object = xmalloc(sizeof(Object));
        object->type = type;
        object->data = data;
        return object;
}

Object *createBoolObject(bool boolean) {
        return createObject(OBJECT_BOOL, xmemdup(&boolean, sizeof(boolean)));
}

Object *createNilObject(void) {
        return createObject(OBJECT_NIL, NULL);
}

Object *createNumberObject(double num) {
        return createObject(OBJECT_NUMBER, xmemdup(&num, sizeof(num)));
}

Object *createStringObject(const char *str) {
        return createObject(OBJECT_STRING, str);
}

static const char *numberToString(double num) {
        static char str[1024];
        snprintf(str, sizeof(str), "%lf", num);

        char *dotptr = strchr(str, '.');
        if (dotptr == NULL) {
                strncat(str, ".0", 3);
                return str;
        }

        char *ptr = str + strlen(str) - 1;
        while (ptr > dotptr + 1 && *ptr == '0') {
                ptr--;
        }
        *(++ptr) = '\0';
        return str;
}

const char *objectToString(const Object *object) {
        switch (object->type) {
        case OBJECT_BOOL:
                return *(const bool *)object->data ? "true" : "false";
        case OBJECT_NIL:
                return "nil";
        case OBJECT_NUMBER:
                return numberToString(*(const double *)object->data);
        case OBJECT_STRING:
                return (const char *)object->data;
        }
}
