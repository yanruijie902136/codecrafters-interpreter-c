#include "lox/expr.h"
#include "lox/object.h"
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
binary_expr_create(const Expr *left, const Token *operator, const Expr *right)
{
        BinaryExpr *binary_expr = xmalloc(sizeof(BinaryExpr));
        binary_expr->left = left;
        binary_expr->operator = operator;
        binary_expr->right = right;
        return expr_create(EXPR_BINARY, binary_expr);
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

static char str[1024];
static char *p = NULL;

static void
append_string(const char *format, ...)
{
        va_list ap;
        va_start(ap, format);
        vsnprintf(p, sizeof(str) - (p - str), format, ap);
        va_end(ap);
        p = str + strlen(str);
}

static void expr_stringify_static(const Expr *expr);

static void
binary_expr_stringify(const BinaryExpr *binaryExpr)
{
        append_string("(%s ", binaryExpr->operator->lexeme);
        expr_stringify_static(binaryExpr->left);
        append_string(" ");
        expr_stringify_static(binaryExpr->right);
        append_string(")");
}

static void
grouping_expr_stringify(const GroupingExpr *groupingExpr)
{
        append_string("(group ");
        expr_stringify_static(groupingExpr->expression);
        append_string(")");
}

static void
literal_expr_stringify(const LiteralExpr *literalExpr)
{
        append_string("%s", object_stringify(literalExpr->value, false));
}

static void
unary_expr_stringify(const UnaryExpr *unaryExpr)
{
        append_string("(%s ", unaryExpr->operator->lexeme);
        expr_stringify_static(unaryExpr->right);
        append_string(")");
}

static void
expr_stringify_static(const Expr *expr)
{
        switch (expr->type)
        {
        case EXPR_BINARY:
                binary_expr_stringify(expr->data);
                break;
        case EXPR_GROUPING:
                grouping_expr_stringify(expr->data);
                break;
        case EXPR_LITERAL:
                literal_expr_stringify(expr->data);
                break;
        case EXPR_UNARY:
                unary_expr_stringify(expr->data);
                break;
        }
}

const char *expr_stringify(const Expr *expr)
{
        p = str;
        expr_stringify_static(expr);
        return str;
}
