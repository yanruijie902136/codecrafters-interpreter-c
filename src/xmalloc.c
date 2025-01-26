#include "lox/xmalloc.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

void *xmalloc(size_t size) {
        void *ptr = malloc(size);
        if (ptr == NULL) {
                err(EX_OSERR, "malloc");
        }
        return ptr;
}

void *xrealloc(void *ptr, size_t size) {
        ptr = realloc(ptr, size);
        if (ptr == NULL) {
                err(EX_OSERR, "realloc");
        }
        return ptr;
}

char *xstrndup(const char *str, size_t n) {
        char *str2 = strndup(str, n);
        if (str2 == NULL) {
                err(EX_OSERR, "strndup");
        }
        return str2;
}
