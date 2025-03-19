#ifndef CODECRAFTERS_INTERPRETER_LOX_INTERPRETER_H
#define CODECRAFTERS_INTERPRETER_LOX_INTERPRETER_H

#include "lox/environment.h"
#include "lox/expr.h"
#include "util/vector.h"

void interpret_expr(const Expr *expr);
void interpret_stmts(const Vector *statements);
Environment *get_globals(void);
Object *execute_block(Vector *statements, Environment *environment);
void interpreter_resolve(const Expr *expr, size_t depth);

#endif
