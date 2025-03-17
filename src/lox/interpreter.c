#include "lox/interpreter.h"
#include "lox/expr.h"
#include "lox/object.h"

#include <stdio.h>
#include <string.h>

static const char *stringify(const Object *object) {
        static char str[256];
        snprintf(str, sizeof(str), "%s", object_to_string(object));

        if (object_is_number(object)) {
                size_t n = strlen(str);
                char *p = str + n - 2;
                if (n >= 2 && strcmp(p, ".0") == 0) {
                        *p = '\0';
                }
        }

        return str;
}

static Object *evaluate_expr(const Expr *expr);

static Object *evaluate_literal_expr(const LiteralExpr *literal_expr) {
        return literal_expr->value;
}

static Object *evaluate_expr(const Expr *expr) {
        switch (expr->type) {
        case EXPR_LITERAL:
                return evaluate_literal_expr((const LiteralExpr *)expr);
        }
}

void interpret_expr(const Expr *expr) {
        printf("%s\n", stringify(evaluate_expr(expr)));
}
