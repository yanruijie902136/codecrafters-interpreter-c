#ifndef CODECRAFTERS_INTERPRETER_PARSER_H
#define CODECRAFTERS_INTERPRETER_PARSER_H

#include "lox/expr.h"
#include "lox/ptr_vector.h"

Expr *parse_to_expr(const PtrVector *tokens);

#endif
