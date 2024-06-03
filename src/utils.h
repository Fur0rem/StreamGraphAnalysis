#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <math.h>
#include <stdlib.h> 

// Text formatting
#define TEXT_BOLD "\033[1m"
#define TEXT_RED "\033[31m"
#define TEXT_GREEN "\033[32m"
#define TEXT_RESET "\033[0m"

// Malloc with error handling
#define MALLOC_CHECK(size) ({ \
    void *ptr = (void*)malloc(size); \
    if (ptr == NULL) { \
        fprintf(stderr, "Memory allocation failed\n"); \
        exit(1); \
    } \
    ptr; \
})

// Floating point comparison
#define F_EQUALS(a, b) (fabs((a) - (b)) < 0.0001)

// Dynamic array (vectors)
#define DefVector(type) \
\
typedef struct T_##type##Vector { \
    type *array; \
    int size, capacity; \
} type##Vector; \
\
static type##Vector *type##VectorCreate(int capacity) { \
    type##Vector *s = (type##Vector*)MALLOC_CHECK(sizeof(type##Vector)); \
    s->array = (type*)MALLOC_CHECK(sizeof(type) * capacity); \
    s->size=0; \
    s->capacity = capacity; \
    return s; \
} \
\
static void type##VectorFree(type##Vector *s) { \
    free(s->array); free(s); \
} \
\
static void type##VectorPush(type##Vector *s, type value) { \
    if (s->size == s->capacity) { \
        s->capacity *= 2; \
        s->array = (type*)realloc(s->array, sizeof(type) * s->capacity); \
    } \
    s->array[s->size++] = value; \
} \

#endif