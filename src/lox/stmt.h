#ifndef CODECRAFTERS_INTERPRETER_LOX_STMT_H
#define CODECRAFTERS_INTERPRETER_LOX_STMT_H

#include "lox/expr.h"
#include "lox/token.h"
#include "util/vector.h"

typedef enum {
        STMT_BLOCK,
        STMT_CLASS,
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
} Stmt;

typedef struct {
        Stmt base;
        Vector *statements;
} BlockStmt;

BlockStmt *block_stmt_construct(Vector *statements);

typedef struct {
        Stmt base;
        Token *name;
        VariableExpr *superclass;
        Vector *methods;
} ClassStmt;

ClassStmt *class_stmt_construct(Token *name, VariableExpr *superclass, Vector *methods);

typedef struct {
        Stmt base;
        Expr *expression;
} ExpressionStmt;

ExpressionStmt *expression_stmt_construct(Expr *expression);

typedef struct {
        Stmt base;
        Token *name;
        Vector *params;
        Vector *body;
} FunctionStmt;

FunctionStmt *function_stmt_construct(Token *name, Vector *params, Vector *body);

typedef struct {
        Stmt base;
        Expr *condition;
        Stmt *then_branch;
        Stmt *else_branch;
} IfStmt;

IfStmt *if_stmt_construct(Expr *condition, Stmt *then_branch, Stmt *else_branch);

typedef struct {
        Stmt base;
        Expr *expression;
} PrintStmt;

PrintStmt *print_stmt_construct(Expr *expression);

typedef struct {
        Stmt base;
        Token *keyword;
        Expr *value;
} ReturnStmt;

ReturnStmt *return_stmt_construct(Token *keyword, Expr *value);

typedef struct {
        Stmt base;
        Token *name;
        Expr *initializer;
} VarStmt;

VarStmt *var_stmt_construct(Token *name, Expr *initializer);

typedef struct {
        Stmt base;
        Expr *condition;
        Stmt *body;
} WhileStmt;

WhileStmt *while_stmt_construct(Expr *condition, Stmt *body);

#endif
