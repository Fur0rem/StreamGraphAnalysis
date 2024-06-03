#ifndef HASHSET_H
#define HASHSET_H

#include "utils.h"
#include <stdio.h>
#include "vector.h"


// TODO : changer à nb cases
// Hashset
#define DefHashset(type, hashfunc, eqfunc, print_format) \
\
DefVector(type) \
\
typedef struct T_##type##Hashset { \
    type##Vector* buckets; \
    int size; \
    int capacity; \
} type##Hashset; \
\
static type##Hashset type##Hashset_with_capacity(int capacity) { \
    type##Hashset s = { \
        .buckets = (type##Vector*)malloc(sizeof(type##Vector) * capacity), \
        .size = 0, \
        .capacity = capacity \
    }; \
    for (int i = 0; i < capacity; i++) { \
        s.buckets[i] = type##Vector_with_capacity(1); \
    } \
    return s; \
} \
\
static void type##Hashset_destroy(type##Hashset s) { \
    for (int i = 0; i < s.capacity; i++) { \
        type##Vector_destroy(s.buckets[i]); \
    } \
    free(s.buckets); \
} \
\
static bool type##Hashset_insert(type##Hashset* s, type value) { \
    int index = hashfunc(value) % s->capacity; \
    for (int i = 0; i < s->buckets[index].size; i++) { \
        if (eqfunc(s->buckets[index].array[i], value)) { \
            return false; \
        } \
    } \
    type##Vector_push(&s->buckets[index], value); \
    s->buckets[index].size++; \
    return true; \
} \
\
static bool type##Hashset_contains(type##Hashset s, type value) { \
    int index = hashfunc(value) % s.capacity; \
    for (int i = 0; i < s.buckets[index].size; i++) { \
        if (eqfunc(s.buckets[index].array[i], value)) { \
            return true; \
        } \
    } \
    return false; \
} \
\
static type* type##Hashset_find(type##Hashset s, type value) { \
    int index = hashfunc(value) % s.capacity; \
    for (int i = 0; i < s.buckets[index].size; i++) { \
        if (eqfunc(s.buckets[index].array[i], value)) { \
            return &s.buckets[index].array[i]; \
        } \
    } \
    return NULL; \
} \
\
static bool type##Hashset_remove(type##Hashset* s, type value) { \
    int index = hashfunc(value) % s->capacity; \
    for (int i = 0; i < s->buckets[index].size; i++) { \
        printf("Comparing " print_format " and " print_format "\n", value, s->buckets[index].array[i]); \
        if (eqfunc(s->buckets[index].array[i], value)) { \
            type##Vector location = &s->buckets[index]; \
            location->array[i] = location->array[location->size - 1]; \
            location->size--; \
            return true; \
        } \
    } \
    return false; \
} \
\
static void type##Hashset_print(type##Hashset* s) { \
    for (int i = 0; i < s->capacity; i++) { \
        printf("%d: ", i); \
        for (int j = 0; j < s->buckets[i].size; j++) { \
            printf(print_format, s->buckets[i].array[j]); \
        } \
        printf("\n"); \
    } \
}
#endif // HASHSET_H