#ifndef CODECRAFTERS_INTERPRETER_LOX_SCANNER_H
#define CODECRAFTERS_INTERPRETER_LOX_SCANNER_H

#include <stdbool.h>

#include "util/vector.h"

Vector *scan_tokens(const char *source);
bool has_scan_error(void);

#endif
