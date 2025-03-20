#include "util/map.h"
#include "util/xmalloc.h"

#include <assert.h>

typedef struct Node Node;
struct Node {
        const void *key;
        void *value;
        size_t level;
        Node *lch;
        Node *rch;
};

static Node *node_construct(const void *key, void *value) {
        Node *node = xmalloc(sizeof(Node));
        node->key = key;
        node->value = value;
        node->level = 1;
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

static void insert(Node **rootp, const void *key, void *value, Comparator comparator) {
        Node *root = *rootp;
        if (root == NULL) {
                *rootp = node_construct(key, value);
                return;
        }

        int c = comparator(key, root->key);
        if (c < 0) {
                insert(&root->lch, key, value, comparator);
        } else if (c > 0) {
                insert(&root->rch, key, value, comparator);
        } else {
                root->value = value;
        }

        skew(&root);
        split(&root);
        *rootp = root;
}

static Node *search(Node *root, const void *key, Comparator comparator) {
        if (root == NULL) {
                return NULL;
        }

        int c = comparator(key, root->key);
        if (c == 0) {
                return root;
        }
        return search(c < 0 ? root->lch : root->rch, key, comparator);
}

struct Map {
        Node *root;
        Comparator comparator;
};

Map *map_construct(Comparator comparator) {
        Map *map = xmalloc(sizeof(Map));
        map->root = NULL;
        map->comparator = comparator;
        return map;
}

void map_put(Map *map, const void *key, void *value) {
        insert(&map->root, key, value, map->comparator);
}

bool map_contains(const Map *map, const void *key) {
        return search(map->root, key, map->comparator) != NULL;
}

void *map_get(Map *map, const void *key) {
        Node *node = search(map->root, key, map->comparator);
        assert(node != NULL);
        return node->value;
}
