#include "lox/interpreter.h"
#include "lox/callable.h"
#include "lox/environment.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/stmt.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

#include <err.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>

typedef struct {
        Environment *globals;
        Environment *environment;
        jmp_buf env;
} Interpreter;

static Interpreter interpreter;

static void
init(void)
{
        interpreter.globals = environment_create(NULL);
        interpreter.environment = interpreter.globals;

        environment_define(interpreter.globals, "clock", object_create_callable(clock_create()));
}

static Object *evaluate_expr(const Expr *expr);

static Object *
evaluate_assign_expr(const AssignExpr *assign_expr)
{
        Object *object = evaluate_expr(assign_expr->value);

        const char *name = assign_expr->name->lexeme;
        if (environment_assign(interpreter.environment, name, object) < 0)
        {
                fprintf(stderr, "Undefined variable '%s'.\n", name);
                longjmp(interpreter.env, 1);
        }

        return object_copy(object);
}

static void
check_number_operands(const Object *left, const Object *right, double *num1, double *num2)
{
        if (left->type != OBJECT_NUMBER || right->type != OBJECT_NUMBER)
        {
                fprintf(stderr, "Operand must be numbers.\n");
                longjmp(interpreter.env, 1);
        }
        *num1 = *(const double *)left->data;
        *num2 = *(const double *)right->data;
}

