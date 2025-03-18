#include "lox/interpreter.h"
#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/stmt.h"
#include "util/xmalloc.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
        Environment *globals;
        Environment *environment;
} interpreter;

static void init(void) {
        interpreter.globals = environment_construct(NULL);
        interpreter.environment = interpreter.globals;
}

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

static Object *evaluate_assign_expr(const AssignExpr *assign_expr) {
        Object *value = evaluate_expr(assign_expr->value);
        environment_assign(interpreter.environment, assign_expr->name, value);
        return value;
}

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
        default:
                errx(EXIT_FAILURE, "unexpected operator");
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
        default:
                errx(EXIT_FAILURE, "unexpected operator");
        }
}

static Object *evaluate_variable_expr(const VariableExpr *variable_expr) {
        return environment_get(interpreter.environment, variable_expr->name);
}

static Object *evaluate_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_ASSIGN:
                return evaluate_assign_expr((const AssignExpr *)expr);
        case EXPR_BINARY:
                return evaluate_binary_expr((const BinaryExpr *)expr);
        case EXPR_GROUPING:
                return evaluate_grouping_expr((const GroupingExpr *)expr);
        case EXPR_LITERAL:
                return evaluate_literal_expr((const LiteralExpr *)expr);
        case EXPR_UNARY:
                return evaluate_unary_expr((const UnaryExpr *)expr);
        case EXPR_VARIABLE:
                return evaluate_variable_expr((const VariableExpr *)expr);
        }
}

static void execute_stmt(const Stmt *stmt);

static void execute_block(Vector *statements, Environment *environment) {
        Environment *previous = interpreter.environment;
        interpreter.environment = environment;

        size_t num_statements = vector_size(statements);
        for (size_t i = 0; i < num_statements; i++) {
                execute_stmt(vector_at(statements, i));
        }

        interpreter.environment = previous;
}

static void execute_block_stmt(const BlockStmt *block_stmt) {
        execute_block(block_stmt->statements, environment_construct(interpreter.environment));
}

static void execute_expression_stmt(const ExpressionStmt *expression_stmt) {
        evaluate_expr(expression_stmt->expression);
}

static void execute_if_stmt(const IfStmt *if_stmt) {
        if (object_is_truthy(evaluate_expr(if_stmt->condition))) {
                execute_stmt(if_stmt->then_branch);
        } else if (if_stmt->else_branch != NULL) {
                execute_stmt(if_stmt->else_branch);
        }
}

static void execute_print_stmt(const PrintStmt *print_stmt) {
        printf("%s\n", stringify(evaluate_expr(print_stmt->expression)));
}

static void execute_var_stmt(const VarStmt *var_stmt) {
        Object *value = var_stmt->initializer == NULL ? nil_object_construct() : evaluate_expr(var_stmt->initializer);
        environment_define(interpreter.environment, var_stmt->name->lexeme, value);
}

static void execute_stmt(const Stmt *stmt) {
        switch (stmt->type) {
        case STMT_BLOCK:
                execute_block_stmt((const BlockStmt *)stmt);
                break;
        case STMT_EXPRESSION:
                execute_expression_stmt((const ExpressionStmt *)stmt);
                break;
        case STMT_IF:
                execute_if_stmt((const IfStmt *)stmt);
                break;
        case STMT_PRINT:
                execute_print_stmt((const PrintStmt *)stmt);
                break;
        case STMT_VAR:
                execute_var_stmt((const VarStmt *)stmt);
                break;
        }
}

void interpret_expr(const Expr *expr) {
        init();
        printf("%s\n", stringify(evaluate_expr(expr)));
}

void interpret_stmts(const Vector *statements) {
        init();
        size_t num_statements = vector_size(statements);
        for (size_t i = 0; i < num_statements; i++) {
                execute_stmt(vector_at(statements, i));
        }
}
