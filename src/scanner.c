#include "lox/scanner.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

#include <stdbool.h>

typedef struct {
        const char *start;
        const char *current;
        PtrVector *tokens;
} Scanner;

static Scanner scanner;

static void init(const char *source) {
        scanner.start = source;
        scanner.current = source;
        scanner.tokens = createPtrVector();
}

static char *getLexeme(void) {
        size_t lexemeLength = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexemeLength);
}

static void addToken(TokenType type) {
        Token *token = createToken(type, getLexeme());
        ptrVectorAppend(scanner.tokens, token);
}

PtrVector *scanTokens(const char *source) {
        init(source);

        addToken(TOKEN_EOF);

        return scanner.tokens;
}
