#include "lox/scanner.h"
#include "lox/token.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <stdbool.h>

static struct {
        const char *start;
        const char *current;
        Vector *tokens;
} scanner;

static void init(const char *source) {
        scanner.start = source;
        scanner.current = source;
        scanner.tokens = vector_construct();
}

static char peek(void) {
        return *scanner.current;
}

static bool is_at_end(void) {
        return peek() == '\0';
}

static char advance(void) {
        return *scanner.current++;
}

static char *get_lexeme(void) {
        size_t lexeme_length = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexeme_length);
}

static void add_token(TokenType type) {
        Token *token = token_construct(type, get_lexeme());
        vector_push_back(scanner.tokens, token);
}

static void scan_token(void) {
        char c = advance();
        switch (c) {
        case '(':
                add_token(TOKEN_LEFT_PAREN);
                break;
        case ')':
                add_token(TOKEN_RIGHT_PAREN);
                break;
        }
}

Vector *scan_tokens(const char *source) {
        init(source);
        while (!is_at_end()) {
                scan_token();
                scanner.start = scanner.current;
        }
        add_token(TOKEN_EOF);
        return scanner.tokens;
}
