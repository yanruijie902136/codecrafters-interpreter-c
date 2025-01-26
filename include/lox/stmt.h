#ifndef CODECRAFTERS_INTERPRETER_STMT_H
#define CODECRAFTERS_INTERPRETER_STMT_H

#include "lox/expr.h"

typedef enum {
        STMT_EXPRESSION,
        STMT_PRINT,
        STMT_VAR,
} StmtType;

typedef struct {
        StmtType type;
        const void *data;
} Stmt;

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

Stmt *expression_stmt_create(const Expr *expression);
Stmt *print_stmt_create(const Expr *expression);
Stmt *var_stmt_create(const Token *name, const Expr *initializer);

#endif
