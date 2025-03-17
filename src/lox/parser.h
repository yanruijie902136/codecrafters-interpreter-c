#ifndef CODECRAFTERS_INTERPRETER_LOX_PARSER_H
#define CODECRAFTERS_INTERPRETER_LOX_PARSER_H

#include "lox/expr.h"
#include "util/vector.h"

Expr *parse_expr(const Vector *tokens);

#endif
