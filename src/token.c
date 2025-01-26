#include "lox/token.h"
#include "lox/xmalloc.h"

#include <stdio.h>

Token *createToken(TokenType type, const char *lexeme) {
        Token *token = xmalloc(sizeof(Token));
        token->type = type;
        token->lexeme = lexeme;
        return token;
}

static const char *tokenTypeToString(TokenType type) {
        switch (type) {
        case TOKEN_EOF:
                return "EOF";
        }
}

const char *tokenToString(const Token *token) {
        static char str[1024];
        snprintf(str, sizeof(str), "%s %s null", tokenTypeToString(token->type), token->lexeme);
        return str;
}
