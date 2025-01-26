#ifndef CODECRAFTERS_INTERPRETER_INTERPRETER_H
#define CODECRAFTERS_INTERPRETER_INTERPRETER_H

#include "lox/expr.h"
#include "lox/object.h"

Object *interpretExpr(const Expr *expr);

#endif
