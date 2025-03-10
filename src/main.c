#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "lox/ast_printer.h"
#include "lox/expr.h"
#include "lox/interpreter.h"
#include "lox/object.h"
#include "lox/parser.h"
#include "lox/scanner.h"
#include "lox/ptr_vector.h"
#include "lox/token.h"
#include "lox/xmalloc.h"

static char *
read_source(const char *path)
{
        FILE *file = fopen(path, "rb");
        if (file == NULL)
                err(EX_NOINPUT, "%s", path);

        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *source = xmalloc(file_size + 1);
        fread(source, sizeof(char), file_size, file);
        source[file_size] = '\0';

        fclose(file);

        return source;
}

static PtrVector *
tokenize(const char *source, bool print_tokens)
{
        PtrVector *tokens = scan_tokens(source);

        if (print_tokens)
        {
                size_t num_tokens = ptr_vector_size(tokens);
                for (size_t i = 0; i < num_tokens; i++)
                {
                        Token *token = ptr_vector_at(tokens, i);
                        printf("%s\n", token_stringify(token));
                }
        }

        if (has_lexical_error())
                exit(EX_DATAERR);

        return tokens;
}

static Expr *
parse_expr(const PtrVector *tokens, bool print_ast)
{
        Expr *expr = parse_to_expr(tokens);
        if (expr == NULL)
                exit(EX_DATAERR);

        if (print_ast)
                println_expr(expr);

        return expr;
}

static PtrVector *
parse_stmts(const PtrVector *tokens)
{
        PtrVector *stmts = parse_to_stmts(tokens);
        if (stmts == NULL)
                exit(EX_DATAERR);
        return stmts;
}

static void
evaluate(const Expr *expr)
{
        const Object *object = interpret_expr(expr);
        if (object == NULL)
                exit(EX_SOFTWARE);

        printf("%s\n", object_stringify(object, true));
}

static void
run(const PtrVector *stmts)
{
        if (interpret_stmts(stmts) < 0)
                exit(EX_SOFTWARE);
}

int
main(int argc, char *argv[])
{
        if (argc != 3)
        {
                fprintf(stderr, "usage: your_program.sh <command> <file>\n");
                exit(EX_USAGE);
        }

        const char *source = read_source(argv[2]);

        const char *command = argv[1];
        if (strcmp(command, "tokenize") == 0)
                tokenize(source, true);
        else if (strcmp(command, "parse") == 0)
                parse_expr(tokenize(source, false), true);
        else if (strcmp(command, "evaluate") == 0)
                evaluate(parse_expr(tokenize(source, false), false));
        else if (strcmp(command, "run") == 0)
                run(parse_stmts(tokenize(source, false)));
        else
                errx(EX_USAGE, "unknown command: %s", command);

        exit(EX_OK);
}
