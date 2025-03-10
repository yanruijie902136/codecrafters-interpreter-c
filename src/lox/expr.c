#include "lox/expr.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static Expr *
expr_create(ExprType type, const void *data)
{
        Expr *expr = xmalloc(sizeof(Expr));
        expr->type = type;
        expr->data = data;
        return expr;
}

Expr *
assign_expr_create(const Token *name, const Expr *value)
{
        AssignExpr *assign_expr = xmalloc(sizeof(AssignExpr));
        assign_expr->name = name;
        assign_expr->value = value;
        return expr_create(EXPR_ASSIGN, assign_expr);
}

Expr *
binary_expr_create(const Expr *left, const Token *operator, const Expr *right)
{
        BinaryExpr *binary_expr = xmalloc(sizeof(BinaryExpr));
        binary_expr->left = left;
        binary_expr->operator = operator;
        binary_expr->right = right;
        return expr_create(EXPR_BINARY, binary_expr);
}

Expr *
call_expr_create(const Expr *callee, const Token *paren, const PtrVector *arguments)
{
        CallExpr *call_expr = xmalloc(sizeof(CallExpr));
        call_expr->callee = callee;
        call_expr->paren = paren;
        call_expr->arguments = arguments;
        return expr_create(EXPR_CALL, call_expr);
}

Expr *
grouping_expr_create(const Expr *expression)
{
        GroupingExpr *grouping_expr = xmalloc(sizeof(GroupingExpr));
        grouping_expr->expression = expression;
        return expr_create(EXPR_GROUPING, grouping_expr);
}

Expr *
literal_expr_create(const Object *value)
{
        LiteralExpr *literal_expr = xmalloc(sizeof(LiteralExpr));
        literal_expr->value = value;
        return expr_create(EXPR_LITERAL, literal_expr);
}

Expr *
logical_expr_create(const Expr *left, const Token *operator, const Expr *right)
{
        LogicalExpr *logical_expr = xmalloc(sizeof(LogicalExpr));
        logical_expr->left = left;
        logical_expr->operator = operator;
        logical_expr->right = right;
        return expr_create(EXPR_LOGICAL, logical_expr);
}

Expr *
unary_expr_create(const Token *operator, const Expr *right)
{
        UnaryExpr *unary_expr = xmalloc(sizeof(UnaryExpr));
        unary_expr->operator = operator;
        unary_expr->right = right;
        return expr_create(EXPR_UNARY, unary_expr);
}

Expr *
variable_expr_create(const Token *name)
{
        VariableExpr *variable_expr = xmalloc(sizeof(VariableExpr));
        variable_expr->name = name;
        return expr_create(EXPR_VARIABLE, variable_expr);
}
