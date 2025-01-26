#include "lox/object.h"
#include "lox/xmalloc.h"

#include <stdio.h>
#include <stdlib.h>
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

Object *copyObject(const Object *object) {
        switch (object->type) {
        case OBJECT_BOOL:
                return createObject(OBJECT_BOOL, xmemdup(object->data, sizeof(bool)));
        case OBJECT_NIL:
                return createObject(OBJECT_NIL, NULL);
        case OBJECT_NUMBER:
                return createObject(OBJECT_NUMBER, xmemdup(object->data, sizeof(double)));
        case OBJECT_STRING:
                return createObject(OBJECT_STRING, xstrdup(object->data));
        }
}

void freeObject(Object *object) {
        free((void *)object->data);
        free(object);
}

static const char *numberToString(double num, bool minPrecision) {
        static char str[1024];
        snprintf(str, sizeof(str), "%lf", num);

        char *dotptr = strchr(str, '.');
        if (dotptr == NULL) {
                if (!minPrecision) {
                        strncat(str, ".0", 3);
                }
                return str;
        }

        char *ptr = str + strlen(str) - 1;
        while (ptr > dotptr + 1 && *ptr == '0') {
                ptr--;
        }
        if (minPrecision && ptr == dotptr + 1 && *ptr == '0') {
                *dotptr = '\0';
                return str;
        }
        *(++ptr) = '\0';
        return str;
}

const char *objectToString(const Object *object, bool minPrecision) {
        switch (object->type) {
        case OBJECT_BOOL:
                return *(const bool *)object->data ? "true" : "false";
        case OBJECT_NIL:
                return "nil";
        case OBJECT_NUMBER:
                return numberToString(*(const double *)object->data, minPrecision);
        case OBJECT_STRING:
                return (const char *)object->data;
        }
}
