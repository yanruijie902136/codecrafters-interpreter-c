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
        void *root;
};

Environment *
environment_create(void)
{
        Environment *environment = xmalloc(sizeof(Environment));
        environment->root = NULL;
        return environment;
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

Object *
environment_get(const Environment *environment, const char *name)
{
        Node *target_node = node_create(name, NULL);
        Node **res = tfind(target_node, &environment->root, node_compare);
        node_destroy(target_node);
        return res == NULL ? NULL : object_copy((*res)->object);
}
