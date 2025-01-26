#ifndef CODECRAFTERS_INTERPRETER_OBJECT_H
#define CODECRAFTERS_INTERPRETER_OBJECT_H

typedef enum {
        OBJECT_STRING,
} ObjectType;

typedef struct {
        ObjectType type;
        const void *data;
} Object;

Object *createStringObject(const char *str);
const char *objectToString(const Object *object);

#endif
