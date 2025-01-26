#include "lox/ptr_vector.h"
#include "lox/xmalloc.h"

struct PtrVector {
        void *mem;
        size_t capacity;
        size_t size;
};

static const size_t PTRSIZE = sizeof(void *);

PtrVector *
ptr_vector_create(void)
{
        PtrVector *vector = xmalloc(sizeof(PtrVector));
        vector->capacity = 10;
        vector->mem = xmalloc(PTRSIZE * vector->capacity);
        vector->size = 0;
        return vector;
}

size_t
ptr_vector_size(const PtrVector *vector)
{
        return vector->size;
}

void *
ptr_vector_at(const PtrVector *vector, size_t index)
{
        return *(void **)(vector->mem + PTRSIZE * index);
}

void
ptr_vector_append(PtrVector *vector, const void *ptr)
{
        if (vector->capacity == vector->size)
        {
                vector->capacity *= 2;
                vector->mem = xrealloc(vector->mem, PTRSIZE * vector->capacity);
        }
        const void **pos = vector->mem + PTRSIZE * vector->size;
        *pos = ptr;
        vector->size++;
}
