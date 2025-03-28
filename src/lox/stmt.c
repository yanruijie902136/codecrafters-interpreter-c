#include "lox/stmt.h"
#include "lox/expr.h"
#include "util/xmalloc.h"

BlockStmt *block_stmt_construct(Vector *statements) {
        BlockStmt *block_stmt = xmalloc(sizeof(BlockStmt));
        block_stmt->base.type = STMT_BLOCK;
        block_stmt->statements = statements;
        return block_stmt;
}

ClassStmt *class_stmt_construct(Token *name, VariableExpr *superclass, Vector *methods) {
        ClassStmt *class_stmt = xmalloc(sizeof(ClassStmt));
        class_stmt->base.type = STMT_CLASS;
        class_stmt->name = name;
        class_stmt->superclass = superclass;
        class_stmt->methods = methods;
        return class_stmt;
}

ExpressionStmt *expression_stmt_construct(Expr *expression) {
        ExpressionStmt *expression_stmt = xmalloc(sizeof(ExpressionStmt));
        expression_stmt->base.type = STMT_EXPRESSION;
        expression_stmt->expression = expression;
        return expression_stmt;
}

FunctionStmt *function_stmt_construct(Token *name, Vector *params, Vector *body) {
        FunctionStmt *function_stmt = xmalloc(sizeof(FunctionStmt));
        function_stmt->base.type = STMT_FUNCTION;
        function_stmt->name = name;
        function_stmt->params = params;
        function_stmt->body = body;
        return function_stmt;
}

IfStmt *if_stmt_construct(Expr *condition, Stmt *then_branch, Stmt *else_branch) {
        IfStmt *if_stmt = xmalloc(sizeof(IfStmt));
        if_stmt->base.type = STMT_IF;
        if_stmt->condition = condition;
        if_stmt->then_branch = then_branch;
        if_stmt->else_branch = else_branch;
        return if_stmt;
}

PrintStmt *print_stmt_construct(Expr *expression) {
        PrintStmt *print_stmt = xmalloc(sizeof(PrintStmt));
        print_stmt->base.type = STMT_PRINT;
        print_stmt->expression = expression;
        return print_stmt;
}

ReturnStmt *return_stmt_construct(Token *keyword, Expr *value) {
        ReturnStmt *return_stmt = xmalloc(sizeof(ReturnStmt));
        return_stmt->base.type = STMT_RETURN;
        return_stmt->keyword = keyword;
        return_stmt->value = value;
        return return_stmt;
}

VarStmt *var_stmt_construct(Token *name, Expr *initializer) {
        VarStmt *var_stmt = xmalloc(sizeof(VarStmt));
        var_stmt->base.type = STMT_VAR;
        var_stmt->name = name;
        var_stmt->initializer = initializer;
        return var_stmt;
}

WhileStmt *while_stmt_construct(Expr *condition, Stmt *body) {
        WhileStmt *while_stmt = xmalloc(sizeof(WhileStmt));
        while_stmt->base.type = STMT_WHILE;
        while_stmt->condition = condition;
        while_stmt->body = body;
        return while_stmt;
}
