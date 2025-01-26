#ifndef CODECRAFTERS_INTERPRETER_TOKEN_H
#define CODECRAFTERS_INTERPRETER_TOKEN_H

#include "lox/object.h"

typedef enum {
        TOKEN_BANG,
        TOKEN_BANG_EQUAL,
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_EOF,
        TOKEN_EQUAL,
        TOKEN_EQUAL_EQUAL,
        TOKEN_GREATER,
        TOKEN_GREATER_EQUAL,
        TOKEN_LEFT_BRACE,
        TOKEN_LEFT_PAREN,
        TOKEN_LESS,
        TOKEN_LESS_EQUAL,
        TOKEN_MINUS,
        TOKEN_PLUS,
        TOKEN_RIGHT_BRACE,
        TOKEN_RIGHT_PAREN,
        TOKEN_SEMICOLON,
        TOKEN_SLASH,
        TOKEN_STAR,
        TOKEN_STRING,
} TokenType;

typedef struct {
        TokenType type;
        const char *lexeme;
        const Object *literal;
} Token;

Token *createToken(TokenType type, const char *lexeme, const Object *literal);
const char *tokenToString(const Token *token);

#endif
