#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "lox/expr.h"
#include "lox/interpreter.h"
#include "lox/object.h"
#include "lox/parser.h"
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

static PtrVector *tokenize(const char *source, bool printTokens) {
        PtrVector *tokens = scanTokens(source);

        if (printTokens) {
                size_t numTokens = ptrVectorSize(tokens);
                for (size_t i = 0; i < numTokens; i++) {
                        Token *token = ptrVectorAt(tokens, i);
                        printf("%s\n", tokenToString(token));
                }
        }

        if (hasLexicalError()) {
                exit(EX_DATAERR);
        }

        return tokens;
}

static Expr *parse(const PtrVector *tokens, bool printExpr) {
        Expr *expr = parseToExpr(tokens);
        if (expr == NULL) {
                exit(EX_DATAERR);
        }

        if (printExpr) {
                printf("%s\n", exprToString(expr));
        }

        return expr;
}

static void evaluate(const Expr *expr) {
        const Object *object = interpretExpr(expr);
        printf("%s\n", objectToString(object, true));
}

int main(int argc, char *argv[]) {
        if (argc != 3) {
                fprintf(stderr, "usage: your_program.sh <command> <file>\n");
                exit(EX_USAGE);
        }

        const char *source = readSource(argv[2]);

        const char *command = argv[1];
        if (strcmp(command, "tokenize") == 0) {
                tokenize(source, true);
        }
        else if (strcmp(command, "parse") == 0) {
                parse(tokenize(source, false), true);
        }
        else if (strcmp(command, "evaluate") == 0) {
                evaluate(parse(tokenize(source, false), false));
        }
        else {
                errx(EX_USAGE, "unknown command: %s", command);
        }

        exit(EX_OK);
}
