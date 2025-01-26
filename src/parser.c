#include "lox/parser.h"
#include "lox/expr.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"

typedef struct {
        const PtrVector *tokens;
        size_t current;
} Parser;

static Parser parser;

static void init(const PtrVector *tokens) {
        parser.tokens = tokens;
        parser.current = 0;
}

static Token *peek(void) {
        return ptrVectorAt(parser.tokens, parser.current);
}

static Token *previous(void) {
        return ptrVectorAt(parser.tokens, parser.current - 1);
}

static bool isAtEnd(void) {
        return peek()->type == TOKEN_EOF;
}

static Token *advance(void) {
        if (!isAtEnd()) {
                parser.current++;
        }
        return previous();
}

static bool match(TokenType type) {
        if (peek()->type != type) {
                return false;
        }
        advance();
        return true;
}

static Expr *parseExpression(void);
static Expr *parseTerm(void);
static Expr *parseFactor(void);
static Expr *parseUnary(void);
static Expr *parsePrimary(void);

static Expr *parseExpression(void) {
        return parseTerm();
}

static Expr *parseTerm(void) {
        Expr *expr = parseFactor();
        while (match(TOKEN_MINUS) || match(TOKEN_PLUS)) {
                Token *operator = previous();
                Expr *right = parseFactor();
                expr = createBinaryExpr(expr, operator, right);
        }
        return expr;
}

static Expr *parseFactor(void) {
        Expr *expr = parseUnary();
        while (match(TOKEN_SLASH) || match(TOKEN_STAR)) {
                Token *operator = previous();
                Expr *right = parseUnary();
                expr = createBinaryExpr(expr, operator, right);
        }
        return expr;
}

static Expr *parseUnary(void) {
        if (match(TOKEN_BANG) || match(TOKEN_MINUS)) {
                Token *operator = previous();
                Expr *right = parseUnary();
                return createUnaryExpr(operator, right);
        }
        return parsePrimary();
}

static Expr *parsePrimary(void) {
        if (match(TOKEN_FALSE)) {
                return createLiteralExpr(createBoolObject(false));
        }
        else if (match(TOKEN_TRUE)) {
                return createLiteralExpr(createBoolObject(true));
        }
        else if (match(TOKEN_NIL)) {
                return createLiteralExpr(createNilObject());
        }
        else if (match(TOKEN_NUMBER) || match(TOKEN_STRING)) {
                return createLiteralExpr(previous()->literal);
        }
        else if (match(TOKEN_LEFT_PAREN)) {
                Expr *expression = parseExpression();
                advance();
                return createGroupingExpr(expression);
        }

        return NULL;
}

Expr *parseToExpr(const PtrVector *tokens) {
        init(tokens);
        return parseExpression();
}
