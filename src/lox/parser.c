#include "lox/parser.h"
#include "lox/errors.h"
#include "lox/expr.h"
#include "lox/stmt.h"
#include "lox/token.h"
#include "lox/object.h"
#include "util/vector.h"

#include <stdio.h>

static struct {
        const Vector *tokens;
        size_t current;
} parser;

static void init(const Vector *tokens) {
        parser.tokens = tokens;
        parser.current = 0;
}

static Token *peek(void) {
        return vector_at(parser.tokens, parser.current);
}

static bool check(TokenType type) {
        return peek()->type == type;
}

static bool is_at_end(void) {
        return check(TOKEN_EOF);
}

static Token *previous(void) {
        return vector_at(parser.tokens, parser.current - 1);
}

static Token *advance(void) {
        if (!is_at_end()) {
                parser.current++;
        }
        return previous();
}

static bool match(TokenType type) {
        if (!check(type)) {
                return false;
        }
        advance();
        return true;
}

static Expr *expression(void);
static Expr *assignment(void);
static Expr *or(void);
static Expr *and(void);
static Expr *equality(void);
static Expr *comparison(void);
static Expr *term(void);
static Expr *factor(void);
static Expr *unary(void);
static Expr *call(void);
static Expr *finish_call(Expr *callee);
static Expr *primary(void);

static Stmt *declaration(void);
static Stmt *class_declaration(void);
static FunctionStmt *function(const char *kind);
static Stmt *var_declaration(void);
static Stmt *statement(void);
static Stmt *for_statement(void);
static Stmt *if_statement(void);
static Stmt *print_statement(void);
static Stmt *return_statement(void);
static Stmt *while_statement(void);
static Stmt *expression_statement(void);
static Vector *block(void);

static Expr *expression(void) {
        return assignment();
}

static Expr *assignment(void) {
        Expr *expr = or();
        if (match(TOKEN_EQUAL)) {
                Token *equals = previous();
                Expr *value = assignment();

                if (expr->type == EXPR_VARIABLE) {
                        Token *name = ((VariableExpr *)expr)->name;
                        return (Expr *)assign_expr_construct(name, value);
                } else if (expr->type == EXPR_GET) {
                        GetExpr *get_expr = (GetExpr *)expr;
                        return (Expr *)set_expr_construct(get_expr->object, get_expr->name, value);
                }

                parse_error(equals, "Invalid assignment target.");
        }
        return expr;
}

static Expr *or(void) {
        Expr *expr = and();
        while (match(TOKEN_OR)) {
                Token *operator = previous();
                Expr *right = and();
                expr = (Expr *)logical_expr_construct(expr, operator, right);
        }
        return expr;
}

static Expr *and(void) {
        Expr *expr = equality();
        while (match(TOKEN_AND)) {
                Token *operator = previous();
                Expr *right = equality();
                expr = (Expr *)logical_expr_construct(expr, operator, right);
        }
        return expr;
}

static Expr *equality(void) {
        Expr *expr = comparison();
        while (match(TOKEN_BANG_EQUAL) || match(TOKEN_EQUAL_EQUAL)) {
                Token *operator = previous();
                Expr *right = comparison();
                expr = (Expr *)binary_expr_construct(expr, operator, right);
        }
        return expr;
}

static Expr *comparison(void) {
        Expr *expr = term();
        while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) || match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)) {
                Token *operator = previous();
                Expr *right = term();
                expr = (Expr *)binary_expr_construct(expr, operator, right);
        }
        return expr;
}

static Expr *term(void) {
        Expr *expr = factor();
        while (match(TOKEN_MINUS) || match(TOKEN_PLUS)) {
                Token *operator = previous();
                Expr *right = factor();
                expr = (Expr *)binary_expr_construct(expr, operator, right);
        }
        return expr;
}

static Expr *factor(void) {
        Expr *expr = unary();
        while (match(TOKEN_SLASH) || match(TOKEN_STAR)) {
                Token *operator = previous();
                Expr *right = unary();
                expr = (Expr *)binary_expr_construct(expr, operator, right);
        }
        return expr;
}

static Expr *unary(void) {
        if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
                Token *operator = previous();
                Expr *right = unary();
                return (Expr *)unary_expr_construct(operator, right);
        }
        return call();
}

static Expr *call(void) {
        Expr *expr = primary();
        for ( ; ; ) {
                if (match(TOKEN_LEFT_PAREN)) {
                        expr = finish_call(expr);
                } else if (match(TOKEN_DOT)) {
                        if (!match(TOKEN_IDENTIFIER)) {
                                parse_error(peek(), "Expect property name after '.'.");
                        }
                        expr = (Expr *)get_expr_construct(expr, previous());
                } else {
                        break;
                }
        }
        return expr;
}

