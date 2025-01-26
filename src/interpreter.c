#include "lox/interpreter.h"
#include "lox/expr.h"

static const Object *evaluateExpr(const Expr *expr);

static const Object *evaluateGroupingExpr(const GroupingExpr *groupingExpr) {
        return evaluateExpr(groupingExpr->expression);
}

static const Object *evaluateLiteralExpr(const LiteralExpr *literalExpr) {
        return literalExpr->value;
}

static const Object *evaluateExpr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_GROUPING:
                return evaluateGroupingExpr(expr->data);
        case EXPR_LITERAL:
                return evaluateLiteralExpr(expr->data);
        }
}

const Object *interpretExpr(const Expr *expr) {
        return evaluateExpr(expr);
}
