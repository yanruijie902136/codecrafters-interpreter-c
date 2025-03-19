#include "util/set.h"
#include "util/xmalloc.h"

typedef struct Node Node;
struct Node {
        void *element;
        size_t level;
        Node *lch;
        Node *rch;
};

static Node *node_construct(void *element) {
        Node *node = xmalloc(sizeof(Node));
        node->element = element;
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

static void insert(Node **rootp, void *element, int (*comparator)(const void *, const void *)) {
        Node *root = *rootp;
        if (root == NULL) {
                *rootp = node_construct(element);
                return;
        }

        int c = comparator(element, root->element);
        if (c < 0) {
                insert(&root->lch, element, comparator);
        } else if (c > 0) {
                insert(&root->rch, element, comparator);
        } else {
                root->element = element;
        }

        skew(&root);
        split(&root);
        *rootp = root;
}

static Node *search(Node *root, const void *element, int (*comparator)(const void *, const void *)) {
        if (root == NULL) {
                return NULL;
        }

        int c = comparator(element, root->element);
        if (c == 0) {
                return root;
        }
        return c < 0 ? search(root->lch, element, comparator) : search(root->rch, element, comparator);
}

struct Set {
        Node *root;
        int (*comparator)(const void *, const void *);
};

Set *set_construct(int (*comparator)(const void *, const void *)) {
        Set *set = xmalloc(sizeof(Set));
        set->root = NULL;
        set->comparator = comparator;
        return set;
}

void set_insert(Set *set, void *element) {
        insert(&set->root, element, set->comparator);
}

void *set_search(Set *set, const void *element) {
        Node *node = search(set->root, element, set->comparator);
        return node == NULL ? NULL : node->element;
}
