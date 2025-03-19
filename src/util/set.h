#ifndef CODECRAFTERS_INTERPRETER_UTIL_SET_H
#define CODECRAFTERS_INTERPRETER_UTIL_SET_H

typedef struct Set Set;

Set *set_construct(int (*comparator)(const void *, const void *));
void set_insert(Set *set, void *element);
void *set_search(Set *set, const void *element);

#endif
