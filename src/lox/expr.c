#include "lox/expr.h"
#include "util/xmalloc.h"

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
