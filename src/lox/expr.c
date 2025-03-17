#include "lox/expr.h"
#include "util/xmalloc.h"

LiteralExpr *literal_expr_construct(Object *value) {
        LiteralExpr *literal_expr = xmalloc(sizeof(LiteralExpr));
        literal_expr->base.type = EXPR_LITERAL;
        literal_expr->value = value;
        return literal_expr;
}
