#include "lox/scanner.h"
#include "lox/token.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

static struct {
        const char *start;
        const char *current;
        Vector *tokens;
        bool has_error;
} scanner;

static void init(const char *source) {
        scanner.start = source;
        scanner.current = source;
        scanner.tokens = vector_construct();
        scanner.has_error = false;
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

static bool match(char c) {
        if (peek() != c) {
                return false;
        }
        advance();
        return true;
}

static char *get_lexeme(void) {
        size_t lexeme_length = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexeme_length);
}

static void add_token(TokenType type) {
        Token *token = token_construct(type, get_lexeme());
        vector_push_back(scanner.tokens, token);
}

static void error(const char *format, ...) {
        fprintf(stderr, "[line 1] Error: ");
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        scanner.has_error = true;
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
        case '{':
                add_token(TOKEN_LEFT_BRACE);
                break;
        case '}':
                add_token(TOKEN_RIGHT_BRACE);
                break;
        case ',':
                add_token(TOKEN_COMMA);
                break;
        case '.':
                add_token(TOKEN_DOT);
                break;
        case '-':
                add_token(TOKEN_MINUS);
                break;
        case '+':
                add_token(TOKEN_PLUS);
                break;
        case ';':
                add_token(TOKEN_SEMICOLON);
                break;
        case '*':
                add_token(TOKEN_STAR);
                break;
        case '=':
                add_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
                break;
        case '!':
                add_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
                break;
        case '>':
                add_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
                break;
        case '<':
                add_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
                break;
        default:
                error("Unexpected character: %c", c);
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

bool has_scan_error(void) {
        return scanner.has_error;
}
