#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/set.h"
#include "util/xmalloc.h"

#include <string.h>

typedef struct {
        const char *name;
        Object *value;
} Element;

static int element_compare(const void *element1, const void *element2) {
        const char *name1 = ((const Element *)element1)->name;
        const char *name2 = ((const Element *)element2)->name;
        return strcmp(name1, name2);
}

static Element *element_construct(const char *name, Object *value) {
        Element *element = xmalloc(sizeof(Element));
        element->name = name;
        element->value = value;
        return element;
}

struct Environment {
        Set *values;
        Environment *enclosing;
};

Environment *environment_construct(Environment *enclosing) {
        Environment *environment = xmalloc(sizeof(Environment));
        environment->values = set_construct(element_compare);
        environment->enclosing = enclosing;
        return environment;
}

Object *environment_get(const Environment *environment, const Token *name) {
        Element element = {
                .name = name->lexeme,
        };
        Element *p = set_search(environment->values, &element);
        if (p != NULL) {
                return p->value;
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

Object *environment_get_at(const Environment *environment, const Token *name, size_t depth) {
        return environment_get(ancestor(environment, depth), name);
}

void environment_define(Environment *environment, const char *name, Object *value) {
        set_insert(environment->values, element_construct(name, value));
}

void environment_assign(Environment *environment, const Token *name, Object *value) {
        Element element = {
                .name = name->lexeme,
        };
        Element *p = set_search(environment->values, &element);
        if (p != NULL) {
                p->value = value;
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
