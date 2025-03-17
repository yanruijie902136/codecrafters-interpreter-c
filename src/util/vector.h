#ifndef CODECRAFTERS_INTERPRETER_LOX_VECTOR_H
#define CODECRAFTERS_INTERPRETER_LOX_VECTOR_H

#include <stddef.h>

typedef struct Vector Vector;

Vector *vector_construct(void);
size_t vector_size(const Vector *vector);
void *vector_at(const Vector *vector, size_t index);
void vector_push_back(Vector *vector, void *element);

#endif
