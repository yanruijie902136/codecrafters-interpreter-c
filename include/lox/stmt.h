#ifndef CODECRAFTERS_INTERPRETER_STMT_H
#define CODECRAFTERS_INTERPRETER_STMT_H

#include "lox/expr.h"

typedef enum {
        STMT_EXPRESSION,
        STMT_PRINT,
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

Stmt *expression_stmt_create(const Expr *expression);
Stmt *print_stmt_create(const Expr *expression);

#endif
