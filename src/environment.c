#include "lox/environment.h"
#include "lox/object.h"
#include "lox/xmalloc.h"

#include <err.h>
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

typedef struct {
        const char *name;
        const Object *object;
} Node;

static Node *
node_create(const char *name, const Object *object)
{
        Node *node = xmalloc(sizeof(Node));
        node->name = name;
        node->object = object;
        return node;
}

static void
node_destroy(Node *node)
{
        free((void *)node->object);
        free(node);
}

static int
node_compare(const void *v1, const void *v2)
{
        const Node *node1 = v1, *node2 = v2;
        return strcmp(node1->name, node2->name);
}

struct Environment {
        Environment *enclosing;
        void *root;
};

Environment *
environment_create(Environment *enclosing)
{
        Environment *environment = xmalloc(sizeof(Environment));
        environment->enclosing = enclosing;
        environment->root = NULL;
        return environment;
}

void
environment_destroy(Environment *environment)
{
        while (environment->root != NULL)
        {
                Node *node = *(Node **)environment->root;
                tdelete(node, &environment->root, node_compare);
                node_destroy(node);
        }
        free(environment);
}

void
environment_define(Environment *environment, const char *name, const Object *object)
{
        Node *new_node = node_create(name, object);
        Node **res = tsearch(new_node, &environment->root, node_compare);
        if (res == NULL)
                err(EX_OSERR, "tsearch");
        if (*res != new_node)
        {
                node_destroy(*res);
                *res = new_node;
        }
}

int
environment_assign(Environment *environment, const char *name, const Object *object)
{
        Node *target_node = node_create(name, NULL);
        Node **res = tfind(target_node, &environment->root, node_compare);
        node_destroy(target_node);

        if (res != NULL)
        {
                object_destroy((Object *)(*res)->object);
                (*res)->object = object;
                return 0;

        }

        if (environment->enclosing != NULL)
                return environment_assign(environment->enclosing, name, object);

        return -1;
}

Object *
environment_get(const Environment *environment, const char *name)
{
        Node *target_node = node_create(name, NULL);
        Node **res = tfind(target_node, &environment->root, node_compare);
        node_destroy(target_node);

        if (res != NULL)
                return object_copy((*res)->object);

        if (environment->enclosing != NULL)
                return environment_get(environment->enclosing, name);

        return NULL;
}
