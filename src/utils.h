#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Text formatting
#define TEXT_BOLD  "\033[1m"
#define TEXT_RED   "\033[31m"
#define TEXT_GREEN "\033[32m"
#define TEXT_RESET "\033[0m"

// Malloc with error handling
#define MALLOC_CHECK(size)                                                                                             \
	({                                                                                                                 \
		if (size == 0) {                                                                                               \
			fprintf(stderr, "Memory allocation with size 0\n");                                                        \
		}                                                                                                              \
		void* ptr = malloc(size);                                                                                      \
		if (ptr == NULL) {                                                                                             \
			fprintf(stderr, "Memory allocation failed\n");                                                             \
			exit(1);                                                                                                   \
		}                                                                                                              \
		ptr;                                                                                                           \
	})

#ifdef DEBUG
#	define MALLOC(size) MALLOC_CHECK(size)
#else
#	define MALLOC(size) malloc(size)
#endif

// Floating point comparison
#define EPS						   1e-9
#define F_EQUALS(a, b)			   (fabs((a) - (b)) < EPS)
#define F_EQUALS_APPROX(a, b, eps) (fabs((a) - (b)) < eps)

#define NO_FREE(type) ((void (*)(type))NULL)
#define UNCOMPARABLE(type)                                                                                             \
	bool type##_equals(type a, type b) {                                                                               \
		return false;                                                                                                  \
	}

#define DEFAULT_COMPARE(type)                                                                                          \
	bool type##_equals(type val1, type val2) {                                                                         \
		return (val1) == (val2);                                                                                       \
	}

#define DEFAULT_TO_STRING(type, format)                                                                                \
	char* type##_to_string(const type* value) {                                                                        \
		char* str = (char*)malloc(100);                                                                                \
		sprintf(str, format, *value);                                                                                  \
		return str;                                                                                                    \
	}

#define DeclareGenerics(type)                                                                                          \
	char* type##_to_string(const type* value);                                                                         \
	bool type##_equals(type a, type b);

#define DEFAULT_MIN_MAX(type)                                                                                          \
	type type##_min(type a, type b) {                                                                                  \
		return a < b ? a : b;                                                                                          \
	}                                                                                                                  \
	type type##_max(type a, type b) {                                                                                  \
		return a > b ? a : b;                                                                                          \
	}

#ifdef DEBUG
#	define ASSERT(expr)                                                                                               \
		if (!(expr)) {                                                                                                 \
			fprintf(stderr, "Assertion failed: %s\n", #expr);                                                          \
			assert(expr);                                                                                              \
		}
#else
#	ifdef __clang__
#		define ASSERT(expr) __builtin_expect((expr), 1)
#	elif defined(__GNUC__)
#		define ASSERT(expr) __attribute__((assume(expr)))
#	endif
#endif

#endif