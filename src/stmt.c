#include "lox/stmt.h"
#include "lox/ptr_vector.h"
#include "lox/xmalloc.h"

static Stmt *
stmt_create(StmtType type, const void *data)
{
        Stmt *stmt = xmalloc(sizeof(Stmt));
        stmt->type = type;
        stmt->data = data;
        return stmt;
}

Stmt *
block_stmt_create(const PtrVector *statements)
{
        BlockStmt *block_stmt = xmalloc(sizeof(BlockStmt));
        block_stmt->statements = statements;
        return stmt_create(STMT_BLOCK, block_stmt);
}

Stmt *
expression_stmt_create(const Expr *expression)
{
        ExpressionStmt *expression_stmt = xmalloc(sizeof(ExpressionStmt));
        expression_stmt->expression = expression;
        return stmt_create(STMT_EXPRESSION, expression_stmt);
}

Stmt *
if_stmt_create(const Expr *condition, const Stmt *then_branch, const Stmt *else_branch)
{
        IfStmt *if_stmt = xmalloc(sizeof(IfStmt));
        if_stmt->condition = condition;
        if_stmt->then_branch = then_branch;
        if_stmt->else_branch = else_branch;
        return stmt_create(STMT_IF, if_stmt);
}

Stmt *
print_stmt_create(const Expr *expression)
{
        PrintStmt *print_stmt = xmalloc(sizeof(PrintStmt));
        print_stmt->expression = expression;
        return stmt_create(STMT_PRINT, print_stmt);
}

Stmt *
var_stmt_create(const Token *name, const Expr *initializer)
{
        VarStmt *var_stmt = xmalloc(sizeof(VarStmt));
        var_stmt->name = name;
        var_stmt->initializer = initializer;
        return stmt_create(STMT_VAR, var_stmt);
}

Stmt *
while_stmt_create(const Expr *condition, const Stmt *body)
{
        WhileStmt *while_stmt = xmalloc(sizeof(WhileStmt));
        while_stmt->condition = condition;
        while_stmt->body = body;
        return stmt_create(STMT_WHILE, while_stmt);
}
