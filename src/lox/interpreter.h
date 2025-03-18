#ifndef CODECRAFTERS_INTERPRETER_LOX_INTERPRETER_H
#define CODECRAFTERS_INTERPRETER_LOX_INTERPRETER_H

#include "lox/expr.h"
#include "util/vector.h"

void interpret_expr(const Expr *expr);
void interpret_stmts(const Vector *statements);

#endif
