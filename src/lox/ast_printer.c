#include "lox/ast_printer.h"
#include "lox/expr.h"
#include "lox/object.h"

#include <stdio.h>

static void print_expr(const Expr *expr);

static void print_literal_expr(const LiteralExpr *literal_expr) {
        printf("%s", object_to_string(literal_expr->value));
}

static void print_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_LITERAL:
                print_literal_expr((const LiteralExpr *)expr);
                break;
        }
}

void println_expr(const Expr *expr) {
        print_expr(expr);
        printf("\n");
}
