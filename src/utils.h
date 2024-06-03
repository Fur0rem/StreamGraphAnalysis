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

#endif