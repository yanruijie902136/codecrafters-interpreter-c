#include "lox/ast_printer.h"
#include "lox/expr.h"
#include "lox/object.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

static void print_expr(const Expr *expr);

static void print_binary_expr(const BinaryExpr *binary_expr) {
        printf("(%s ", binary_expr->operator->lexeme);
        print_expr(binary_expr->left);
        printf(" ");
        print_expr(binary_expr->right);
        printf(")");
}

static void print_grouping_expr(const GroupingExpr *grouping_expr) {
        printf("(group ");
        print_expr(grouping_expr->expression);
        printf(")");
}

static void print_literal_expr(const LiteralExpr *literal_expr) {
        printf("%s", object_to_string(literal_expr->value));
}

static void print_unary_expr(const UnaryExpr *unary_expr) {
        printf("(%s ", unary_expr->operator->lexeme);
        print_expr(unary_expr->right);
        printf(")");
}

static void print_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_BINARY:
                print_binary_expr((const BinaryExpr *)expr);
                break;
        case EXPR_GROUPING:
                print_grouping_expr((const GroupingExpr *)expr);
                break;
        case EXPR_LITERAL:
                print_literal_expr((const LiteralExpr *)expr);
                break;
        case EXPR_UNARY:
                print_unary_expr((const UnaryExpr *)expr);
                break;
        default:
                errx(EXIT_FAILURE, "unexpected expr in ast_printer");
        }
}

void println_expr(const Expr *expr) {
        print_expr(expr);
        printf("\n");
}
