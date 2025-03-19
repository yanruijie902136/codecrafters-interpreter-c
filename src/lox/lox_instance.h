#ifndef CODECRAFTERS_INTERPRETER_LOX_INSTANCE_H
#define CODECRAFTERS_INTERPRETER_LOX_INSTANCE_H

#include "lox/lox_class.h"
#include "lox/object.h"
#include "lox/token.h"

typedef struct LoxInstance LoxInstance;

LoxInstance *lox_instance_construct(LoxClass *class);
const char *lox_instance_to_string(const LoxInstance *instance);
Object *lox_instance_get(LoxInstance *instance, const Token *name);
void lox_instance_set(LoxInstance *instance, const Token *name, Object *value);

LoxFunction *lox_function_bind(LoxFunction *function, LoxInstance *instance);

Object *lox_instance_object_construct(LoxInstance *instance);
bool object_is_lox_instance(const Object *object);
LoxInstance *object_as_lox_instance(const Object *object);

#endif
