#ifndef CODECRAFTERS_INTERPRETER_INTERPRETER_H
#define CODECRAFTERS_INTERPRETER_INTERPRETER_H

#include "lox/environment.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"

Object *interpret_expr(const Expr *expr);
int interpret_stmts(const PtrVector *stmts);

Object *execute_block(const PtrVector *statements, Environment *environment);
Environment *get_globals(void);

#endif
