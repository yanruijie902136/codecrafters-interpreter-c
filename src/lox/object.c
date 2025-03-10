#include "lox/object.h"
#include "lox/callable.h"
#include "lox/xmalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Object *
object_create(ObjectType type, const void *data)
{
        Object *object = xmalloc(sizeof(Object));
        object->type = type;
        object->data = data;
        return object;
}

Object *
object_create_bool(bool boolean)
{
        return object_create(OBJECT_BOOL, xmemdup(&boolean, sizeof(boolean)));
}

Object *
object_create_callable(const void *callable)
{
        return object_create(OBJECT_CALLABLE, callable);
}

Object *
object_create_nil(void)
{
        return object_create(OBJECT_NIL, NULL);
}

Object *
object_create_number(double num)
{
        return object_create(OBJECT_NUMBER, xmemdup(&num, sizeof(num)));
}

Object *
object_create_string(const char *str)
{
        return object_create(OBJECT_STRING, str);
}

Object *
object_copy(const Object *object)
{
        switch (object->type)
        {
        case OBJECT_BOOL:
                return object_create(OBJECT_BOOL, xmemdup(object->data, sizeof(bool)));
        case OBJECT_CALLABLE:
                return object_create(OBJECT_CALLABLE, callable_copy(object->data));
        case OBJECT_NIL:
                return object_create(OBJECT_NIL, NULL);
        case OBJECT_NUMBER:
                return object_create(OBJECT_NUMBER, xmemdup(object->data, sizeof(double)));
        case OBJECT_STRING:
                return object_create(OBJECT_STRING, xstrdup(object->data));
        }
}

void
object_destroy(Object *object)
{
        free((void *)object->data);
        free(object);
}

static const char *
number_to_string(double num, bool min_precision)
{
        static char str[1024];
        snprintf(str, sizeof(str), "%lf", num);

        char *dotptr = strchr(str, '.');
        if (dotptr == NULL)
        {
                if (!min_precision)
                        strncat(str, ".0", 3);
                return str;
        }

        char *ptr = str + strlen(str) - 1;
        while (ptr > dotptr + 1 && *ptr == '0')
                ptr--;
        if (min_precision && ptr == dotptr + 1 && *ptr == '0')
        {
                *dotptr = '\0';
                return str;
        }
        *(++ptr) = '\0';
        return str;
}

const char *
object_stringify(const Object *object, bool min_precision)
{
        switch (object->type)
        {
        case OBJECT_BOOL:
                return *(const bool *)object->data ? "true" : "false";
        case OBJECT_CALLABLE:
                return callable_stringify(object->data);
        case OBJECT_NIL:
                return "nil";
        case OBJECT_NUMBER:
                return number_to_string(*(const double *)object->data, min_precision);
        case OBJECT_STRING:
                return (const char *)object->data;
        }
}
