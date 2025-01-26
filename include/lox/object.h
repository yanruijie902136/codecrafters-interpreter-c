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

Object *createBoolObject(bool boolean);
Object *createNilObject(void);
Object *createNumberObject(double num);
Object *createStringObject(const char *str);

Object *copyObject(const Object *object);
void freeObject(Object *object);

const char *objectToString(const Object *object, bool minPrecision);

#endif
