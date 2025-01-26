#include "lox/expr.h"
#include "lox/object.h"
#include "lox/xmalloc.h"

#include <stdio.h>
#include <string.h>

static Expr *createExpr(ExprType type, const void *data) {
        Expr *expr = xmalloc(sizeof(Expr));
        expr->type = type;
        expr->data = data;
        return expr;
}

Expr *createGroupingExpr(const Expr *expression) {
        GroupingExpr *groupingExpr = xmalloc(sizeof(GroupingExpr));
        groupingExpr->expression = expression;
        return createExpr(EXPR_GROUPING, groupingExpr);
}

Expr *createLiteralExpr(const Object *value) {
        LiteralExpr *literalExpr = xmalloc(sizeof(LiteralExpr));
        literalExpr->value = value;
        return createExpr(EXPR_LITERAL, literalExpr);
}

static char str[1024];
static char *p = NULL;

static void appendString(const char *s) {
        snprintf(p, sizeof(str) - (p - str), "%s", s);
        p += strlen(s);
}

static void stringifyExpr(const Expr *expr);

static void stringifyGroupingExpr(const GroupingExpr *groupingExpr) {
        appendString("(group ");
        stringifyExpr(groupingExpr->expression);
        appendString(")");
}

static void stringifyLiteralExpr(const LiteralExpr *literalExpr) {
        appendString(objectToString(literalExpr->value));
}

static void stringifyExpr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_GROUPING:
                stringifyGroupingExpr(expr->data);
                break;
        case EXPR_LITERAL:
                stringifyLiteralExpr(expr->data);
                break;
        }
}

const char *exprToString(const Expr *expr) {
        p = str;
        stringifyExpr(expr);
        return str;
}
