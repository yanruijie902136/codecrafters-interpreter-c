#ifndef CODECRAFTERS_INTERPRETER_PARSER_H
#define CODECRAFTERS_INTERPRETER_PARSER_H

#include "lox/expr.h"
#include "lox/ptr_vector.h"

Expr *parseToExpr(const PtrVector *tokens);

#endif