static Expr *finish_call(Expr *callee) {
        Vector *arguments = vector_construct();
        if (!check(TOKEN_RIGHT_PAREN)) {
                do {
                        if (vector_size(arguments) >= 255) {
                                parse_error(peek(), "Can't have more than 255 arguments.");
                        }
                        vector_push_back(arguments, expression());
                } while (match(TOKEN_COMMA));
        }

        if (!match(TOKEN_RIGHT_PAREN)) {
                parse_error(peek(), "Expect ')' after arguments.");
        }
        Token *paren = previous();

        return (Expr *)call_expr_construct(callee, paren, arguments);
}

static Expr *primary(void) {
        if (match(TOKEN_FALSE)) {
                return (Expr *)literal_expr_construct(boolean_object_construct(false));
        }
        if (match(TOKEN_TRUE)) {
                return (Expr *)literal_expr_construct(boolean_object_construct(true));
        }
        if (match(TOKEN_NIL)) {
                return (Expr *)literal_expr_construct(nil_object_construct());
        }

        if (match(TOKEN_NUMBER) || match(TOKEN_STRING)) {
                return (Expr *)literal_expr_construct(previous()->literal);
        }

        if (match(TOKEN_SUPER)) {
                Token *keyword = previous();
                if (!match(TOKEN_DOT)) {
                        parse_error(peek(), "Expect '.' after 'super'.");
                }
                if (!match(TOKEN_IDENTIFIER)) {
                        parse_error(peek(), "Expect superclass method name.");
                }
                return (Expr *)super_expr_construct(keyword, previous());
        }

        if (match(TOKEN_THIS)) {
                return (Expr *)this_expr_construct(previous());
        }

        if (match(TOKEN_IDENTIFIER)) {
                return (Expr *)variable_expr_construct(previous());
        }

        if (match(TOKEN_LEFT_PAREN)) {
                Expr *expr = expression();
                if (!match(TOKEN_RIGHT_PAREN)) {
                        parse_error(peek(), "Expect ')' after expression.");
                }
                return (Expr *)grouping_expr_construct(expr);
        }

        parse_error(peek(), "Expect expression.");
}

static Stmt *declaration(void) {
        if (match(TOKEN_CLASS)) {
                return class_declaration();
        }
        if (match(TOKEN_FUN)) {
                return (Stmt *)function("function");
        }
        if (match(TOKEN_VAR)) {
                return var_declaration();
        }
        return statement();
}

static Stmt *class_declaration(void) {
        if (!match(TOKEN_IDENTIFIER)) {
                parse_error(peek(), "Expect class name.");
        }
        Token *name = previous();

        VariableExpr *superclass = NULL;
        if (match(TOKEN_LESS)) {
                if (!match(TOKEN_IDENTIFIER)) {
                        parse_error(peek(), "Expect superclass name.");
                }
                superclass = variable_expr_construct(previous());
        }

        if (!match(TOKEN_LEFT_BRACE)) {
                parse_error(peek(), "Expect '{' before class body.");
        }

        Vector *methods = vector_construct();
        while (!check(TOKEN_RIGHT_BRACE) && !is_at_end()) {
                vector_push_back(methods, function("method"));
        }

        if (!match(TOKEN_RIGHT_BRACE)) {
                parse_error(peek(), "Expect '}' after class body.");
        }

        return (Stmt *)class_stmt_construct(name, superclass, methods);
}

static FunctionStmt *function(const char *kind) {
        if (!match(TOKEN_IDENTIFIER)) {
                parse_error(peek(), "Expect %s name.", kind);
        }
        Token *name = previous();
        if (!match(TOKEN_LEFT_PAREN)) {
                parse_error(peek(), "Expect '(' after %s name.", kind);
        }

        Vector *params = vector_construct();
        if (!check(TOKEN_RIGHT_PAREN)) {
                do {
                        if (vector_size(params) >= 255) {
                                parse_error(peek(), "Can't have more than 255 parameters.");
                        }
                        if (!match(TOKEN_IDENTIFIER)) {
                                parse_error(peek(), "Expect parameter name.");
                        }
                        vector_push_back(params, previous());
                } while (match(TOKEN_COMMA));
        }
        if (!match(TOKEN_RIGHT_PAREN)) {
                parse_error(peek(), "Expect ')' after parameters.");
        }

        if (!match(TOKEN_LEFT_BRACE)) {
                parse_error(peek(), "Expect '{' before %s body.", kind);
        }
        Vector *body = block();
        return function_stmt_construct(name, params, body);
}

static Stmt *var_declaration(void) {
        if (!match(TOKEN_IDENTIFIER)) {
                parse_error(peek(), "Expect variable name.");
        }
        Token *name = previous();

        Expr *initializer = NULL;
        if (match(TOKEN_EQUAL)) {
                initializer = expression();
        }

        if (!match(TOKEN_SEMICOLON)) {
                parse_error(peek(), "Expect ';' after variable declaration.");
        }

        return (Stmt *)var_stmt_construct(name, initializer);
}

