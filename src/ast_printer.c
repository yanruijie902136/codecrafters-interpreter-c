#include "lox/ast_printer.h"
#include "lox/expr.h"
#include "lox/object.h"

#include <stdio.h>

static void print_expr_(const Expr *expr);

static void
print_binary_expr(const BinaryExpr *binary_expr)
{
        printf("(%s ", binary_expr->operator->lexeme);
        print_expr_(binary_expr->left);
        printf(" ");
        print_expr_(binary_expr->right);
        printf(")");
}

static void
print_grouping_expr(const GroupingExpr *grouping_expr)
{
        printf("(group ");
        print_expr_(grouping_expr->expression);
        printf(")");
}

static void
print_literal_expr(const LiteralExpr *literal_expr)
{
        printf("%s", object_stringify(literal_expr->value, false));
}

static void
print_unary_expr(const UnaryExpr *unary_expr)
{
        printf("(%s ", unary_expr->operator->lexeme);
        print_expr_(unary_expr->right);
        printf(")");
}

static void
print_expr_(const Expr *expr)
{
        switch (expr->type)
        {
        case EXPR_BINARY:
                print_binary_expr(expr->data);
                break;
        case EXPR_GROUPING:
                print_grouping_expr(expr->data);
                break;
        case EXPR_LITERAL:
                print_literal_expr(expr->data);
                break;
        case EXPR_UNARY:
                print_unary_expr(expr->data);
                break;
        }
}

void
print_expr(const Expr *expr)
{
        print_expr_(expr);
        printf("\n");
}
