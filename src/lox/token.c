#include "lox/token.h"
#include "util/xmalloc.h"

#include <stdio.h>

Token *token_construct(TokenType type, char *lexeme, Object *literal, size_t line) {
        Token *token = xmalloc(sizeof(Token));
        token->type = type;
        token->lexeme = lexeme;
        token->literal = literal;
        token->line = line;
        return token;
}

static const char *token_type_to_string(TokenType type) {
        switch (type) {
        case TOKEN_AND:
                return "AND";
        case TOKEN_BANG:
                return "BANG";
        case TOKEN_BANG_EQUAL:
                return "BANG_EQUAL";
        case TOKEN_CLASS:
                return "CLASS";
        case TOKEN_COMMA:
                return "COMMA";
        case TOKEN_DOT:
                return "DOT";
        case TOKEN_ELSE:
                return "ELSE";
        case TOKEN_EOF:
                return "EOF";
        case TOKEN_EQUAL:
                return "EQUAL";
        case TOKEN_EQUAL_EQUAL:
                return "EQUAL_EQUAL";
        case TOKEN_FALSE:
                return "FALSE";
        case TOKEN_FOR:
                return "FOR";
        case TOKEN_FUN:
                return "FUN";
        case TOKEN_GREATER:
                return "GREATER";
        case TOKEN_GREATER_EQUAL:
                return "GREATER_EQUAL";
        case TOKEN_IDENTIFIER:
                return "IDENTIFIER";
        case TOKEN_IF:
                return "IF";
        case TOKEN_LEFT_BRACE:
                return "LEFT_BRACE";
        case TOKEN_LEFT_PAREN:
                return "LEFT_PAREN";
        case TOKEN_LESS:
                return "LESS";
        case TOKEN_LESS_EQUAL:
                return "LESS_EQUAL";
        case TOKEN_MINUS:
                return "MINUS";
        case TOKEN_NIL:
                return "NIL";
        case TOKEN_NUMBER:
                return "NUMBER";
        case TOKEN_OR:
                return "OR";
        case TOKEN_PLUS:
                return "PLUS";
        case TOKEN_PRINT:
                return "PRINT";
        case TOKEN_RETURN:
                return "RETURN";
        case TOKEN_RIGHT_BRACE:
                return "RIGHT_BRACE";
        case TOKEN_RIGHT_PAREN:
                return "RIGHT_PAREN";
        case TOKEN_SEMICOLON:
                return "SEMICOLON";
        case TOKEN_SLASH:
                return "SLASH";
        case TOKEN_STAR:
                return "STAR";
        case TOKEN_STRING:
                return "STRING";
        case TOKEN_SUPER:
                return "SUPER";
        case TOKEN_THIS:
                return "THIS";
        case TOKEN_TRUE:
                return "TRUE";
        case TOKEN_VAR:
                return "VAR";
        case TOKEN_WHILE:
                return "WHILE";
        }
}

const char *token_to_string(const Token *token) {
        static char str[256];
        const char *literal_str = token->literal == NULL ? "null" : object_to_string(token->literal);
        snprintf(str, sizeof(str), "%s %s %s", token_type_to_string(token->type), token->lexeme, literal_str);
        return str;
}
