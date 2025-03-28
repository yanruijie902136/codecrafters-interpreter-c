#include "lox/interpreter.h"
#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/expr.h"
#include "lox/lox_callable.h"
#include "lox/lox_class.h"
#include "lox/lox_function.h"
#include "lox/lox_instance.h"
#include "lox/object.h"
#include "lox/stmt.h"
#include "lox/token.h"
#include "util/map.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
        Environment *globals;
        Environment *environment;
        Map *locals;
} interpreter;

static void init(void) {
        static bool initialized = false;
        if (initialized) {
                return;
        }

        interpreter.globals = environment_construct(NULL);
        LoxClock *lox_clock = lox_clock_construct();
        environment_define(interpreter.globals, "clock", lox_callable_object_construct((LoxCallable *)lox_clock));
        interpreter.environment = interpreter.globals;
        interpreter.locals = map_construct(ptr_compare);

        initialized = true;
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

static Object *lookup_variable(const Token *name, const Expr *expr) {
        if (map_contains(interpreter.locals, expr)) {
                size_t depth = (size_t)map_get(interpreter.locals, expr);
                return environment_get_at(interpreter.environment, name->lexeme, depth);
        }
        return environment_get(interpreter.globals, name);
}

static Object *evaluate_expr(const Expr *expr);

static Object *evaluate_assign_expr(const AssignExpr *assign_expr) {
        Object *value = evaluate_expr(assign_expr->value);

        if (map_contains(interpreter.locals, assign_expr)) {
                size_t depth = (size_t)map_get(interpreter.locals, assign_expr);
                environment_assign_at(interpreter.environment, assign_expr->name, value, depth);
        } else {
                environment_assign(interpreter.globals, assign_expr->name, value);
        }

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

static Object *evaluate_call_expr(const CallExpr *call_expr) {
        Object *callee = evaluate_expr(call_expr->callee);

        Vector *arguments = vector_construct();
        size_t num_arguments = vector_size(call_expr->arguments);
        for (size_t i = 0; i < num_arguments; i++) {
                Expr *argument = vector_at(call_expr->arguments, i);
                vector_push_back(arguments, evaluate_expr(argument));
        }

        if (!object_is_lox_callable(callee)) {
                interpret_error(call_expr->paren, "Can only call functions and classes.");
        }
        LoxCallable *function = object_as_lox_callable(callee);

        size_t arity = lox_callable_arity(function);
        if (num_arguments != arity) {
                interpret_error(call_expr->paren, "Expected %zu arguments but got %zu.", arity, num_arguments);
        }

        return lox_callable_call(function, arguments);
}

static Object *evaluate_get_expr(const GetExpr *get_expr) {
        Object *object = evaluate_expr(get_expr->object);
        if (!object_is_lox_instance(object)) {
                interpret_error(get_expr->name, "Only instances have properties.");
        }
        LoxInstance *instance = object_as_lox_instance(object);
        return lox_instance_get(instance, get_expr->name);
}

static Object *evaluate_grouping_expr(const GroupingExpr *grouping_expr) {
        return evaluate_expr(grouping_expr->expression);
}

static Object *evaluate_literal_expr(const LiteralExpr *literal_expr) {
        return literal_expr->value;
}

static Object *evaluate_logical_expr(const LogicalExpr *logical_expr) {
        Object *left = evaluate_expr(logical_expr->left);
        Token *operator = logical_expr->operator;
        switch (operator->type) {
        case TOKEN_AND:
                if (!object_is_truthy(left)) {
                        return left;
                }
                break;
        case TOKEN_OR:
                if (object_is_truthy(left)) {
                        return left;
                }
                break;
        default:
                errx(EXIT_FAILURE, "unexpected operator");
        }
        return evaluate_expr(logical_expr->right);
}

static Object *evaluate_set_expr(const SetExpr *set_expr) {
        Object *object = evaluate_expr(set_expr->object);
        if (!object_is_lox_instance(object)) {
                interpret_error(set_expr->name, "Only instances have fields.");
        }
        Object *value = evaluate_expr(set_expr->value);
        LoxInstance *instance = object_as_lox_instance(object);
        lox_instance_set(instance, set_expr->name, value);
        return value;
}

static Object *evaluate_super_expr(const SuperExpr *super_expr) {
        size_t depth = (size_t)map_get(interpreter.locals, super_expr);

        Object *superclass_object = environment_get_at(interpreter.environment, "super", depth);
        LoxClass *superclass = (LoxClass *)object_as_lox_callable(superclass_object);

        Object *instance_object = environment_get_at(interpreter.environment, "this", depth - 1);
        LoxInstance *instance = object_as_lox_instance(instance_object);

        LoxFunction *method = lox_class_find_method(superclass, super_expr->method->lexeme);
        if (method == NULL) {
                interpret_error(super_expr->method, "Undefined property '%s'.", super_expr->method->lexeme);
        }
        return lox_callable_object_construct((LoxCallable *)lox_function_bind(method, instance));
}

static Object *evaluate_this_expr(const ThisExpr *this_expr) {
        return lookup_variable(this_expr->keyword, (const Expr *)this_expr);
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
        return lookup_variable(variable_expr->name, (const Expr *)variable_expr);
}

static Object *evaluate_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_ASSIGN:
                return evaluate_assign_expr((const AssignExpr *)expr);
        case EXPR_BINARY:
                return evaluate_binary_expr((const BinaryExpr *)expr);
        case EXPR_CALL:
                return evaluate_call_expr((const CallExpr *)expr);
        case EXPR_GET:
                return evaluate_get_expr((const GetExpr *)expr);
        case EXPR_GROUPING:
                return evaluate_grouping_expr((const GroupingExpr *)expr);
        case EXPR_LITERAL:
                return evaluate_literal_expr((const LiteralExpr *)expr);
        case EXPR_LOGICAL:
                return evaluate_logical_expr((const LogicalExpr *)expr);
        case EXPR_SET:
                return evaluate_set_expr((const SetExpr *)expr);
        case EXPR_SUPER:
                return evaluate_super_expr((const SuperExpr *)expr);
        case EXPR_THIS:
                return evaluate_this_expr((const ThisExpr *)expr);
        case EXPR_UNARY:
                return evaluate_unary_expr((const UnaryExpr *)expr);
        case EXPR_VARIABLE:
                return evaluate_variable_expr((const VariableExpr *)expr);
        }
}

