#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/map.h"
#include "util/xmalloc.h"

#include <string.h>

Environment *environment_construct(Environment *enclosing) {
        Environment *environment = xmalloc(sizeof(Environment));
        environment->values = map_construct(str_compare);
        environment->enclosing = enclosing;
        return environment;
}

Object *environment_get(const Environment *environment, const Token *name) {
        if (map_contains(environment->values, name->lexeme)) {
                return map_get(environment->values, name->lexeme);
        }
        if (environment->enclosing != NULL) {
                return environment_get(environment->enclosing, name);
        }
        interpret_error(name, "Undefined variable '%s'.", name->lexeme);
}

static Environment *ancestor(const Environment *environment, size_t depth) {
        Environment *p = (Environment *)environment;
        for (size_t i = 0; i < depth; i++) {
                p = p->enclosing;
        }
        return p;
}

Object *environment_get_at(const Environment *environment, const char *name, size_t depth) {
        Environment *env = ancestor(environment, depth);
        return map_get(env->values, name);
}

void environment_define(Environment *environment, const char *name, Object *value) {
        map_put(environment->values, name, value);
}

void environment_assign(Environment *environment, const Token *name, Object *value) {
        if (map_contains(environment->values, name->lexeme)) {
                map_put(environment->values, name->lexeme, value);
                return;
        }
        if (environment->enclosing != NULL) {
                environment_assign(environment->enclosing, name, value);
                return;
        }
        interpret_error(name, "Undefined variable '%s'.", name->lexeme);
}

void environment_assign_at(Environment *environment, const Token *name, Object *value, size_t depth) {
        environment_assign(ancestor(environment, depth), name, value);
}
