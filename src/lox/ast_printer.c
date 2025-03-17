#include "lox/ast_printer.h"
#include "lox/expr.h"
#include "lox/object.h"

#include <stdio.h>

static void print_expr(const Expr *expr);

static void print_grouping_expr(const GroupingExpr *grouping_expr) {
        printf("(group ");
        print_expr(grouping_expr->expression);
        printf(")");
}

static void print_literal_expr(const LiteralExpr *literal_expr) {
        printf("%s", object_to_string(literal_expr->value));
}

static void print_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_GROUPING:
                print_grouping_expr((const GroupingExpr *)expr);
                break;
        case EXPR_LITERAL:
                print_literal_expr((const LiteralExpr *)expr);
                break;
        }
}

void println_expr(const Expr *expr) {
        print_expr(expr);
        printf("\n");
}
