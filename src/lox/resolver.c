#include "lox/resolver.h"
#include "lox/errors.h"
#include "lox/expr.h"
#include "lox/interpreter.h"
#include "lox/stmt.h"
#include "lox/token.h"
#include "util/map.h"
#include "util/vector.h"

#include <stdbool.h>
#include <string.h>

typedef enum {
        CLASS_NONE,
        CLASS_CLASS,
        CLASS_SUBCLASS,
} ClassType;

typedef enum {
        FUNCTION_NONE,
        FUNCTION_FUNCTION,
        FUNCTION_INITIALIZER,
        FUNCTION_METHOD,
} FunctionType;

static struct {
        Vector *scopes;
        ClassType current_class;
        FunctionType current_function;
} resolver;

static void init(void) {
        static bool initialized = false;
        if (initialized) {
                return;
        }
        resolver.scopes = vector_construct();
        resolver.current_class = CLASS_NONE;
        resolver.current_function = FUNCTION_NONE;
        initialized = true;
}

static void begin_scope(void) {
        vector_push_back(resolver.scopes, map_construct(str_compare));
}

static void end_scope(void) {
        vector_pop_back(resolver.scopes);
}

static void declare(const Token *name) {
        if (vector_is_empty(resolver.scopes)) {
                return;
        }
        Map *scope = vector_at_back(resolver.scopes);

        if (map_contains(scope, name->lexeme)) {
                resolve_error(name, "Already a variable with this name in this scope.");
        }
        map_put(scope, name->lexeme, (void *)false);
}

static void define(const Token *name) {
        if (vector_is_empty(resolver.scopes)) {
                return;
        }
        Map *scope = vector_at_back(resolver.scopes);
        map_put(scope, name->lexeme, (void *)true);
}

static void resolve_local(const Expr *expr, const Token *name) {
        size_t num_scopes = vector_size(resolver.scopes);
        for (size_t i = 0; i < num_scopes; i++) {
                Map *scope = vector_at(resolver.scopes, num_scopes - i - 1);
                if (map_contains(scope, name->lexeme)) {
                        interpreter_resolve(expr, i);
                        return;
                }
        }
}

static void resolve_function(const FunctionStmt *function, FunctionType type) {
        FunctionType enclosing_function = resolver.current_function;
        resolver.current_function = type;

        begin_scope();
        size_t num_params = vector_size(function->params);
        for (size_t i = 0; i < num_params; i++) {
                Token *param = vector_at(function->params, i);
                declare(param);
                define(param);
        }
        resolve_stmts(function->body);
        end_scope();

        resolver.current_function = enclosing_function;
}

static void resolve_expr(const Expr *expr);

static void resolve_assign_expr(const AssignExpr *assign_expr) {
        resolve_expr(assign_expr->value);
        resolve_local((const Expr *)assign_expr, assign_expr->name);
}

static void resolve_binary_expr(const BinaryExpr *binary_expr) {
        resolve_expr(binary_expr->left);
        resolve_expr(binary_expr->right);
}

static void resolve_call_expr(const CallExpr *call_expr) {
        resolve_expr(call_expr->callee);
        size_t num_arguments = vector_size(call_expr->arguments);
        for (size_t i = 0; i < num_arguments; i++) {
                resolve_expr(vector_at(call_expr->arguments, i));
        }
}

static void resolve_get_expr(const GetExpr *get_expr) {
        resolve_expr(get_expr->object);
}

static void resolve_grouping_expr(const GroupingExpr *grouping_expr) {
        resolve_expr(grouping_expr->expression);
}

static void resolve_literal_expr(const LiteralExpr *literal_expr) {
        return;
}

static void resolve_logical_expr(const LogicalExpr *logical_expr) {
        resolve_expr(logical_expr->left);
        resolve_expr(logical_expr->right);
}

static void resolve_set_expr(const SetExpr *set_expr) {
        resolve_expr(set_expr->value);
        resolve_expr(set_expr->object);
}

static void resolve_super_expr(const SuperExpr *super_expr) {
        if (resolver.current_class == CLASS_NONE) {
                resolve_error(super_expr->keyword, "Can't use 'super' outside of a class.");
        } else if (resolver.current_class != CLASS_SUBCLASS) {
                resolve_error(super_expr->keyword, "Can't use 'super' in a class with no superclass.");
        }
        resolve_local((const Expr *)super_expr, super_expr->keyword);
}

static void resolve_this_expr(const ThisExpr *this_expr) {
        if (resolver.current_class == CLASS_NONE) {
                resolve_error(this_expr->keyword, "Can't use 'this' outside of a class.");
        }
        resolve_local((const Expr *)this_expr, this_expr->keyword);
}

static void resolve_unary_expr(const UnaryExpr *unary_expr) {
        resolve_expr(unary_expr->right);
}

static void resolve_variable_expr(const VariableExpr *variable_expr) {
        if (!vector_is_empty(resolver.scopes)) {
                Map *scope = vector_at_back(resolver.scopes);
                const Token *name = variable_expr->name;
                if (map_contains(scope, name->lexeme) && !(bool)map_get(scope, name->lexeme)) {
                        resolve_error(name, "Can't read local variable in its own initializer.");
                }
        }
        resolve_local((const Expr *)variable_expr, variable_expr->name);
}

