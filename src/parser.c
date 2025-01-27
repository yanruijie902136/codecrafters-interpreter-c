#include "lox/parser.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/stmt.h"
#include "lox/token.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

typedef struct {
        const PtrVector *tokens;
        size_t current;
        jmp_buf env;
} Parser;

static Parser parser;

static void
init(const PtrVector *tokens)
{
        parser.tokens = tokens;
        parser.current = 0;
}

static Token *
peek(void)
{
        return ptr_vector_at(parser.tokens, parser.current);
}

static Token *
previous(void)
{
        return ptr_vector_at(parser.tokens, parser.current - 1);
}

static bool
is_at_end(void)
{
        return peek()->type == TOKEN_EOF;
}

static Token *
advance(void)
{
        if (!is_at_end())
                parser.current++;
        return previous();
}

static bool
match(TokenType type)
{
        if (peek()->type != type)
                return false;
        advance();
        return true;
}

__attribute__((noreturn))
static void
error(const Token *token, const char *format, ...)
{
        fprintf(stderr, "[line %zu] Error at ", token->line);
        if (token->type == TOKEN_EOF)
                fprintf(stderr, "end: ");
        else
                fprintf(stderr, "'%s': ", token->lexeme);
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        longjmp(parser.env, 1);
}

static Expr *parse_expression(void);
static Expr *parse_assignment(void);
static Expr *parse_or(void);
static Expr *parse_and(void);
static Expr *parse_equality(void);
static Expr *parse_comparison(void);
static Expr *parse_term(void);
static Expr *parse_factor(void);
static Expr *parse_unary(void);
static Expr *parse_call(void);
static Expr *finish_call(const Expr *callee);
static Expr *parse_primary(void);

static Stmt *parse_declaration(void);
static Stmt *parse_var_declaration(void);
static Stmt *parse_statement(void);
static PtrVector *parse_block(void);
static Stmt *parse_expression_statement(void);
static Stmt *parse_for_statement(void);
static Stmt *parse_if_statement(void);
static Stmt *parse_print_statement(void);
static Stmt *parse_while_statement(void);

static Expr *
parse_expression(void)
{
        return parse_assignment();
}

static Expr *
parse_assignment(void)
{
        Expr *expr = parse_or();

        if (match(TOKEN_EQUAL))
        {
                Token *equals = previous();
                Expr *value = parse_assignment();

                if (expr->type == EXPR_VARIABLE)
                {
                        const Token *name = ((VariableExpr *)expr->data)->name;
                        return assign_expr_create(name, value);
                }

                error(equals, "Invalid assignment target.");
        }

        return expr;
}

static Expr *
parse_or(void)
{
        Expr *expr = parse_and();
        while (match(TOKEN_OR))
        {
                Token *operator = previous();
                Expr *right = parse_and();
                expr = logical_expr_create(expr, operator, right);
        }
        return expr;
}

static Expr *
parse_and(void)
{
        Expr *expr = parse_equality();
        while (match(TOKEN_AND))
        {
                Token *operator = previous();
                Expr *right = parse_equality();
                expr = logical_expr_create(expr, operator, right);
        }
        return expr;
}

static Expr *
parse_equality(void)
{
        Expr *expr = parse_comparison();
        while (match(TOKEN_BANG_EQUAL) || match(TOKEN_EQUAL_EQUAL))
        {
                Token *operator = previous();
                Expr *right = parse_comparison();
                expr = binary_expr_create(expr, operator, right);
        }
        return expr;
}

static Expr *
parse_comparison(void)
{
        Expr *expr = parse_term();
        while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) ||
                match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL))
        {
                Token *operator = previous();
                Expr *right = parse_term();
                expr = binary_expr_create(expr, operator, right);
        }
        return expr;
}

static Expr *
parse_term(void)
{
        Expr *expr = parse_factor();
        while (match(TOKEN_MINUS) || match(TOKEN_PLUS))
        {
                Token *operator = previous();
                Expr *right = parse_factor();
                expr = binary_expr_create(expr, operator, right);
        }
        return expr;
}

static Expr *
parse_factor(void)
{
        Expr *expr = parse_unary();
        while (match(TOKEN_SLASH) || match(TOKEN_STAR))
        {
                Token *operator = previous();
                Expr *right = parse_unary();
                expr = binary_expr_create(expr, operator, right);
        }
        return expr;
}

static Expr *
parse_unary(void)
{
        if (match(TOKEN_BANG) || match(TOKEN_MINUS))
        {
                Token *operator = previous();
                Expr *right = parse_unary();
                return unary_expr_create(operator, right);
        }
        return parse_call();
}

static Expr *
parse_call(void)
{
        Expr *expr = parse_primary();
        for ( ; ; )
        {
                if (match(TOKEN_LEFT_PAREN))
                        expr = finish_call(expr);
                else
                        break;
        }
        return expr;
}

static Expr *
finish_call(const Expr *callee)
{
        PtrVector *arguments = ptr_vector_create();
        if (peek()->type != TOKEN_RIGHT_PAREN)
        {
                do
                {
                        if (ptr_vector_size(arguments) >= 255)
                                error(peek(), "Can't have more than 255 arguments.");
                        ptr_vector_append(arguments, parse_expression());
                }
                while (match(TOKEN_COMMA));
        }

        if (!match(TOKEN_RIGHT_PAREN))
                error(peek(), "Expect ')' after arguments.");
        Token *paren = previous();

        return call_expr_create(callee, paren, arguments);
}

