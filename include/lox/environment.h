#ifndef CODECRAFTERS_INTERPRETER_ENVIRONMENT_H
#define CODECRAFTERS_INTERPRETER_ENVIRONMENT_H

#include "lox/object.h"

typedef struct Environment Environment;

Environment *environment_create(Environment *enclosing);
void environment_destroy(Environment *environment);
void environment_define(Environment *environment, const char *name, const Object *object);
int environment_assign(Environment *environment, const char *name, const Object *object);
Object *environment_get(const Environment *environment, const char *name);

#endif