static Object *execute_stmt(const Stmt *stmt);

static Object *execute_block_stmt(const BlockStmt *block_stmt) {
        return execute_block(block_stmt->statements, environment_construct(interpreter.environment));
}

static Object *execute_class_stmt(const ClassStmt *class_stmt) {
        Object *superclass_object = NULL;
        if (class_stmt->superclass != NULL) {
                superclass_object = evaluate_variable_expr(class_stmt->superclass);
                if (!object_is_lox_callable(superclass_object) || object_as_lox_callable(superclass_object)->type != LOX_CALLABLE_CLASS) {
                        interpret_error(class_stmt->superclass->name, "Superclass must be a class.");
                }
        }

        environment_define(interpreter.environment, class_stmt->name->lexeme, NULL);

        if (class_stmt->superclass != NULL) {
                interpreter.environment = environment_construct(interpreter.environment);
                environment_define(interpreter.environment, "super", superclass_object);
        }

        Map *methods = map_construct(str_compare);
        size_t num_methods = vector_size(class_stmt->methods);
        for (size_t i = 0; i < num_methods; i++) {
                FunctionStmt *method = vector_at(class_stmt->methods, i);
                bool is_initializer = strcmp(method->name->lexeme, "init") == 0;
                LoxFunction *function = lox_function_construct(method, interpreter.environment, is_initializer);
                map_put(methods, method->name->lexeme, function);
        }

        LoxClass *superclass = superclass_object == NULL ? NULL : (LoxClass *)object_as_lox_callable(superclass_object);
        LoxClass *class = lox_class_construct(class_stmt->name->lexeme, superclass, methods);

        if (superclass != NULL) {
                interpreter.environment = interpreter.environment->enclosing;
        }

        environment_assign(interpreter.environment, class_stmt->name, lox_callable_object_construct((LoxCallable *)class));

        return NULL;
}