static Expr *
parse_primary(void)
{
        if (match(TOKEN_FALSE))
                return literal_expr_create(object_create_bool(false));
        if (match(TOKEN_TRUE))
                return literal_expr_create(object_create_bool(true));
        if (match(TOKEN_NIL))
                return literal_expr_create(object_create_nil());
        if (match(TOKEN_NUMBER) || match(TOKEN_STRING))
                return literal_expr_create(previous()->literal);

        if (match(TOKEN_LEFT_PAREN))
        {
                Expr *expression = parse_expression();
                if (!match(TOKEN_RIGHT_PAREN))
                        error(peek(), "Expect ')' after expression.");
                return grouping_expr_create(expression);
        }

        if (match(TOKEN_IDENTIFIER))
                return variable_expr_create(previous());

        error(peek(), "Expect expression.");
}

static Stmt *
parse_declaration(void)
{
        if (match(TOKEN_VAR))
                return parse_var_declaration();
        return parse_statement();
}

static Stmt *
parse_var_declaration(void)
{
        if (!match(TOKEN_IDENTIFIER))
                error(peek(), "Expect variable name.");
        Token *name = previous();

        Expr *initializer = NULL;
        if (match(TOKEN_EQUAL))
                initializer = parse_expression();

        if (!match(TOKEN_SEMICOLON))
                error(peek(), "Expect ';' after variable declaration.");

        return var_stmt_create(name, initializer);
}

static Stmt *
parse_statement(void)
{
        if (match(TOKEN_LEFT_BRACE))
                return block_stmt_create(parse_block());
        if (match(TOKEN_FOR))
                return parse_for_statement();
        if (match(TOKEN_IF))
                return parse_if_statement();
        if (match(TOKEN_PRINT))
                return parse_print_statement();
        if (match(TOKEN_WHILE))
                return parse_while_statement();

        return parse_expression_statement();
}

static PtrVector *
parse_block(void)
{
        PtrVector *statements = ptr_vector_create();
        while (peek()->type != TOKEN_RIGHT_BRACE && !is_at_end())
                ptr_vector_append(statements, parse_declaration());

        if (!match(TOKEN_RIGHT_BRACE))
                error(peek(), "Expect '}' after block.");

        return statements;
}

static Stmt *
parse_expression_statement(void)
{
        Expr *expression = parse_expression();
        if (!match(TOKEN_SEMICOLON))
                error(peek(), "Expect ';' after expression.");
        return expression_stmt_create(expression);
}

static Stmt *
parse_for_statement(void)
{
        if (!match(TOKEN_LEFT_PAREN))
                error(peek(), "Expect '(' after 'for'.");

        Stmt *initializer;
        if (match(TOKEN_SEMICOLON))
                initializer = NULL;
        else if (match(TOKEN_VAR))
                initializer = parse_var_declaration();
        else
                initializer = parse_expression_statement();

        Expr *condition = NULL;
        if (peek()->type != TOKEN_SEMICOLON)
                condition = parse_expression();
        if (!match(TOKEN_SEMICOLON))
                error(peek(), "Expect ';' after loop condition.");

        Expr *increment = NULL;
        if (peek()->type != TOKEN_RIGHT_PAREN)
                increment = parse_expression();
        if (!match(TOKEN_RIGHT_PAREN))
                error(peek(), "Expect ')' after for clauses.");

        Stmt *body = parse_statement();

        if (increment != NULL)
        {
                PtrVector *statements = ptr_vector_create();
                ptr_vector_append(statements, body);
                ptr_vector_append(statements, expression_stmt_create(increment));
                body = block_stmt_create(statements);
        }

        if (condition == NULL)
                condition = literal_expr_create(object_create_bool(true));
        body = while_stmt_create(condition, body);

        if (initializer != NULL)
        {
                PtrVector *statements = ptr_vector_create();
                ptr_vector_append(statements, initializer);
                ptr_vector_append(statements, body);
                body = block_stmt_create(statements);
        }

        return body;
}

static Stmt *
parse_if_statement(void)
{
        if (!match(TOKEN_LEFT_PAREN))
                error(peek(), "Expect '(' after 'if'.");

        Expr *condition = parse_expression();

        if (!match(TOKEN_RIGHT_PAREN))
                error(peek(), "Expect ')' after if condition.");

        Stmt *then_branch = parse_statement();
        Stmt *else_branch = NULL;
        if (match(TOKEN_ELSE))
                else_branch = parse_statement();

        return if_stmt_create(condition, then_branch, else_branch);
}

static Stmt *
parse_print_statement(void)
{
        Expr *expression = parse_expression();
        if (!match(TOKEN_SEMICOLON))
                error(peek(), "Expect ';' after value.");
        return print_stmt_create(expression);
}

static Stmt *
parse_while_statement(void)
{
        if (!match(TOKEN_LEFT_PAREN))
                error(peek(), "Expect '(' after 'while'.");

        Expr *condition = parse_expression();

        if (!match(TOKEN_RIGHT_PAREN))
                error(peek(), "Expect ')' after condition.");

        Stmt *body = parse_statement();
        return while_stmt_create(condition, body);
}

Expr *
parse_to_expr(const PtrVector *tokens)
{
        init(tokens);

        if (setjmp(parser.env) != 0)
                return NULL;
        return parse_expression();
}

PtrVector *
parse_to_stmts(const PtrVector *tokens)
{
        init(tokens);

        if (setjmp(parser.env) != 0)
                return NULL;

        PtrVector *statements = ptr_vector_create();
        while (!is_at_end())
                ptr_vector_append(statements, parse_declaration());
        return statements;
}
