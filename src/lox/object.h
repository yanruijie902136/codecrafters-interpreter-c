#ifndef CODECRAFTERS_INTERPRETER_LOX_OBJECT_H
#define CODECRAFTERS_INTERPRETER_LOX_OBJECT_H

typedef struct Object Object;

Object *number_object_construct(double number);
Object *string_object_construct(char *string);
const char *object_to_string(const Object *object);

#endif
