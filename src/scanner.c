#include "lox/scanner.h"
#include "lox/object.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
        const char *start;
        const char *current;
        size_t line;
        PtrVector *tokens;
        bool hasError;
} Scanner;

static Scanner scanner;

static void init(const char *source) {
        scanner.start = source;
        scanner.current = source;
        scanner.line = 1;
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

static char peekNext(void) {
        return isAtEnd() ? '\0' : *(scanner.current + 1);
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

static void addTokenFull(TokenType type, const char *lexeme, const Object *literal) {
        Token *token = createToken(type, lexeme, literal);
        ptrVectorAppend(scanner.tokens, token);
}

static void addToken(TokenType type) {
        addTokenFull(type, getLexeme(), NULL);
}

static void error(const char *format, ...) {
        fprintf(stderr, "[line %zu] Error: ", scanner.line);
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        scanner.hasError = true;
}

static char *removeQuotes(const char *quotedStr) {
        return xstrndup(quotedStr + 1, strlen(quotedStr) - 2);
}

static void scanString(void) {
        while (!isAtEnd() && peek() != '\"') {
                if (peek() == '\n') {
                        scanner.line++;
                }
                advance();
        }

        if (isAtEnd()) {
                error("Unterminated string.");
                return;
        }
        advance();

        char *lexeme = getLexeme();
        Object *literal = createStringObject(removeQuotes(lexeme));
        addTokenFull(TOKEN_STRING, lexeme, literal);
}

static void scanNumber(void) {
        while (isdigit(peek())) {
                advance();
        }

        if (peek() == '.' && isdigit(peekNext())) {
                advance();
                while (isdigit(peek())) {
                        advance();
                }
        }

        char *lexeme = getLexeme();
        Object *literal = createNumberObject(atof(lexeme));
        addTokenFull(TOKEN_NUMBER, lexeme, literal);
}

static bool isAlphaNumeric(char c) {
        return c == '_' || isalnum(c);
}

static TokenType identifierOrKeyword(const char *lexeme) {
        typedef struct {
                const char *lexeme;
                TokenType type;
        } Keyword;

        static const Keyword KEYWORDS[] = {
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
        static const size_t NUM_KEYWORDS = sizeof(KEYWORDS) / sizeof(Keyword);

        for (size_t i = 0; i < NUM_KEYWORDS; i++) {
                if (strcmp(lexeme, KEYWORDS[i].lexeme) == 0) {
                        return KEYWORDS[i].type;
                }
        }
        return TOKEN_IDENTIFIER;
}

static void scanIdentifier(void) {
        while (isAlphaNumeric(peek())) {
                advance();
        }

        char *lexeme = getLexeme();
        addTokenFull(identifierOrKeyword(lexeme), lexeme, NULL);
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
        case '\n':
                scanner.line++;
                break;
        case '\"':
                scanString();
                break;
        default:
                if (isspace(c)) {
                        break;
                }
                else if (isdigit(c)) {
                        scanNumber();
                        break;
                }
                else if (isAlphaNumeric(c)) {
                        scanIdentifier();
                        break;
                }
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
