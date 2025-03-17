#ifndef CODECRAFTERS_INTERPRETER_LOX_EXPR_H
#define CODECRAFTERS_INTERPRETER_LOX_EXPR_H

#include "lox/object.h"

typedef enum {
        EXPR_LITERAL,
} ExprType;

typedef struct {
        ExprType type;
} Expr;

typedef struct {
        Expr base;
        Object *value;
} LiteralExpr;

LiteralExpr *literal_expr_construct(Object *value);

#endif
