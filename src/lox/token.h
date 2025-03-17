#ifndef CODECRAFTERS_INTERPRETER_LOX_TOKEN_H
#define CODECRAFTERS_INTERPRETER_LOX_TOKEN_H

typedef enum {
        TOKEN_EOF,
} TokenType;

typedef struct {
        TokenType type;
        char *lexeme;
} Token;

Token *token_construct(TokenType type, char *lexeme);
void token_destruct(void *token);
const char *token_to_string(const Token *token);

#endif
