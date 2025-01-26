#include "lox/interpreter.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>

static Object *evaluate_expr(const Expr *expr);

static void
check_number_operands(const Object *left, const Object *right, double *num1, double *num2)
{
        *num1 = *(const double *)left->data;
        *num2 = *(const double *)right->data;
}

static void
check_number_operand(const Object *right, double *num)
{
        *num = *(const double *)right->data;
}

static char *
concatenate(const char *s1, const char *s2)
{
        size_t n = strlen(s1) + strlen(s2) + 1;
        char *str = xmalloc(n + 1);
        snprintf(str, n + 1, "%s%s", s1, s2);
        return str;
}

static bool
is_equal(const Object *left, const Object *right)
{
        if (left->type != right->type)
                return false;

        switch (left->type)
        {
        case OBJECT_BOOL:
                return *(const bool *)left->data == *(const bool *)right->data;
        case OBJECT_NIL:
                return true;
        case OBJECT_NUMBER:
                return *(const double *)left->data == *(const double *)right->data;
        case OBJECT_STRING:
                return strcmp(left->data, right->data) == 0;
        }
}

static Object *
evaluate_binary_expr(const BinaryExpr *binary_expr)
{
        Object *left = evaluate_expr(binary_expr->left);
        Object *right = evaluate_expr(binary_expr->right);

        Object *object;
        double num1, num2;
        switch (binary_expr->operator->type)
        {
        case TOKEN_BANG_EQUAL:
                object = object_create_bool(!is_equal(left, right));
                break;
        case TOKEN_EQUAL_EQUAL:
                object = object_create_bool(is_equal(left, right));
                break;
        case TOKEN_GREATER:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_bool(num1 > num2);
                break;
        case TOKEN_GREATER_EQUAL:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_bool(num1 >= num2);
                break;
        case TOKEN_LESS:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_bool(num1 < num2);
                break;
        case TOKEN_LESS_EQUAL:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_bool(num1 <= num2);
                break;
        case TOKEN_MINUS:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_number(num1 - num2);
                break;
        case TOKEN_PLUS:
                if (left->type == OBJECT_STRING && right->type == OBJECT_STRING)
                {
                        object = object_create_string(concatenate(left->data, right->data));
                        break;
                }
                check_number_operands(left, right, &num1, &num2);
                object = object_create_number(num1 + num2);
                break;
        case TOKEN_SLASH:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_number(num1 / num2);
                break;
        case TOKEN_STAR:
                check_number_operands(left, right, &num1, &num2);
                object = object_create_number(num1 * num2);
                break;
        default:
                errx(EX_SOFTWARE, "unexpected binary operator");
        }

        object_destroy(left);
        object_destroy(right);
        return object;
}

static Object *
evaluate_grouping_expr(const GroupingExpr *grouping_expr)
{
        return evaluate_expr(grouping_expr->expression);
}

static Object *
evaluate_literal_expr(const LiteralExpr *literal_expr)
{
        return object_copy(literal_expr->value);
}

static bool
is_truthy(const Object *object)
{
        if (object->type == OBJECT_NIL)
                return false;
        else if (object->type == OBJECT_BOOL)
                return *(const bool *)object->data;
        return true;
}

static Object *
evaluate_unary_expr(const UnaryExpr *unary_expr)
{
        Object *right = evaluate_expr(unary_expr->right);

        Object *object;
        double num;
        switch (unary_expr->operator->type)
        {
        case TOKEN_BANG:
                object = object_create_bool(!is_truthy(right));
                break;
        case TOKEN_MINUS:
                check_number_operand(right, &num);
                object = object_create_number(-num);
                break;
        default:
                errx(EX_SOFTWARE, "unexpected unary operator");
        }

        object_destroy(right);
        return object;
}

static Object *
evaluate_expr(const Expr *expr)
{
        switch (expr->type)
        {
        case EXPR_BINARY:
                return evaluate_binary_expr(expr->data);
        case EXPR_GROUPING:
                return evaluate_grouping_expr(expr->data);
        case EXPR_LITERAL:
                return evaluate_literal_expr(expr->data);
        case EXPR_UNARY:
                return evaluate_unary_expr(expr->data);
        }
}

Object *
interpret_expr(const Expr *expr)
{
        return evaluate_expr(expr);
}
