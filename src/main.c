#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "lox/scanner.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

static char *readSource(const char *path) {
        FILE *file = fopen(path, "rb");
        if (file == NULL) {
                err(EX_NOINPUT, "%s", path);
        }

        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *source = xmalloc(fileSize + 1);
        fread(source, sizeof(char), fileSize, file);
        source[fileSize] = '\0';

        fclose(file);

        return source;
}

static void tokenize(const char *source) {
        PtrVector *tokens = scanTokens(source);

        size_t numTokens = ptrVectorSize(tokens);
        for (size_t i = 0; i < numTokens; i++) {
                Token *token = ptrVectorAt(tokens, i);
                printf("%s\n", tokenToString(token));
        }

        if (hasLexicalError()) {
                exit(EX_DATAERR);
        }
}

int main(int argc, char *argv[]) {
        if (argc != 3) {
                fprintf(stderr, "usage: your_program.sh <command> <file>\n");
                exit(EX_USAGE);
        }

        const char *source = readSource(argv[2]);

        const char *command = argv[1];
        if (strcmp(command, "tokenize") == 0) {
                tokenize(source);
        }
        else {
                errx(EX_USAGE, "unknown command: %s", command);
        }

        exit(EX_OK);
}
