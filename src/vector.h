#ifndef VECTOR_H
#define VECTOR_H

#include "utils.h"
#include <stdio.h>

// Dynamic array (vectors)
#define DefVector(type) \
\
typedef struct T_##type##Vector { \
    type *array; \
    int size, capacity; \
} type##Vector; \
\
static type##Vector type##Vector_with_capacity(int capacity) { \
    type##Vector s = { \
        .array = (type*)malloc(sizeof(type) * capacity), \
        .size = 0, \
        .capacity = capacity \
    }; \
    return s; \
} \
\
static void type##Vector_destroy(type##Vector s) { \
    free(s.array); \
} \
\
static void type##Vector_push(type##Vector s, type value) { \
    if (s.size == s.capacity) { \
        s.capacity *= 2; \
        s.array = (type*)realloc(s.array, sizeof(type) * s.capacity); \
    } \
    s.array[s.size++] = value; \
} \

#endif // VECTOR_H