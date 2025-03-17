#include "lox/token.h"
#include "util/xmalloc.h"

#include <stdio.h>
#include <stdlib.h>

Token *token_construct(TokenType type, char *lexeme) {
        Token *token = xmalloc(sizeof(Token));
        token->type = type;
        token->lexeme = lexeme;
        return token;
}

void token_destruct(void *pointer) {
        if (pointer == NULL) {
                return;
        }
        Token *token = pointer;
        free(token->lexeme);
        free(token);
}

static const char *token_type_to_string(TokenType type) {
        switch (type) {
        case TOKEN_EOF:
                return "EOF";
        case TOKEN_LEFT_BRACE:
                return "LEFT_BRACE";
        case TOKEN_LEFT_PAREN:
                return "LEFT_PAREN";
        case TOKEN_RIGHT_BRACE:
                return "RIGHT_BRACE";
        case TOKEN_RIGHT_PAREN:
                return "RIGHT_PAREN";
        }
}

const char *token_to_string(const Token *token) {
        static char str[256];
        snprintf(str, sizeof(str), "%s %s null", token_type_to_string(token->type), token->lexeme);
        return str;
}
