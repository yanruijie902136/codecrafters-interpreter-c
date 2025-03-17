#include "lox/parser.h"
#include "lox/expr.h"
#include "lox/token.h"
#include "lox/object.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>

static struct {
        const Vector *tokens;
        size_t current;
        jmp_buf env;
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

__attribute__((noreturn))
static void verror(const Token *token, const char *format, va_list ap) {
        fprintf(stderr, "[line %zu] Error ", token->line);
        if (token->type == TOKEN_EOF) {
                fprintf(stderr, "at end: ");
        } else {
                fprintf(stderr, "at '%s': ", token->lexeme);
        }
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        longjmp(parser.env, 1);
}

__attribute__((noreturn))
static void error(const Token *token, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        verror(token, format, ap);
}

static Token *consume(TokenType type, const char *format, ...) {
        if (match(type)) {
                return previous();
        }
        va_list ap;
        va_start(ap, format);
        verror(peek(), format, ap);
}

static Expr *expression(void);
static Expr *equality(void);
static Expr *comparison(void);
static Expr *term(void);
static Expr *factor(void);
static Expr *unary(void);
static Expr *primary(void);

static Expr *expression(void) {
        return equality();
}

static Expr *equality(void) {
        Expr *expr = comparison();
        while (match(TOKEN_BANG_EQUAL) || match(TOKEN_EQUAL_EQUAL)) {
                Token *operator = previous();
                expr = (Expr *)binary_expr_construct(expr, operator, comparison());
        }
        return expr;
}

static Expr *comparison(void) {
        Expr *expr = term();
        while (match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) || match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)) {
                Token *operator = previous();
                expr = (Expr *)binary_expr_construct(expr, operator, term());
        }
        return expr;
}

static Expr *term(void) {
        Expr *expr = factor();
        while (match(TOKEN_MINUS) || match(TOKEN_PLUS)) {
                Token *operator = previous();
                expr = (Expr *)binary_expr_construct(expr, operator, factor());
        }
        return expr;
}

static Expr *factor(void) {
        Expr *expr = unary();
        while (match(TOKEN_SLASH) || match(TOKEN_STAR)) {
                Token *operator = previous();
                expr = (Expr *)binary_expr_construct(expr, operator, unary());
        }
        return expr;
}

static Expr *unary(void) {
        if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
                Token *operator = previous();
                return (Expr *)unary_expr_construct(operator, unary());
        }
        return primary();
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

        if (match(TOKEN_LEFT_PAREN)) {
                Expr *expr = expression();
                consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
                return (Expr *)grouping_expr_construct(expr);
        }

        error(peek(), "Expect expression.");
}

Expr *parse_expr(const Vector *tokens) {
        init(tokens);
        if (setjmp(parser.env) != 0) {
                return NULL;
        }
        return expression();
}
