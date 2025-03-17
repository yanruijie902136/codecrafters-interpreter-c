#include "util/xmalloc.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t size) {
        void *pointer = malloc(size);
        if (pointer == NULL) {
                err(EXIT_FAILURE, "malloc");
        }
        return pointer;
}

void *xrealloc(void *pointer, size_t size) {
        pointer = realloc(pointer, size);
        if (pointer == NULL) {
                err(EXIT_FAILURE, "realloc");
        }
        return pointer;
}

char *xstrdup(const char *string) {
        char *s = strdup(string);
        if (s == NULL) {
                err(EXIT_FAILURE, "strdup");
        }
        return s;
}

char *xstrndup(const char *string, size_t num_chars) {
        char *s = strndup(string, num_chars);
        if (s == NULL) {
                err(EXIT_FAILURE, "strndup");
        }
        return s;
}
