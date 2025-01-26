#ifndef CODECRAFTERS_INTERPRETER_PTR_VECTOR_H
#define CODECRAFTERS_INTERPRETER_PTR_VECTOR_H

#include <stddef.h>

typedef struct PtrVector PtrVector;

PtrVector *ptr_vector_create(void);
size_t ptr_vector_size(const PtrVector *vector);
void *ptr_vector_at(const PtrVector *vector, size_t index);
void ptr_vector_append(PtrVector *vector, const void *ptr);

#endif
