#ifndef CODECRAFTERS_INTERPRETER_LOX_EXPR_H
#define CODECRAFTERS_INTERPRETER_LOX_EXPR_H

#include "lox/token.h"
#include "lox/object.h"
#include "util/vector.h"

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
} Expr;

typedef struct {
        Expr base;
        Token *name;
        Expr *value;
} AssignExpr;

AssignExpr *assign_expr_construct(Token *name, Expr *value);

typedef struct {
        Expr base;
        Expr *left;
        Token *operator;
        Expr *right;
} BinaryExpr;

BinaryExpr *binary_expr_construct(Expr *left, Token *operator, Expr *right);

typedef struct {
        Expr base;
        Expr *callee;
        Token *paren;
        Vector *arguments;
} CallExpr;

CallExpr *call_expr_construct(Expr *callee, Token *paren, Vector *arguments);

typedef struct {
        Expr base;
        Expr *expression;
} GroupingExpr;

GroupingExpr *grouping_expr_construct(Expr *expression);

typedef struct {
        Expr base;
        Object *value;
} LiteralExpr;

LiteralExpr *literal_expr_construct(Object *value);

typedef struct {
        Expr base;
        Expr *left;
        Token *operator;
        Expr *right;
} LogicalExpr;

LogicalExpr *logical_expr_construct(Expr *left, Token *operator, Expr *right);

typedef struct {
        Expr base;
        Token *operator;
        Expr *right;
} UnaryExpr;

UnaryExpr *unary_expr_construct(Token *operator, Expr *right);

typedef struct {
        Expr base;
        Token *name;
} VariableExpr;

VariableExpr *variable_expr_construct(Token *name);

#endif
