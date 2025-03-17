#ifndef CODECRAFTERS_INTERPRETER_LOX_TOKEN_H
#define CODECRAFTERS_INTERPRETER_LOX_TOKEN_H

#include <stddef.h>

#include "lox/object.h"

typedef enum {
        TOKEN_AND,
        TOKEN_BANG,
        TOKEN_BANG_EQUAL,
        TOKEN_CLASS,
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_ELSE,
        TOKEN_EOF,
        TOKEN_EQUAL,
        TOKEN_EQUAL_EQUAL,
        TOKEN_FALSE,
        TOKEN_FOR,
        TOKEN_FUN,
        TOKEN_GREATER,
        TOKEN_GREATER_EQUAL,
        TOKEN_IDENTIFIER,
        TOKEN_IF,
        TOKEN_LEFT_BRACE,
        TOKEN_LEFT_PAREN,
        TOKEN_LESS,
        TOKEN_LESS_EQUAL,
        TOKEN_MINUS,
        TOKEN_NIL,
        TOKEN_NUMBER,
        TOKEN_OR,
        TOKEN_PLUS,
        TOKEN_PRINT,
        TOKEN_RETURN,
        TOKEN_RIGHT_BRACE,
        TOKEN_RIGHT_PAREN,
        TOKEN_SEMICOLON,
        TOKEN_SLASH,
        TOKEN_STAR,
        TOKEN_STRING,
        TOKEN_SUPER,
        TOKEN_THIS,
        TOKEN_TRUE,
        TOKEN_VAR,
        TOKEN_WHILE,
} TokenType;

typedef struct {
        TokenType type;
        char *lexeme;
        Object *literal;
        size_t line;
} Token;

Token *token_construct(TokenType type, char *lexeme, Object *literal, size_t line);
const char *token_to_string(const Token *token);

#endif
