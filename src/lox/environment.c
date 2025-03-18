#include "lox/environment.h"
#include "lox/errors.h"
#include "lox/object.h"
#include "lox/token.h"
#include "util/xmalloc.h"

#include <string.h>

typedef struct Node Node;
struct Node {
        const char *name;
        Object *value;
        size_t level;
        Node *lch;
        Node *rch;
};

static Node *node_construct(const char *name, Object *value) {
        Node *node = xmalloc(sizeof(Node));
        node->name = name;
        node->value = value;
        node->level = 0;
        node->lch = NULL;
        node->rch = NULL;
        return node;
}

static void skew(Node **rootp) {
        Node *root = *rootp;
        if (root == NULL || root->lch == NULL || root->level != root->lch->level) {
                return;
        }
        Node *lch = root->lch;
        root->lch = lch->rch;
        lch->rch = root;
        *rootp = lch;
}

static void split(Node **rootp) {
        Node *root = *rootp;
        if (root == NULL || root->rch == NULL || root->rch->rch == NULL || root->level != root->rch->rch->level) {
                return;
        }
        Node *rch = root->rch;
        root->rch = rch->lch;
        rch->lch = root;
        rch->level++;
        *rootp = rch;
}

static void insert(Node **rootp, const char *name, Object *value) {
        Node *root = *rootp;
        if (root == NULL) {
                *rootp = node_construct(name, value);
                return;
        }

        int c = strcmp(name, root->name);
        if (c < 0) {
                insert(&root->lch, name, value);
        } else if (c > 0) {
                insert(&root->rch, name, value);
        } else {
                root->value = value;
        }

        skew(&root);
        split(&root);
        *rootp = root;
}

static Node *search(Node *root, const char *name) {
        if (root == NULL) {
                return NULL;
        }

        int c = strcmp(name, root->name);
        if (c < 0) {
                return search(root->lch, name);
        } else if (c > 0) {
                return search(root->rch, name);
        } else {
                return root;
        }
}

struct Environment {
        Node *root;
};

Environment *environment_construct(void) {
        Environment *environment = xmalloc(sizeof(Environment));
        environment->root = NULL;
        return environment;
}

Object *environment_get(const Environment *environment, const Token *name) {
        Node *node = search(environment->root, name->lexeme);
        if (node == NULL) {
                interpret_error(name, "Undefined variable '%s'.", name->lexeme);
        }
        return node->value;
}

void environment_define(Environment *environment, const char *name, Object *value) {
        insert(&environment->root, name, value);
}

void environment_assign(Environment *environment, const Token *name, Object *value) {
        Node *node = search(environment->root, name->lexeme);
        if (node == NULL) {
                interpret_error(name, "Undefined variable '%s'.", name->lexeme);
        }
        node->value = value;
}
