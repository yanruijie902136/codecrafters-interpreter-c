#ifndef CODECRAFTERS_INTERPRETER_UTIL_XMALLOC_H
#define CODECRAFTERS_INTERPRETER_UTIL_XMALLOC_H

#include <stddef.h>

void *xmalloc(size_t size);
void *xrealloc(void *pointer, size_t size);
char *xstrdup(const char *string);
char *xstrndup(const char *string, size_t num_chars);

#endif
