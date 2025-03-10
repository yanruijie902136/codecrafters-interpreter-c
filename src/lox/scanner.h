#ifndef CODECRAFTERS_INTERPRETER_SCANNER_H
#define CODECRAFTERS_INTERPRETER_SCANNER_H

#include "lox/ptr_vector.h"

PtrVector *scan_tokens(const char *source);
bool has_lexical_error(void);

#endif
