#include "lox/parser.h"
#include "lox/expr.h"
#include "lox/token.h"
#include "lox/object.h"

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

static Expr *parse_expression(void);
static Expr *parse_primary(void);

static Expr *parse_expression(void) {
        return parse_primary();
}

static Expr *parse_primary(void) {
        if (match(TOKEN_FALSE)) {
                return (Expr *)literal_expr_construct(boolean_object_construct(false));
        }
        if (match(TOKEN_TRUE)) {
                return (Expr *)literal_expr_construct(boolean_object_construct(true));
        }
        if (match(TOKEN_NIL)) {
                return (Expr *)literal_expr_construct(nil_object_construct());
        }

        return NULL;
}

Expr *parse_expr(const Vector *tokens) {
        init(tokens);
        return parse_expression();
}