static Object *execute_expression_stmt(const ExpressionStmt *expression_stmt) {
        evaluate_expr(expression_stmt->expression);
        return NULL;
}

static Object *execute_function_stmt(const FunctionStmt *function_stmt) {
        LoxFunction *function = lox_function_construct(function_stmt, interpreter.environment, false);
        Object *object = lox_callable_object_construct((LoxCallable *)function);
        environment_define(interpreter.environment, function_stmt->name->lexeme, object);
        return NULL;
}

static Object *execute_if_stmt(const IfStmt *if_stmt) {
        if (object_is_truthy(evaluate_expr(if_stmt->condition))) {
                return execute_stmt(if_stmt->then_branch);
        } else if (if_stmt->else_branch != NULL) {
                return execute_stmt(if_stmt->else_branch);
        }
        return NULL;
}

static Object *execute_print_stmt(const PrintStmt *print_stmt) {
        printf("%s\n", stringify(evaluate_expr(print_stmt->expression)));
        return NULL;
}

static Object *execute_return_stmt(const ReturnStmt *return_stmt) {
        if (return_stmt->value == NULL) {
                return nil_object_construct();
        }
        return evaluate_expr(return_stmt->value);
}

static Object *execute_var_stmt(const VarStmt *var_stmt) {
        Object *value = var_stmt->initializer == NULL ? nil_object_construct() : evaluate_expr(var_stmt->initializer);
        environment_define(interpreter.environment, var_stmt->name->lexeme, value);
        return NULL;
}

static Object *execute_while_stmt(const WhileStmt *while_stmt) {
        while (object_is_truthy(evaluate_expr(while_stmt->condition))) {
                Object *result = execute_stmt(while_stmt->body);
                if (result != NULL) {
                        return result;
                }
        }
        return NULL;
}

static Object *execute_stmt(const Stmt *stmt) {
        switch (stmt->type) {
        case STMT_BLOCK:
                return execute_block_stmt((const BlockStmt *)stmt);
        case STMT_CLASS:
                return execute_class_stmt((const ClassStmt *)stmt);
        case STMT_EXPRESSION:
                return execute_expression_stmt((const ExpressionStmt *)stmt);
        case STMT_FUNCTION:
                return execute_function_stmt((const FunctionStmt *)stmt);
        case STMT_IF:
                return execute_if_stmt((const IfStmt *)stmt);
        case STMT_PRINT:
                return execute_print_stmt((const PrintStmt *)stmt);
        case STMT_RETURN:
                return execute_return_stmt((const ReturnStmt *)stmt);
        case STMT_VAR:
                return execute_var_stmt((const VarStmt *)stmt);
        case STMT_WHILE:
                return execute_while_stmt((const WhileStmt *)stmt);
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

Environment *get_globals(void) {
        return interpreter.globals;
}

Object *execute_block(Vector *statements, Environment *environment) {
        Environment *previous = interpreter.environment;
        interpreter.environment = environment;

        Object *result = NULL;
        size_t num_statements = vector_size(statements);
        for (size_t i = 0; i < num_statements; i++) {
                result = execute_stmt(vector_at(statements, i));
                if (result != NULL) {
                        break;
                }
        }

        interpreter.environment = previous;
        return result;
}

void interpreter_resolve(const Expr *expr, size_t depth) {
        init();
        map_put(interpreter.locals, expr, (void *)depth);
}
