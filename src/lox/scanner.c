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

static char *get_lexeme(void) {
        size_t lexeme_length = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexeme_length);
}

static void add_token(TokenType type) {
        Token *token = token_construct(type, get_lexeme());
        vector_push_back(scanner.tokens, token);
}

Vector *scan_tokens(const char *source) {
        init(source);
        add_token(TOKEN_EOF);
        return scanner.tokens;
}
