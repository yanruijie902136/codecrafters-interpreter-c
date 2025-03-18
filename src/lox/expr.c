#include "lox/expr.h"
#include "util/xmalloc.h"

BinaryExpr *binary_expr_construct(Expr *left, Token *operator, Expr *right) {
        BinaryExpr *binary_expr = xmalloc(sizeof(BinaryExpr));
        binary_expr->base.type = EXPR_BINARY;
        binary_expr->left = left;
        binary_expr->operator = operator;
        binary_expr->right = right;
        return binary_expr;
}

GroupingExpr *grouping_expr_construct(Expr *expression) {
        GroupingExpr *grouping_expr = xmalloc(sizeof(GroupingExpr));
        grouping_expr->base.type = EXPR_GROUPING;
        grouping_expr->expression = expression;
        return grouping_expr;
}

LiteralExpr *literal_expr_construct(Object *value) {
        LiteralExpr *literal_expr = xmalloc(sizeof(LiteralExpr));
        literal_expr->base.type = EXPR_LITERAL;
        literal_expr->value = value;
        return literal_expr;
}

UnaryExpr *unary_expr_construct(Token *operator, Expr *right) {
        UnaryExpr *unary_expr = xmalloc(sizeof(UnaryExpr));
        unary_expr->base.type = EXPR_UNARY;
        unary_expr->operator = operator;
        unary_expr->right = right;
        return unary_expr;
}

VariableExpr *variable_expr_construct(Token *name) {
        VariableExpr *variable_expr = xmalloc(sizeof(VariableExpr));
        variable_expr->base.type = EXPR_VARIABLE;
        variable_expr->name = name;
        return variable_expr;
}
