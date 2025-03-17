#ifndef CODECRAFTERS_INTERPRETER_LOX_TOKEN_H
#define CODECRAFTERS_INTERPRETER_LOX_TOKEN_H

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
        char *lexeme;
} Token;

Token *token_construct(TokenType type, char *lexeme);
void token_destruct(void *token);
const char *token_to_string(const Token *token);

#endif
