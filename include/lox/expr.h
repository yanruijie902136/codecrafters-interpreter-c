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

Expr *createBinaryExpr(const Expr *left, const Token *operator, const Expr *right);
Expr *createGroupingExpr(const Expr *expression);
Expr *createLiteralExpr(const Object *value);
Expr *createUnaryExpr(const Token *operator, const Expr *right);
const char *exprToString(const Expr *expr);

#endif
