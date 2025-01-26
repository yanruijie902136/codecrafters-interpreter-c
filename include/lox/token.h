#ifndef CODECRAFTERS_INTERPRETER_TOKEN_H
#define CODECRAFTERS_INTERPRETER_TOKEN_H

typedef enum {
        TOKEN_EOF,
        TOKEN_LEFT_PAREN,
        TOKEN_RIGHT_PAREN,
} TokenType;

typedef struct {
        TokenType type;
        const char *lexeme;
} Token;

Token *createToken(TokenType type, const char *lexeme);
const char *tokenToString(const Token *token);

#endif
