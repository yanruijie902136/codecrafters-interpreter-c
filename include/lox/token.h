#ifndef CODECRAFTERS_INTERPRETER_TOKEN_H
#define CODECRAFTERS_INTERPRETER_TOKEN_H

typedef enum {
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_EOF,
        TOKEN_LEFT_BRACE,
        TOKEN_LEFT_PAREN,
        TOKEN_MINUS,
        TOKEN_PLUS,
        TOKEN_RIGHT_BRACE,
        TOKEN_RIGHT_PAREN,
        TOKEN_SEMICOLON,
        TOKEN_STAR,
} TokenType;

typedef struct {
        TokenType type;
        const char *lexeme;
} Token;

Token *createToken(TokenType type, const char *lexeme);
const char *tokenToString(const Token *token);

#endif
