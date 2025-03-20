#ifndef CODECRAFTERS_INTERPRETER_UTIL_MAP_H
#define CODECRAFTERS_INTERPRETER_UTIL_MAP_H

#include <string.h>

typedef struct Map Map;
typedef int (*Comparator)(const void *, const void *);

Map *map_construct(Comparator comparator);
void map_put(Map *map, const void *key, void *value);
bool map_contains(const Map *map, const void *key);
void *map_get(Map *map, const void *key);

static inline int str_compare(const void *str1, const void *str2) {
        return strcmp(str1, str2);
}

static inline int ptr_compare(const void *ptr1, const void *ptr2) {
        if (ptr1 == ptr2) {
                return 0;
        }
        return ptr1 < ptr2 ? -1 : 1;
}

#endif
