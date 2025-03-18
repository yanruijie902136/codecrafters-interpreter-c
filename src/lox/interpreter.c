#include "lox/interpreter.h"
#include "lox/expr.h"
#include "lox/object.h"

#include <stdio.h>
#include <string.h>

static const char *stringify(const Object *object) {
        static char str[256];
        snprintf(str, sizeof(str), "%s", object_to_string(object));

        if (object_is_number(object)) {
                size_t n = strlen(str);
                char *p = str + n - 2;
                if (n >= 2 && strcmp(p, ".0") == 0) {
                        *p = '\0';
                }
        }

        return str;
}

static Object *evaluate_expr(const Expr *expr);

static Object *evaluate_binary_expr(const BinaryExpr *binary_expr) {
        Object *left = evaluate_expr(binary_expr->left);
        Object *right = evaluate_expr(binary_expr->right);
        switch (binary_expr->operator->type) {
        case TOKEN_MINUS:
                return number_object_construct(object_as_number(left) - object_as_number(right));
        case TOKEN_PLUS:
                return number_object_construct(object_as_number(left) + object_as_number(right));
        case TOKEN_SLASH:
                return number_object_construct(object_as_number(left) / object_as_number(right));
        case TOKEN_STAR:
                return number_object_construct(object_as_number(left) * object_as_number(right));
        }
}

static Object *evaluate_grouping_expr(const GroupingExpr *grouping_expr) {
        return evaluate_expr(grouping_expr->expression);
}

static Object *evaluate_literal_expr(const LiteralExpr *literal_expr) {
        return literal_expr->value;
}

static Object *evaluate_unary_expr(const UnaryExpr *unary_expr) {
        Object *right = evaluate_expr(unary_expr->right);
        switch (unary_expr->operator->type) {
        case TOKEN_BANG:
                return boolean_object_construct(!object_is_truthy(right));
        case TOKEN_MINUS:
                return number_object_construct(-object_as_number(right));
        }
}

static Object *evaluate_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_BINARY:
                return evaluate_binary_expr((const BinaryExpr *)expr);
        case EXPR_GROUPING:
                return evaluate_grouping_expr((const GroupingExpr *)expr);
        case EXPR_LITERAL:
                return evaluate_literal_expr((const LiteralExpr *)expr);
        case EXPR_UNARY:
                return evaluate_unary_expr((const UnaryExpr *)expr);
        }
}

void interpret_expr(const Expr *expr) {
        printf("%s\n", stringify(evaluate_expr(expr)));
}
