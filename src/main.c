#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lox/scanner.h"
#include "lox/token.h"
#include "util/vector.h"
#include "util/xmalloc.h"

static char *read_source(const char *path) {
        FILE *file = fopen(path, "r");
        if (file == NULL) {
                err(EXIT_FAILURE, "%s", path);
        }

        fseek(file, 0, SEEK_END);
        size_t num_chars = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *source = xmalloc(num_chars + 1);
        fread(source, 1, num_chars, file);
        source[num_chars] = '\0';

        fclose(file);
        return source;
}

static void tokenize(const char *source) {
        Vector *tokens = scan_tokens(source);

        size_t num_tokens = vector_size(tokens);
        for (size_t i = 0; i < num_tokens; i++) {
                const Token *token = vector_get_const(tokens, i);
                printf("%s\n", token_to_string(token));
        }

        if (has_scan_error()) {
                exit(65);
        }
}

int main(int argc, char *argv[]) {
        if (argc != 3) {
                fprintf(stderr, "usage: %s command file\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        char *source = read_source(argv[2]);

        const char *command = argv[1];
        if (strcmp(command, "tokenize") == 0) {
                tokenize(source);
        } else {
                errx(EXIT_FAILURE, "unknown command: %s", command);
        }

        exit(EXIT_SUCCESS);
}
