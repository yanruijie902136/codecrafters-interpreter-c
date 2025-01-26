#ifndef CODECRAFTERS_INTERPRETER_EXPR_H
#define CODECRAFTERS_INTERPRETER_EXPR_H

#include "lox/object.h"

typedef enum {
        EXPR_GROUPING,
        EXPR_LITERAL,
} ExprType;

typedef struct {
        ExprType type;
        const void *data;
} Expr;

typedef struct {
        const Expr *expression;
} GroupingExpr;

typedef struct {
        const Object *value;
} LiteralExpr;

Expr *createGroupingExpr(const Expr *expression);
Expr *createLiteralExpr(const Object *value);
const char *exprToString(const Expr *expr);

#endif
