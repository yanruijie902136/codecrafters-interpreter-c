#include "lox/stmt.h"
#include "lox/expr.h"
#include "util/xmalloc.h"

PrintStmt *print_stmt_construct(Expr *expression) {
        PrintStmt *print_stmt = xmalloc(sizeof(PrintStmt));
        print_stmt->base.type = STMT_PRINT;
        print_stmt->expression = expression;
        return print_stmt;
}
