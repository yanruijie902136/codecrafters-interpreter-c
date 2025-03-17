#ifndef CODECRAFTERS_INTERPRETER_LOX_OBJECT_H
#define CODECRAFTERS_INTERPRETER_LOX_OBJECT_H

#include <stdbool.h>

typedef struct Object Object;

Object *boolean_object_construct(bool boolean);
Object *nil_object_construct(void);
Object *number_object_construct(double number);
Object *string_object_construct(char *string);
const char *object_to_string(const Object *object);

#endif
