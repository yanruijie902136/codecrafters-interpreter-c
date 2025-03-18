#include "lox/expr.h"
#include "util/xmalloc.h"

AssignExpr *assign_expr_construct(Token *name, Expr *value) {
        AssignExpr *assign_expr = xmalloc(sizeof(AssignExpr));
        assign_expr->base.type = EXPR_ASSIGN;
        assign_expr->name = name;
        assign_expr->value = value;
        return assign_expr;
}

BinaryExpr *binary_expr_construct(Expr *left, Token *operator, Expr *right) {
        BinaryExpr *binary_expr = xmalloc(sizeof(BinaryExpr));
        binary_expr->base.type = EXPR_BINARY;
        binary_expr->left = left;
        binary_expr->operator = operator;
        binary_expr->right = right;
        return binary_expr;
}

CallExpr *call_expr_construct(Expr *callee, Token *paren, Vector *arguments) {
        CallExpr *call_expr = xmalloc(sizeof(CallExpr));
        call_expr->base.type = EXPR_CALL;
        call_expr->callee = callee;
        call_expr->paren = paren;
        call_expr->arguments = arguments;
        return call_expr;
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

LogicalExpr *logical_expr_construct(Expr *left, Token *operator, Expr *right) {
        LogicalExpr *logical_expr = xmalloc(sizeof(LogicalExpr));
        logical_expr->base.type = EXPR_LOGICAL;
        logical_expr->left = left;
        logical_expr->operator = operator;
        logical_expr->right = right;
        return logical_expr;
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
