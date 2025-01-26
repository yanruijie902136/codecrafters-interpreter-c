#include "lox/interpreter.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/token.h"

#include <err.h>
#include <sysexits.h>

static Object *evaluateExpr(const Expr *expr);

static Object *evaluateGroupingExpr(const GroupingExpr *groupingExpr) {
        return evaluateExpr(groupingExpr->expression);
}

static Object *evaluateLiteralExpr(const LiteralExpr *literalExpr) {
        return copyObject(literalExpr->value);
}

static bool isTruthy(const Object *object) {
        if (object->type == OBJECT_NIL) {
                return false;
        }
        else if (object->type == OBJECT_BOOL) {
                return *(const bool *)object->data;
        }
        return true;
}

static Object *evaluateUnaryExpr(const UnaryExpr *unaryExpr) {
        Object *right = evaluateExpr(unaryExpr->right);

        Object *object;
        switch (unaryExpr->operator->type) {
        case TOKEN_BANG:
                object = createBoolObject(!isTruthy(right));
                break;
        case TOKEN_MINUS:
                double num = *(const double *)right->data;
                object = createNumberObject(-num);
                break;
        default:
                errx(EX_SOFTWARE, "unexpected unary operator");
        }

        freeObject(right);
        return object;
}

static Object *evaluateExpr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_GROUPING:
                return evaluateGroupingExpr(expr->data);
        case EXPR_LITERAL:
                return evaluateLiteralExpr(expr->data);
        case EXPR_UNARY:
                return evaluateUnaryExpr(expr->data);
        }
}

Object *interpretExpr(const Expr *expr) {
        return evaluateExpr(expr);
}
