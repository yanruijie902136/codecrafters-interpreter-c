#ifndef CODECRAFTERS_INTERPRETER_STMT_H
#define CODECRAFTERS_INTERPRETER_STMT_H

#include "lox/expr.h"
#include "lox/ptr_vector.h"

typedef enum {
        STMT_BLOCK,
        STMT_EXPRESSION,
        STMT_PRINT,
        STMT_VAR,
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
        const Expr *expression;
} PrintStmt;

typedef struct {
        const Token *name;
        const Expr *initializer;
} VarStmt;

Stmt *block_stmt_create(const PtrVector *statements);
Stmt *expression_stmt_create(const Expr *expression);
Stmt *print_stmt_create(const Expr *expression);
Stmt *var_stmt_create(const Token *name, const Expr *initializer);

#endif