static void resolve_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_ASSIGN:
                resolve_assign_expr((const AssignExpr *)expr);
                break;
        case EXPR_BINARY:
                resolve_binary_expr((const BinaryExpr *)expr);
                break;
        case EXPR_CALL:
                resolve_call_expr((const CallExpr *)expr);
                break;
        case EXPR_GET:
                resolve_get_expr((const GetExpr *)expr);
                break;
        case EXPR_GROUPING:
                resolve_grouping_expr((const GroupingExpr *)expr);
                break;
        case EXPR_LITERAL:
                resolve_literal_expr((const LiteralExpr *)expr);
                break;
        case EXPR_LOGICAL:
                resolve_logical_expr((const LogicalExpr *)expr);
                break;
        case EXPR_SET:
                resolve_set_expr((const SetExpr *)expr);
                break;
        case EXPR_SUPER:
                resolve_super_expr((const SuperExpr *)expr);
                break;
        case EXPR_THIS:
                resolve_this_expr((const ThisExpr *)expr);
                break;
        case EXPR_UNARY:
                resolve_unary_expr((const UnaryExpr *)expr);
                break;
        case EXPR_VARIABLE:
                resolve_variable_expr((const VariableExpr *)expr);
                break;
        }
}

static void resolve_stmt(const Stmt *stmt);

static void resolve_block_stmt(const BlockStmt *block_stmt) {
        begin_scope();
        resolve_stmts(block_stmt->statements);
        end_scope();
}

static void resolve_class_stmt(const ClassStmt *class_stmt) {
        ClassType enclosing_class = resolver.current_class;
        resolver.current_class = CLASS_CLASS;

        declare(class_stmt->name);
        define(class_stmt->name);

        if (class_stmt->superclass != NULL) {
                if (strcmp(class_stmt->superclass->name->lexeme, class_stmt->name->lexeme) == 0) {
                        resolve_error(class_stmt->superclass->name, "A class can't inherit from itself.");
                }
                resolver.current_class = CLASS_SUBCLASS;
                resolve_variable_expr(class_stmt->superclass);
                begin_scope();
                Map *scope = vector_at_back(resolver.scopes);
                map_put(scope, "super", (void *)true);
        }

        begin_scope();
        Map *scope = vector_at_back(resolver.scopes);
        map_put(scope, "this", (void *)true);

        size_t num_methods = vector_size(class_stmt->methods);
        for (size_t i = 0; i < num_methods; i++) {
                FunctionStmt *method = vector_at(class_stmt->methods, i);
                FunctionType type = strcmp(method->name->lexeme, "init") == 0 ? FUNCTION_INITIALIZER : FUNCTION_METHOD;
                resolve_function(method, type);
        }

        end_scope();

        if (class_stmt->superclass != NULL) {
                end_scope();
        }

        resolver.current_class = enclosing_class;
}

static void resolve_expression_stmt(const ExpressionStmt *expression_stmt) {
        resolve_expr(expression_stmt->expression);
}

static void resolve_function_stmt(const FunctionStmt *function_stmt) {
        declare(function_stmt->name);
        define(function_stmt->name);
        resolve_function(function_stmt, FUNCTION_FUNCTION);
}

static void resolve_if_stmt(const IfStmt *if_stmt) {
        resolve_expr(if_stmt->condition);
        resolve_stmt(if_stmt->then_branch);
        if (if_stmt->else_branch != NULL) {
                resolve_stmt(if_stmt->else_branch);
        }
}

static void resolve_print_stmt(const PrintStmt *print_stmt) {
        resolve_expr(print_stmt->expression);
}

static void resolve_return_stmt(const ReturnStmt *return_stmt) {
        if (resolver.current_function == FUNCTION_NONE) {
                resolve_error(return_stmt->keyword, "Can't return from top-level code.");
        }

        if (return_stmt->value != NULL) {
                if (resolver.current_function == FUNCTION_INITIALIZER) {
                        resolve_error(return_stmt->keyword, "Can't return a value from an initializer.");
                }
                resolve_expr(return_stmt->value);
        }
}

static void resolve_var_stmt(const VarStmt *var_stmt) {
        declare(var_stmt->name);
        if (var_stmt->initializer != NULL) {
                resolve_expr(var_stmt->initializer);
        }
        define(var_stmt->name);
}

static void resolve_while_stmt(const WhileStmt *while_stmt) {
        resolve_expr(while_stmt->condition);
        resolve_stmt(while_stmt->body);
}

static void resolve_stmt(const Stmt *stmt) {
        switch (stmt->type) {
        case STMT_BLOCK:
                resolve_block_stmt((const BlockStmt *)stmt);
                break;
        case STMT_CLASS:
                resolve_class_stmt((const ClassStmt *)stmt);
                break;
        case STMT_EXPRESSION:
                resolve_expression_stmt((const ExpressionStmt *)stmt);
                break;
        case STMT_FUNCTION:
                resolve_function_stmt((const FunctionStmt *)stmt);
                break;
        case STMT_IF:
                resolve_if_stmt((const IfStmt *)stmt);
                break;
        case STMT_PRINT:
                resolve_print_stmt((const PrintStmt *)stmt);
                break;
        case STMT_RETURN:
                resolve_return_stmt((const ReturnStmt *)stmt);
                break;
        case STMT_VAR:
                resolve_var_stmt((const VarStmt *)stmt);
                break;
        case STMT_WHILE:
                resolve_while_stmt((const WhileStmt *)stmt);
                break;
        }
}

void resolve_stmts(const Vector *statements) {
        init();
        size_t num_statements = vector_size(statements);
        for (size_t i = 0; i < num_statements; i++) {
                resolve_stmt(vector_at(statements, i));
        }
}
