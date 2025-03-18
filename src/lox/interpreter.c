#include "lox/interpreter.h"
#include "lox/errors.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/stmt.h"
#include "util/xmalloc.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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

static char *concat(const char *s1, const char *s2) {
        size_t size = strlen(s1) + strlen(s2) + 1;
        char *res = xmalloc(size);
        snprintf(res, size, "%s%s", s1, s2);
        return res;
}

static void check_number_operand(const Token *operator, const Object *operand) {
        if (object_is_number(operand)) {
                return;
        }
        interpret_error(operator, "Operand must be a number.");
}

static void check_number_operands(const Token *operator, const Object *left, const Object *right) {
        if (object_is_number(left) && object_is_number(right)) {
                return;
        }
        interpret_error(operator, "Operands must be numbers.");
}

static Object *evaluate_expr(const Expr *expr);

static Object *evaluate_binary_expr(const BinaryExpr *binary_expr) {
        Object *left = evaluate_expr(binary_expr->left);
        Object *right = evaluate_expr(binary_expr->right);
        Token *operator = binary_expr->operator;
        switch (operator->type) {
        case TOKEN_BANG_EQUAL:
                return boolean_object_construct(!object_equals(left, right));
        case TOKEN_EQUAL_EQUAL:
                return boolean_object_construct(object_equals(left, right));
        case TOKEN_GREATER:
                check_number_operands(operator, left, right);
                return boolean_object_construct(object_as_number(left) > object_as_number(right));
        case TOKEN_GREATER_EQUAL:
                check_number_operands(operator, left, right);
                return boolean_object_construct(object_as_number(left) >= object_as_number(right));
        case TOKEN_LESS:
                check_number_operands(operator, left, right);
                return boolean_object_construct(object_as_number(left) < object_as_number(right));
        case TOKEN_LESS_EQUAL:
                check_number_operands(operator, left, right);
                return boolean_object_construct(object_as_number(left) <= object_as_number(right));
        case TOKEN_MINUS:
                check_number_operands(operator, left, right);
                return number_object_construct(object_as_number(left) - object_as_number(right));
        case TOKEN_PLUS:
                if (object_is_string(left) && object_is_string(right)) {
                        char *s = concat(object_as_string(left), object_as_string(right));
                        return string_object_construct(s);
                } else if (object_is_number(left) && object_is_number(right)) {
                        return number_object_construct(object_as_number(left) + object_as_number(right));
                } else {
                        interpret_error(operator, "Operands must be two numbers or two strings.");
                }
        case TOKEN_SLASH:
                check_number_operands(operator, left, right);
                return number_object_construct(object_as_number(left) / object_as_number(right));
        case TOKEN_STAR:
                check_number_operands(operator, left, right);
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
        Token *operator = unary_expr->operator;
        switch (operator->type) {
        case TOKEN_BANG:
                return boolean_object_construct(!object_is_truthy(right));
        case TOKEN_MINUS:
                check_number_operand(operator, right);
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

static void execute_stmt(const Stmt *stmt);

static void execute_print_stmt(const PrintStmt *print_stmt) {
        printf("%s\n", stringify(evaluate_expr(print_stmt->expression)));
}

static void execute_stmt(const Stmt *stmt) {
        switch (stmt->type) {
        case STMT_PRINT:
                execute_print_stmt((const PrintStmt *)stmt);
                break;
        }
}

void interpret_expr(const Expr *expr) {
        printf("%s\n", stringify(evaluate_expr(expr)));
}

void interpret_stmts(const Vector *statements) {
        size_t num_statements = vector_size(statements);
        for (size_t i = 0; i < num_statements; i++) {
                execute_stmt(vector_at(statements, i));
        }
}
