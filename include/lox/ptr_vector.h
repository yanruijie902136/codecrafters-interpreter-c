#ifndef CODECRAFTERS_INTERPRETER_PTR_VECTOR_H
#define CODECRAFTERS_INTERPRETER_PTR_VECTOR_H

#include <stddef.h>

typedef struct PtrVector PtrVector;

PtrVector *createPtrVector(void);
size_t ptrVectorSize(const PtrVector *vector);
void *ptrVectorAt(const PtrVector *vector, size_t index);
void ptrVectorAppend(PtrVector *vector, const void *ptr);

#endif