static Stmt *statement(void) {
        if (match(TOKEN_FOR)) {
                return for_statement();
        }
        if (match(TOKEN_IF)) {
                return if_statement();
        }
        if (match(TOKEN_PRINT)) {
                return print_statement();
        }
        if (match(TOKEN_RETURN)) {
                return return_statement();
        }
        if (match(TOKEN_WHILE)) {
                return while_statement();
        }
        if (match(TOKEN_LEFT_BRACE)) {
                return (Stmt *)block_stmt_construct(block());
        }
        return expression_statement();
}

static Stmt *for_statement(void) {
        if (!match(TOKEN_LEFT_PAREN)) {
                parse_error(peek(), "Expect '(' after 'for'.");
        }

        Stmt *initializer;
        if (match(TOKEN_SEMICOLON)) {
                initializer = NULL;
        } else if (match(TOKEN_VAR)) {
                initializer = var_declaration();
        } else {
                initializer = expression_statement();
        }

        Expr *condition = NULL;
        if (!check(TOKEN_SEMICOLON)) {
                condition = expression();
        }
        if (!match(TOKEN_SEMICOLON)) {
                parse_error(peek(), "Expect ';' after loop condition.");
        }

        Expr *increment = NULL;
        if (!check(TOKEN_RIGHT_PAREN)) {
                increment = expression();
        }
        if (!match(TOKEN_RIGHT_PAREN)) {
                parse_error(peek(), "Expect ')' after for clauses.");
        }

        Stmt *body = statement();

        if (increment != NULL) {
                Vector *statements = vector_construct();
                vector_push_back(statements, body);
                vector_push_back(statements, expression_stmt_construct(increment));
                body = (Stmt *)block_stmt_construct(statements);
        }

        if (condition == NULL) {
                condition = (Expr *)literal_expr_construct(boolean_object_construct(true));
        }
        body = (Stmt *)while_stmt_construct(condition, body);

        if (initializer != NULL) {
                Vector *statements = vector_construct();
                vector_push_back(statements, initializer);
                vector_push_back(statements, body);
                body = (Stmt *)block_stmt_construct(statements);
        }

        return body;
}

static Stmt *if_statement(void) {
        if (!match(TOKEN_LEFT_PAREN)) {
                parse_error(peek(), "Expect '(' after 'if'.");
        }
        Expr *condition = expression();
        if (!match(TOKEN_RIGHT_PAREN)) {
                parse_error(peek(), "Expect ')' after if condition.");
        }

        Stmt *then_branch = statement();
        Stmt *else_branch = NULL;
        if (match(TOKEN_ELSE)) {
                else_branch = statement();
        }

        return (Stmt *)if_stmt_construct(condition, then_branch, else_branch);
}

static Stmt *print_statement(void) {
        Expr *expr = expression();
        if (!match(TOKEN_SEMICOLON)) {
                parse_error(peek(), "Expect ';' after value.");
        }
        return (Stmt *)print_stmt_construct(expr);
}

static Stmt *return_statement(void) {
        Token *keyword = previous();

        Expr *value = NULL;
        if (!check(TOKEN_SEMICOLON)) {
                value = expression();
        }
        if (!match(TOKEN_SEMICOLON)) {
                parse_error(peek(), "Expect ';' after return value.");
        }

        return (Stmt *)return_stmt_construct(keyword, value);
}

static Stmt *while_statement(void) {
        if (!match(TOKEN_LEFT_PAREN)) {
                parse_error(peek(), "Expect '(' after 'while'.");
        }
        Expr *condition = expression();
        if (!match(TOKEN_RIGHT_PAREN)) {
                parse_error(peek(), "Expect ')' after condition.");
        }
        Stmt *body = statement();
        return (Stmt *)while_stmt_construct(condition, body);
}

static Stmt *expression_statement(void) {
        Expr *expr = expression();
        if (!match(TOKEN_SEMICOLON)) {
                parse_error(peek(), "Expect ';' after expression.");
        }
        return (Stmt *)expression_stmt_construct(expr);
}

static Vector *block(void) {
        Vector *statements = vector_construct();
        while (!check(TOKEN_RIGHT_BRACE) && !is_at_end()) {
                vector_push_back(statements, declaration());
        }
        if (!match(TOKEN_RIGHT_BRACE)) {
                parse_error(peek(), "Expect '}' after block.");
        }
        return statements;
}

Expr *parse_expr(const Vector *tokens) {
        init(tokens);
        return expression();
}

Vector *parse_stmts(const Vector *tokens) {
        init(tokens);
        Vector *statements = vector_construct();
        while (!is_at_end()) {
                vector_push_back(statements, declaration());
        }
        return statements;
}
