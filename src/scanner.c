#include "lox/scanner.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
        const char *start;
        const char *current;
        PtrVector *tokens;
        bool hasError;
} Scanner;

static Scanner scanner;

static void init(const char *source) {
        scanner.start = source;
        scanner.current = source;
        scanner.tokens = createPtrVector();
        scanner.hasError = false;
}

static bool isAtEnd(void) {
        return *scanner.current == '\0';
}

static char advance(void) {
        return *scanner.current++;
}

static char peek(void) {
        return *scanner.current;
}

static bool match(char expected) {
        if (peek() != expected) {
                return false;
        }
        advance();
        return true;
}

static char *getLexeme(void) {
        size_t lexemeLength = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexemeLength);
}

static void addToken(TokenType type) {
        Token *token = createToken(type, getLexeme());
        ptrVectorAppend(scanner.tokens, token);
}

static void error(const char *format, ...) {
        fprintf(stderr, "[line 1] Error: ");
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        scanner.hasError = true;
}

static void scanToken(void) {
        char c = advance();
        switch (c) {
        case '(':
                addToken(TOKEN_LEFT_PAREN);
                break;
        case ')':
                addToken(TOKEN_RIGHT_PAREN);
                break;
        case '{':
                addToken(TOKEN_LEFT_BRACE);
                break;
        case '}':
                addToken(TOKEN_RIGHT_BRACE);
                break;
        case ',':
                addToken(TOKEN_COMMA);
                break;
        case '.':
                addToken(TOKEN_DOT);
                break;
        case '-':
                addToken(TOKEN_MINUS);
                break;
        case '+':
                addToken(TOKEN_PLUS);
                break;
        case ';':
                addToken(TOKEN_SEMICOLON);
                break;
        case '*':
                addToken(TOKEN_STAR);
                break;
        case '=':
                addToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
                break;
        case '!':
                addToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
                break;
        case '>':
                addToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
                break;
        case '<':
                addToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
                break;
        case '/':
                if (!match('/')) {
                        addToken(TOKEN_SLASH);
                        break;
                }
                while (!isAtEnd() && peek() != '\n') {
                        advance();
                }
                break;
        default:
                error("Unexpected character: %c", c);
        }
}

PtrVector *scanTokens(const char *source) {
        init(source);

        while (!isAtEnd()) {
                scanToken();
                scanner.start = scanner.current;
        }
        addToken(TOKEN_EOF);

        return scanner.tokens;
}

bool hasLexicalError(void) {
        return scanner.hasError;
}
