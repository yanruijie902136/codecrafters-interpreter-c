#include "lox/scanner.h"
#include "lox/errors.h"
#include "lox/token.h"
#include "util/vector.h"
#include "util/xmalloc.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
        const char *start;
        const char *current;
        Vector *tokens;
        bool has_error;
        size_t line;
} scanner;

static void init(const char *source) {
        scanner.start = source;
        scanner.current = source;
        scanner.tokens = vector_construct();
        scanner.has_error = false;
        scanner.line = 1;
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

static char peek_next(void) {
        return is_at_end() ? '\0' : *(scanner.current + 1);
}

static char *get_lexeme(void) {
        size_t lexeme_length = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexeme_length);
}

static void add_token_complete(TokenType type, char *lexeme, Object *literal) {
        Token *token = token_construct(type, lexeme, literal, scanner.line);
        vector_push_back(scanner.tokens, token);
}

static void add_token(TokenType type) {
        add_token_complete(type, get_lexeme(), NULL);
}

static void comment(void) {
        while (!is_at_end() && peek() != '\n') {
                advance();
        }
}

static void string(void) {
        while (!is_at_end() && peek() != '\"') {
                if (advance() == '\n') {
                        scanner.line++;
                }
        }

        if (is_at_end()) {
                scan_error(scanner.line, "Unterminated string.");
                scanner.has_error = true;
                return;
        }
        advance();

        char *lexeme = get_lexeme();
        char *unquoted_lexeme = xstrndup(lexeme + 1, strlen(lexeme) - 2);
        Object *literal = string_object_construct(unquoted_lexeme);
        add_token_complete(TOKEN_STRING, lexeme, literal);
}

static void number(void) {
        while (isdigit(peek())) {
                advance();
        }

        if (peek() == '.' && isdigit(peek_next())) {
                advance();
                while (isdigit(peek())) {
                        advance();
                }
        }

        char *lexeme = get_lexeme();
        Object *literal = number_object_construct(atof(lexeme));
        add_token_complete(TOKEN_NUMBER, lexeme, literal);
}

static bool is_alpha_numeric(char c) {
        return c == '_' || isalnum(c);
}

static TokenType identifier_or_keyword(const char *lexeme) {
        typedef struct {
                const char *lexeme;
                TokenType type;
        } Keyword;

        static const Keyword keywords[] = {
                {"and", TOKEN_AND},
                {"class", TOKEN_CLASS},
                {"else", TOKEN_ELSE},
                {"false", TOKEN_FALSE},
                {"for", TOKEN_FOR},
                {"fun", TOKEN_FUN},
                {"if", TOKEN_IF},
                {"nil", TOKEN_NIL},
                {"or", TOKEN_OR},
                {"print", TOKEN_PRINT},
                {"return", TOKEN_RETURN},
                {"super", TOKEN_SUPER},
                {"this", TOKEN_THIS},
                {"true", TOKEN_TRUE},
                {"var", TOKEN_VAR},
                {"while", TOKEN_WHILE},
        };
        const size_t num_keywords = sizeof(keywords) / sizeof(Keyword);

        for (size_t i = 0; i < num_keywords; i++) {
                if (strcmp(lexeme, keywords[i].lexeme) == 0) {
                        return keywords[i].type;
                }
        }
        return TOKEN_IDENTIFIER;
}

static void identifier(void) {
        while (is_alpha_numeric(peek())) {
                advance();
        }

        char *lexeme = get_lexeme();
        add_token_complete(identifier_or_keyword(lexeme), lexeme, NULL);
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
        case '/':
                if (match('/')) {
                        comment();
                } else {
                        add_token(TOKEN_SLASH);
                }
                break;
        case '\n':
                scanner.line++;
                break;
        case '\"':
                string();
                break;
        default:
                if (isspace(c)) {
                        break;
                } else if (isdigit(c)) {
                        number();
                } else if (is_alpha_numeric(c)) {
                        identifier();
                } else {
                        scan_error(scanner.line, "Unexpected character: %c", c);
                        scanner.has_error = true;
                }
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
