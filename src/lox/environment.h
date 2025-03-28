#ifndef CODECRAFTERS_INTERPRETER_LOX_ENVIRONMENT_H
#define CODECRAFTERS_INTERPRETER_LOX_ENVIRONMENT_H

#include "lox/object.h"
#include "lox/token.h"
#include "util/map.h"

typedef struct Environment Environment;
struct Environment {
        Map *values;
        Environment *enclosing;
};

Environment *environment_construct(Environment *enclosing);
Object *environment_get(const Environment *environment, const Token *name);
Object *environment_get_at(const Environment *environment, const char *name, size_t depth);
void environment_define(Environment *environment, const char *name, Object *value);
void environment_assign(Environment *environment, const Token *name, Object *value);
void environment_assign_at(Environment *environment, const Token *name, Object *value, size_t depth);

#endif
