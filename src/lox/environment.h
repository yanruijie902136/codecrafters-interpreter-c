#ifndef CODECRAFTERS_INTERPRETER_LOX_ENVIRONMENT_H
#define CODECRAFTERS_INTERPRETER_LOX_ENVIRONMENT_H

#include "lox/object.h"
#include "lox/token.h"

typedef struct Environment Environment;

Environment *environment_construct(void);
Object *environment_get(const Environment *environment, const Token *name);
void environment_define(Environment *environment, const char *name, Object *value);
void environment_assign(Environment *environment, const Token *name, Object *value);

#endif
