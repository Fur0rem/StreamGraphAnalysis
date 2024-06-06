#ifndef VECTOR_H
#define VECTOR_H

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dynamic array (vectors)
#define DefVector(type, freefunc) \
\
typedef struct T_##type##Vector { \
    type *array; \
    size_t size; \
    size_t capacity; \
} type##Vector; \
\
static type##Vector type##Vector_with_capacity(size_t capacity) { \
    type##Vector vec = { \
        .array = (type*)malloc(sizeof(type) * capacity), \
        .size = 0, \
        .capacity = capacity \
    }; \
    return vec; \
} \
\
static type##Vector type##Vector_new() { \
    return type##Vector_with_capacity(2); \
} \
\
static void type##Vector_destroy(type##Vector vec) { \
    if (freefunc) { \
        for (size_t i = 0; i < vec.size; i++) { \
            freefunc(vec.array[i]); \
        } \
    } \
    free(vec.array); \
} \
\
static void type##Vector_push(type##Vector* vec, type value) { \
    if (vec->size == vec->capacity) { \
        vec->capacity *= 2; \
        type* new_array = (type*)malloc(sizeof(type) * vec->capacity); \
        memcpy(new_array, vec->array, sizeof(type) * vec->size); \
        free(vec->array); \
        vec->array = new_array; \
    } \
    vec->array[vec->size] = value; \
    vec->size++; \
} \
\
static void type##Vector_remove_and_swap(type##Vector* vec, size_t idx) { \
    if (freefunc) { \
        freefunc(vec->array[idx]); \
    } \
    vec->array[idx] = vec->array[vec->size - 1]; \
    vec->size--; \
} \

#endif // VECTOR_H