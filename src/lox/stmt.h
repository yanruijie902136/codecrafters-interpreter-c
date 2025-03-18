#ifndef CODECRAFTERS_INTERPRETER_LOX_STMT_H
#define CODECRAFTERS_INTERPRETER_LOX_STMT_H

#include "lox/expr.h"

typedef enum {
        STMT_EXPRESSION,
        STMT_PRINT,
} StmtType;

typedef struct {
        StmtType type;
} Stmt;

typedef struct {
        Stmt base;
        Expr *expression;
} ExpressionStmt;

ExpressionStmt *expression_stmt_construct(Expr *expression);

typedef struct {
        Stmt base;
        Expr *expression;
} PrintStmt;

PrintStmt *print_stmt_construct(Expr *expression);

#endif
