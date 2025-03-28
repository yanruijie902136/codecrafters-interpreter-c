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

GetExpr *get_expr_construct(Expr *object, Token *name) {
        GetExpr *get_expr = xmalloc(sizeof(GetExpr));
        get_expr->base.type = EXPR_GET;
        get_expr->object = object;
        get_expr->name = name;
        return get_expr;
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

SetExpr *set_expr_construct(Expr *object, Token *name, Expr *value) {
        SetExpr *set_expr = xmalloc(sizeof(SetExpr));
        set_expr->base.type = EXPR_SET;
        set_expr->object = object;
        set_expr->name = name;
        set_expr->value = value;
        return set_expr;
}

SuperExpr *super_expr_construct(Token *keyword, Token *method) {
        SuperExpr *super_expr = xmalloc(sizeof(SuperExpr));
        super_expr->base.type = EXPR_SUPER;
        super_expr->keyword = keyword;
        super_expr->method = method;
        return super_expr;
}

ThisExpr *this_expr_construct(Token *keyword) {
        ThisExpr *this_expr = xmalloc(sizeof(ThisExpr));
        this_expr->base.type = EXPR_THIS;
        this_expr->keyword = keyword;
        return this_expr;
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
