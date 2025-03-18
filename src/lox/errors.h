#ifndef CODECRAFTERS_INTERPRETER_LOX_ERRORS_H
#define CODECRAFTERS_INTERPRETER_LOX_ERRORS_H

#include <stddef.h>

#include "lox/token.h"

void scan_error(size_t line, const char *format, ...);

__attribute__((noreturn))
void parse_error(const Token *token, const char *format, ...);

__attribute__((noreturn))
void interpret_error(const Token *token, const char *format, ...);

#endif
