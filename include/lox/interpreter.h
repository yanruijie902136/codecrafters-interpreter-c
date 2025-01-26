#ifndef CODECRAFTERS_INTERPRETER_INTERPRETER_H
#define CODECRAFTERS_INTERPRETER_INTERPRETER_H

#include "lox/expr.h"
#include "lox/object.h"

Object *interpret_expr(const Expr *expr);

#endif
