#include "lox/token.h"
#include "lox/object.h"
#include "lox/xmalloc.h"

#include <stdio.h>

Token *createToken(TokenType type, const char *lexeme, const Object *literal, size_t line) {
        Token *token = xmalloc(sizeof(Token));
        token->type = type;
        token->lexeme = lexeme;
        token->literal = literal;
        token->line = line;
        return token;
}

static const char *tokenTypeToString(TokenType type) {
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

const char *tokenToString(const Token *token) {
        const char *typeStr = tokenTypeToString(token->type);
        const char *literalStr = (
                token->literal == NULL ? "null" : objectToString(token->literal, false)
        );

        static char str[1024];
        snprintf(str, sizeof(str), "%s %s %s", typeStr, token->lexeme, literalStr);
        return str;
}
