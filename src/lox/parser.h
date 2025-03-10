#ifndef CODECRAFTERS_INTERPRETER_PARSER_H
#define CODECRAFTERS_INTERPRETER_PARSER_H

#include "lox/expr.h"
#include "lox/ptr_vector.h"

Expr *parse_to_expr(const PtrVector *tokens);
PtrVector *parse_to_stmts(const PtrVector *tokens);

#endif