static void
check_number_operand(const Object *right, double *num)
{
        if (right->type != OBJECT_NUMBER)
        {
                fprintf(stderr, "Operand must be a number.\n");
                longjmp(interpreter.env, 1);
        }
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
evaluate_call_expr(const CallExpr *call_expr)
{
        Object *callee = evaluate_expr(call_expr->callee);

        PtrVector *arguments = ptr_vector_create();
        size_t num_args = ptr_vector_size(call_expr->arguments);
        for (size_t i = 0; i < num_args; i++)
        {
                Expr *argexpr = ptr_vector_at(call_expr->arguments, i);
                ptr_vector_append(arguments, evaluate_expr(argexpr));
        }

        if (callee->type != OBJECT_CALLABLE)
        {
                fprintf(stderr, "Can only call functions and classes.\n");
                longjmp(interpreter.env, 1);
        }
        Callable *callable = (Callable *)callee->data;

        size_t arity = callable_arity(callable);
        if (num_args != arity)
        {
                fprintf(stderr, "Expected %zu arguments but got %zu instead.\n", arity, num_args);
                longjmp(interpreter.env, 1);
        }

        Object *object = callable_call(callable, arguments);
        object_destroy(callee);
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
evaluate_logical_expr(const LogicalExpr *logical_expr)
{
        Object *left = evaluate_expr(logical_expr->left);

        switch (logical_expr->operator->type)
        {
        case TOKEN_AND:
                if (!is_truthy(left))
                        return left;
                break;
        case TOKEN_OR:
                if (is_truthy(left))
                        return left;
                break;
        default:
                errx(EX_SOFTWARE, "unexpected logical operator");
        }

        object_destroy(left);
        return evaluate_expr(logical_expr->right);
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
evaluate_variable_expr(const VariableExpr *variable_expr)
{
        const char *name = variable_expr->name->lexeme;

        Object *object = environment_get(interpreter.environment, name);
        if (object == NULL)
        {
                fprintf(stderr, "Undefined variable '%s'.\n", name);
                longjmp(interpreter.env, 1);
        }

        return object;
}

static Object *
evaluate_expr(const Expr *expr)
{
        switch (expr->type)
        {
        case EXPR_ASSIGN:
                return evaluate_assign_expr(expr->data);
        case EXPR_BINARY:
                return evaluate_binary_expr(expr->data);
        case EXPR_CALL:
                return evaluate_call_expr(expr->data);
        case EXPR_GROUPING:
                return evaluate_grouping_expr(expr->data);
        case EXPR_LITERAL:
                return evaluate_literal_expr(expr->data);
        case EXPR_LOGICAL:
                return evaluate_logical_expr(expr->data);
        case EXPR_UNARY:
                return evaluate_unary_expr(expr->data);
        case EXPR_VARIABLE:
                return evaluate_variable_expr(expr->data);
        }
}

Object *
interpret_expr(const Expr *expr)
{
        init();

        if (setjmp(interpreter.env) != 0)
                return NULL;
        return evaluate_expr(expr);
}

static Object * execute_stmt(const Stmt *stmt);

Object *
execute_block(const PtrVector *statements, Environment *environment)
{
        Environment *previous = interpreter.environment;
        interpreter.environment = environment;

        Object *ret = NULL;

        size_t num_statements = ptr_vector_size(statements);
        for (size_t i = 0; ret == NULL && i < num_statements; i++)
        {
                Stmt *statement = ptr_vector_at(statements, i);
                ret = execute_stmt(statement);
        }

        interpreter.environment = previous;

        return ret;
}

static Object *
execute_block_stmt(const BlockStmt *block_stmt)
{
        Environment *new_environment = environment_create(interpreter.environment);
        Object *ret = execute_block(block_stmt->statements, new_environment);
        environment_destroy(new_environment);
        return ret;
}

static Object *
execute_expression_stmt(const ExpressionStmt *expression_stmt)
{
        Object *object = evaluate_expr(expression_stmt->expression);
        object_destroy(object);

        return NULL;
}

static Object *
execute_function_stmt(const FunctionStmt *function_stmt)
{
        const char *name = function_stmt->name->lexeme;
        Object *object = object_create_callable(
                function_create(interpreter.environment, function_stmt)
        );
        environment_define(interpreter.environment, function_stmt->name->lexeme, object);

        return NULL;
}

static Object *
execute_if_stmt(const IfStmt *if_stmt)
{
        Object *object = evaluate_expr(if_stmt->condition);

        Object *ret = NULL;
        if (is_truthy(object))
                ret = execute_stmt(if_stmt->then_branch);
        else if (if_stmt->else_branch != NULL)
                ret = execute_stmt(if_stmt->else_branch);

        object_destroy(object);
        return ret;
}

static Object *
execute_print_stmt(const PrintStmt *print_stmt)
{
        Object *object = evaluate_expr(print_stmt->expression);
        printf("%s\n", object_stringify(object, true));
        object_destroy(object);

        return NULL;
}

static Object *
execute_return_stmt(const ReturnStmt *return_stmt)
{
        if (return_stmt->value == NULL)
                return object_create_nil();
        return evaluate_expr(return_stmt->value);
}

static Object *
execute_var_stmt(const VarStmt *var_stmt)
{
        Object *object;
        if (var_stmt->initializer == NULL)
                object = object_create_nil();
        else
                object = evaluate_expr(var_stmt->initializer);

        const char *name = var_stmt->name->lexeme;
        environment_define(interpreter.environment, name, object);

        return NULL;
}

static Object *
execute_while_stmt(const WhileStmt *while_stmt)
{
        for ( ; ; )
        {
                Object *object = evaluate_expr(while_stmt->condition);
                bool boolean = is_truthy(object);
                object_destroy(object);

                if (!boolean)
                        return NULL;

                object = execute_stmt(while_stmt->body);
                if (object != NULL)
                        return object;
        }
}

static Object *
execute_stmt(const Stmt *stmt)
{
        switch (stmt->type)
        {
        case STMT_BLOCK:
                return execute_block_stmt(stmt->data);
        case STMT_EXPRESSION:
                return execute_expression_stmt(stmt->data);
        case STMT_FUNCTION:
                return execute_function_stmt(stmt->data);
        case STMT_IF:
                return execute_if_stmt(stmt->data);
        case STMT_PRINT:
                return execute_print_stmt(stmt->data);
        case STMT_RETURN:
                return execute_return_stmt(stmt->data);
        case STMT_VAR:
                return execute_var_stmt(stmt->data);
        case STMT_WHILE:
                return execute_while_stmt(stmt->data);
        }
}

int
interpret_stmts(const PtrVector *stmts)
{
        init();

        if (setjmp(interpreter.env) != 0)
                return -1;

        size_t num_stmts = ptr_vector_size(stmts);
        for (size_t i = 0; i < num_stmts; i++)
        {
                Stmt *stmt = ptr_vector_at(stmts, i);
                execute_stmt(stmt);
        }

        return 0;
}

Environment *
get_globals(void)
{
        return interpreter.globals;
}
