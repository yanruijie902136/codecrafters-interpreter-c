#include "lox/parser.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
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
static Expr *parse_equality(void);
static Expr *parse_comparison(void);
static Expr *parse_term(void);
static Expr *parse_factor(void);
static Expr *parse_unary(void);
static Expr *parse_primary(void);

static Expr *
parse_expression(void)
{
        return parse_equality();
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
        return parse_primary();
}

static Expr *
parse_primary(void)
{
        if (match(TOKEN_FALSE))
                return literal_expr_create(object_create_bool(false));
        else if (match(TOKEN_TRUE))
                return literal_expr_create(object_create_bool(true));
        else if (match(TOKEN_NIL))
                return literal_expr_create(object_create_nil());
        else if (match(TOKEN_NUMBER) || match(TOKEN_STRING))
                return literal_expr_create(previous()->literal);
        else if (match(TOKEN_LEFT_PAREN))
        {
                Expr *expression = parse_expression();
                if (!match(TOKEN_RIGHT_PAREN))
                        error(peek(), "Expect ')' after expression.");
                return grouping_expr_create(expression);
        }

        error(peek(), "Expect expression.");
}

Expr *
parse_to_expr(const PtrVector *tokens)
{
        init(tokens);
        if (setjmp(parser.env) == 0)
                return parse_expression();
        return NULL;
}
