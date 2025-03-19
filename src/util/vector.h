#ifndef CODECRAFTERS_INTERPRETER_UTIL_VECTOR_H
#define CODECRAFTERS_INTERPRETER_UTIL_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Vector Vector;

Vector *vector_construct(void);
size_t vector_size(const Vector *vector);
bool vector_is_empty(const Vector *vector);
void *vector_at(const Vector *vector, size_t index);
void *vector_at_back(const Vector *vector);
void vector_push_back(Vector *vector, void *element);
void vector_pop_back(Vector *vector);

#endif
