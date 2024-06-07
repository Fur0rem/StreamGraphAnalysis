#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Text formatting
#define TEXT_BOLD "\033[1m"
#define TEXT_RED "\033[31m"
#define TEXT_GREEN "\033[32m"
#define TEXT_RESET "\033[0m"

// Malloc with error handling
#define MALLOC_CHECK(size)                                                                         \
	({                                                                                             \
		void* ptr = (void*)malloc(size);                                                           \
		if (ptr == NULL) {                                                                         \
			fprintf(stderr, "Memory allocation failed\n");                                         \
			exit(1);                                                                               \
		}                                                                                          \
		ptr;                                                                                       \
	})

// Floating point comparison
#define EPS 1e-9
#define F_EQUALS(a, b) (fabs((a) - (b)) < EPS)

#define NO_FREE(type) ((void (*)(type))NULL)
#define UNCOMPARABLE(type)                                                                         \
	bool type##_equals(type a, type b) {                                                           \
		return false;                                                                              \
	}

#define DEFAULT_COMPARE(type)                                                                      \
	bool type##_equals(type a, type b) {                                                           \
		return a == b;                                                                             \
	}

#define DEFAULT_TO_STRING(type, format)                                                            \
	char* type##_to_string(type* value) {                                                          \
		char* str = (char*)malloc(100);                                                            \
		sprintf(str, format, *value);                                                              \
		return str;                                                                                \
	}

#define DeclareGenerics(type)                                                                      \
	char* type##_to_string(type* value);                                                           \
	bool type##_equals(type a, type b);

#endif