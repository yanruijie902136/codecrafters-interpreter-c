#ifndef CODECRAFTERS_INTERPRETER_LOX_STMT_H
#define CODECRAFTERS_INTERPRETER_LOX_STMT_H

#include "lox/expr.h"

typedef enum {
        STMT_EXPRESSION,
        STMT_PRINT,
        STMT_VAR,
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

typedef struct {
        Stmt base;
        Token *name;
        Expr *initializer;
} VarStmt;

VarStmt *var_stmt_construct(Token *name, Expr *initializer);

#endif
