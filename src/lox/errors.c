#include "lox/errors.h"
#include "lox/token.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void scan_error(size_t line, const char *format, ...) {
        fprintf(stderr, "[line %zu] Error: ", line);
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
}

__attribute__((noreturn))
void parse_error(const Token *token, const char *format, ...) {
        fprintf(stderr, "[line %zu] Error at ", token->line);
        if (token->type == TOKEN_EOF) {
                fprintf(stderr, "end: ");
        } else {
                fprintf(stderr, "'%s': ", token->lexeme);
        }
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
        exit(65);
}

__attribute__((noreturn))
void interpret_error(const Token *token, const char *format, ...) {
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n[line %zu]\n", token->line);
        exit(70);
}
