#ifndef CODECRAFTERS_INTERPRETER_EXPR_H
#define CODECRAFTERS_INTERPRETER_EXPR_H

#include "lox/object.h"
#include "lox/token.h"

typedef enum {
        EXPR_BINARY,
        EXPR_GROUPING,
        EXPR_LITERAL,
        EXPR_UNARY,
} ExprType;

typedef struct {
        ExprType type;
        const void *data;
} Expr;

typedef struct {
        const Expr *left;
        const Token *operator;
        const Expr *right;
} BinaryExpr;

typedef struct {
        const Expr *expression;
} GroupingExpr;

typedef struct {
        const Object *value;
} LiteralExpr;

typedef struct {
        const Token *operator;
        const Expr *right;
} UnaryExpr;

Expr *binary_expr_create(const Expr *left, const Token *operator, const Expr *right);
Expr *grouping_expr_create(const Expr *expression);
Expr *literal_expr_create(const Object *value);
Expr *unary_expr_create(const Token *operator, const Expr *right);
const char *expr_stringify(const Expr *expr);

#endif
