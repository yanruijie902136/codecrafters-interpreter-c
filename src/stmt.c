#include "lox/stmt.h"
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
print_stmt_create(const Expr *expression)
{
        PrintStmt *print_stmt = xmalloc(sizeof(PrintStmt));
        print_stmt->expression = expression;
        return stmt_create(STMT_PRINT, print_stmt);
}
