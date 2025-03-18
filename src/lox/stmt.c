#include "lox/stmt.h"
#include "lox/expr.h"
#include "util/xmalloc.h"

ExpressionStmt *expression_stmt_construct(Expr *expression) {
        ExpressionStmt *expression_stmt = xmalloc(sizeof(ExpressionStmt));
        expression_stmt->base.type = STMT_EXPRESSION;
        expression_stmt->expression = expression;
        return expression_stmt;
}

PrintStmt *print_stmt_construct(Expr *expression) {
        PrintStmt *print_stmt = xmalloc(sizeof(PrintStmt));
        print_stmt->base.type = STMT_PRINT;
        print_stmt->expression = expression;
        return print_stmt;
}

VarStmt *var_stmt_construct(Token *name, Expr *initializer) {
        VarStmt *var_stmt = xmalloc(sizeof(VarStmt));
        var_stmt->base.type = STMT_VAR;
        var_stmt->name = name;
        var_stmt->initializer = initializer;
        return var_stmt;
}
