#include "util/vector.h"
#include "util/xmalloc.h"

#include <assert.h>
#include <stdlib.h>

struct Vector {
        void **elements;
        size_t capacity;
        size_t size;
};

Vector *vector_construct(void) {
        Vector *vector = xmalloc(sizeof(Vector));
        const size_t initial_capacity = 10;
        vector->elements = xmalloc(sizeof(void *) * initial_capacity);
        vector->capacity = initial_capacity;
        vector->size = 0;
        return vector;
}

void vector_destruct(Vector *vector, void (*element_destruct)(void *)) {
        for (size_t i = 0; i < vector->size; i++) {
                void *element = vector->elements[i];
                element_destruct(element);
        }
        free(vector->elements);
        free(vector);
}

size_t vector_size(const Vector *vector) {
        return vector->size;
}

const void *vector_get_const(const Vector *vector, size_t index) {
        assert(0 <= index && index < vector->size);
        return vector->elements[index];
}

void vector_push_back(Vector *vector, void *element) {
        if (vector->size == vector->capacity) {
                size_t new_capacity = vector->capacity * 2;
                vector->elements = xrealloc(vector->elements, sizeof(void *) * new_capacity);
                vector->capacity = new_capacity;
        }
        vector->elements[vector->size++] = element;
}
