#ifndef CODECRAFTERS_INTERPRETER_STMT_H
#define CODECRAFTERS_INTERPRETER_STMT_H

#include "lox/expr.h"
#include "lox/ptr_vector.h"

typedef enum {
        STMT_BLOCK,
        STMT_EXPRESSION,
        STMT_FUNCTION,
        STMT_IF,
        STMT_PRINT,
        STMT_RETURN,
        STMT_VAR,
        STMT_WHILE,
} StmtType;

typedef struct {
        StmtType type;
        const void *data;
} Stmt;

typedef struct {
        const PtrVector *statements;
} BlockStmt;

typedef struct {
        const Expr *expression;
} ExpressionStmt;

typedef struct {
        const Token *name;
        const PtrVector *params;
        const PtrVector *body;
} FunctionStmt;

typedef struct {
        const Expr *condition;
        const Stmt *then_branch;
        const Stmt *else_branch;
} IfStmt;

typedef struct {
        const Expr *expression;
} PrintStmt;

typedef struct {
        const Token *keyword;
        const Expr *value;
} ReturnStmt;

typedef struct {
        const Token *name;
        const Expr *initializer;
} VarStmt;

typedef struct {
        const Expr *condition;
        const Stmt *body;
} WhileStmt;

Stmt *block_stmt_create(const PtrVector *statements);
Stmt *expression_stmt_create(const Expr *expression);
Stmt *function_stmt_create(const Token *name, const PtrVector *params, const PtrVector *body);
Stmt *if_stmt_create(const Expr *condition, const Stmt *then_branch, const Stmt *else_branch);
Stmt *print_stmt_create(const Expr *expression);
Stmt *return_stmt_create(const Token *keyword, const Expr *value);
Stmt *var_stmt_create(const Token *name, const Expr *initializer);
Stmt *while_stmt_create(const Expr *condition, const Stmt *body);

#endif
