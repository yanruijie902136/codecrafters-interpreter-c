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

static void
init(const char *source)
{
        scanner.start = source;
        scanner.current = source;
        scanner.line = 1;
        scanner.tokens = ptr_vector_create();
        scanner.hasError = false;
}

static bool
is_at_end(void)
{
        return *scanner.current == '\0';
}

static char
advance(void)
{
        return *scanner.current++;
}

static char
peek(void)
{
        return *scanner.current;
}

static char
peek_next(void)
{
        return is_at_end() ? '\0' : *(scanner.current + 1);
}

static bool
match(char expected)
{
        if (peek() != expected)
                return false;
        advance();
        return true;
}

static char *
get_lexeme(void)
{
        size_t lexemeLength = scanner.current - scanner.start;
        return xstrndup(scanner.start, lexemeLength);
}

static void
add_token_full(TokenType type, const char *lexeme, const Object *literal)
{
        Token *token = token_create(type, lexeme, literal, scanner.line);
        ptr_vector_append(scanner.tokens, token);
}

static void
add_token(TokenType type)
{
        add_token_full(type, get_lexeme(), NULL);
}

static void
error(const char *format, ...)
{
        fprintf(stderr, "[line %zu] Error: ", scanner.line);
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        scanner.hasError = true;
}

static char *
remove_quotes(const char *quotedStr)
{
        return xstrndup(quotedStr + 1, strlen(quotedStr) - 2);
}

static void
scan_string(void)
{
        while (!is_at_end() && peek() != '\"')
        {
                if (peek() == '\n')
                        scanner.line++;
                advance();
        }

        if (is_at_end())
        {
                error("Unterminated string.");
                return;
        }
        advance();

        char *lexeme = get_lexeme();
        Object *literal = object_create_string(remove_quotes(lexeme));
        add_token_full(TOKEN_STRING, lexeme, literal);
}

static void
scan_number(void)
{
        while (isdigit(peek()))
                advance();

        if (peek() == '.' && isdigit(peek_next()))
        {
                advance();
                while (isdigit(peek()))
                        advance();
        }

        char *lexeme = get_lexeme();
        Object *literal = object_create_number(atof(lexeme));
        add_token_full(TOKEN_NUMBER, lexeme, literal);
}

static bool
is_alpha_numeric(char c)
{
        return c == '_' || isalnum(c);
}

static TokenType
identifier_or_keyword(const char *lexeme)
{
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

        for (size_t i = 0; i < NUM_KEYWORDS; i++)
                if (strcmp(lexeme, KEYWORDS[i].lexeme) == 0)
                        return KEYWORDS[i].type;
        return TOKEN_IDENTIFIER;
}

static void
scan_identifier(void)
{
        while (is_alpha_numeric(peek()))
                advance();

        char *lexeme = get_lexeme();
        add_token_full(identifier_or_keyword(lexeme), lexeme, NULL);
}

static void
scan_token(void)
{
        char c = advance();
        switch (c)
        {
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
                if (!match('/'))
                {
                        add_token(TOKEN_SLASH);
                        break;
                }
                while (!is_at_end() && peek() != '\n')
                        advance();
                break;
        case '\n':
                scanner.line++;
                break;
        case '\"':
                scan_string();
                break;
        default:
                if (isspace(c))
                        break;
                else if (isdigit(c))
                {
                        scan_number();
                        break;
                }
                else if (is_alpha_numeric(c))
                {
                        scan_identifier();
                        break;
                }
                error("Unexpected character: %c", c);
        }
}

PtrVector *
scan_tokens(const char *source)
{
        init(source);

        while (!is_at_end())
        {
                scan_token();
                scanner.start = scanner.current;
        }
        add_token(TOKEN_EOF);

        return scanner.tokens;
}

bool
has_lexical_error(void)
{
        return scanner.hasError;
}
