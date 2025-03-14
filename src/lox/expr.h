#ifndef CODECRAFTERS_INTERPRETER_EXPR_H
#define CODECRAFTERS_INTERPRETER_EXPR_H

#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"

typedef enum {
        EXPR_ASSIGN,
        EXPR_BINARY,
        EXPR_CALL,
        EXPR_GROUPING,
        EXPR_LITERAL,
        EXPR_LOGICAL,
        EXPR_UNARY,
        EXPR_VARIABLE,
} ExprType;

typedef struct {
        ExprType type;
        const void *data;
} Expr;

typedef struct {
        const Token *name;
        const Expr *value;
} AssignExpr;

typedef struct {
        const Expr *left;
        const Token *operator;
        const Expr *right;
} BinaryExpr;

typedef struct {
        const Expr *callee;
        const Token *paren;
        const PtrVector *arguments;
} CallExpr;

typedef struct {
        const Expr *expression;
} GroupingExpr;

typedef struct {
        const Object *value;
} LiteralExpr;

typedef struct {
        const Expr *left;
        const Token *operator;
        const Expr *right;
} LogicalExpr;

typedef struct {
        const Token *operator;
        const Expr *right;
} UnaryExpr;

typedef struct {
        const Token *name;
} VariableExpr;

Expr *assign_expr_create(const Token *name, const Expr *value);
Expr *binary_expr_create(const Expr *left, const Token *operator, const Expr *right);
Expr *call_expr_create(const Expr *callee, const Token *paren, const PtrVector *arguments);
Expr *grouping_expr_create(const Expr *expression);
Expr *literal_expr_create(const Object *value);
Expr *logical_expr_create(const Expr *left, const Token *operator, const Expr *right);
Expr *unary_expr_create(const Token *operator, const Expr *right);
Expr *variable_expr_create(const Token *name);

#endif
