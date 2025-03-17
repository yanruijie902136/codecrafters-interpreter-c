#include "lox/token.h"
#include "util/xmalloc.h"

#include <stdio.h>

Token *token_construct(TokenType type, char *lexeme) {
        Token *token = xmalloc(sizeof(Token));
        token->type = type;
        token->lexeme = lexeme;
        return token;
}

static const char *token_type_to_string(TokenType type) {
        switch (type) {
        case TOKEN_COMMA:
                return "COMMA";
        case TOKEN_DOT:
                return "DOT";
        case TOKEN_EOF:
                return "EOF";
        case TOKEN_EQUAL:
                return "EQUAL";
        case TOKEN_EQUAL_EQUAL:
                return "EQUAL_EQUAL";
        case TOKEN_LEFT_BRACE:
                return "LEFT_BRACE";
        case TOKEN_LEFT_PAREN:
                return "LEFT_PAREN";
        case TOKEN_MINUS:
                return "MINUS";
        case TOKEN_PLUS:
                return "PLUS";
        case TOKEN_RIGHT_BRACE:
                return "RIGHT_BRACE";
        case TOKEN_RIGHT_PAREN:
                return "RIGHT_PAREN";
        case TOKEN_SEMICOLON:
                return "SEMICOLON";
        case TOKEN_STAR:
                return "STAR";
        }
}

const char *token_to_string(const Token *token) {
        static char str[256];
        snprintf(str, sizeof(str), "%s %s null", token_type_to_string(token->type), token->lexeme);
        return str;
}
