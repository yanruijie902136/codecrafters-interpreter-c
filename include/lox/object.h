#ifndef CODECRAFTERS_INTERPRETER_OBJECT_H
#define CODECRAFTERS_INTERPRETER_OBJECT_H

typedef enum {
        OBJECT_BOOL,
        OBJECT_NIL,
        OBJECT_NUMBER,
        OBJECT_STRING,
} ObjectType;

typedef struct {
        ObjectType type;
        const void *data;
} Object;

Object *object_create_bool(bool boolean);
Object *object_create_nil(void);
Object *object_create_number(double num);
Object *object_create_string(const char *str);

Object *object_copy(const Object *object);
void object_destroy(Object *object);

const char *object_stringify(const Object *object, bool min_precision);

#endif
