#ifndef CODECRAFTERS_INTERPRETER_XMALLOC_H
#define CODECRAFTERS_INTERPRETER_XMALLOC_H

#include <stddef.h>

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrndup(const char *str, size_t n);

#endif
