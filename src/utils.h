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
// TODO : print stack trace maybe?
#define MALLOC_CHECK(size)                                                                                             \
	({                                                                                                                 \
		if (size == 0) {                                                                                               \
			fprintf(stderr, "Memory allocation with size 0 at %s:%d\n", __FILE__, __LINE__);                           \
			int* ptr = NULL;                                                                                           \
			*ptr	 = 0;                                                                                              \
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
#define DEFAULT_EQUALS(type)                                                                                           \
	bool type##_equals(type* a, type* b) {                                                                             \
		return *a == *b;                                                                                               \
	}
#define DEFAULT_COMPARE(type)                                                                                          \
	int type##_compare(const type* a, const type* b) {                                                                 \
		type a_val = *(type*)a;                                                                                        \
		type b_val = *(type*)b;                                                                                        \
		if (a_val < b_val) {                                                                                           \
			return -1;                                                                                                 \
		}                                                                                                              \
		else if (a_val > b_val) {                                                                                      \
			return 1;                                                                                                  \
		}                                                                                                              \
		else {                                                                                                         \
			return 0;                                                                                                  \
		}                                                                                                              \
	}

#define DeclareEquals(type)	 bool type##_equals(const type* a, const type* b);
#define DeclareCompare(type) int type##_compare(const type* a, const type* b);

#define DEFAULT_MIN_MAX(type)                                                                                          \
	type type##_min(type a, type b) {                                                                                  \
		return a < b ? a : b;                                                                                          \
	}                                                                                                                  \
	type type##_max(type a, type b) {                                                                                  \
		return a > b ? a : b;                                                                                          \
	}

// TODO : optionnal message
#ifdef DEBUG
#	define ASSERT(expr)                                                                                               \
		({                                                                                                             \
			bool __assert_result = (expr);                                                                             \
			if (!(__assert_result)) {                                                                                  \
				fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #expr, __FILE__, __LINE__);                \
				assert(__assert_result);                                                                               \
			}                                                                                                          \
		})
#else
#	ifdef __clang__
#		define ASSERT(expr) __builtin_expect((expr), 1)
#	elif defined(__GNUC__)
#		define ASSERT(expr) __attribute__((assume(expr)))
#	endif
#endif

#ifdef DEBUG
#	define UNREACHABLE_CODE                                                                                           \
		({                                                                                                             \
			fprintf(stderr, "Unreachable code reached at %s:%d\n", __FILE__, __LINE__);                                \
			assert(false);                                                                                             \
		})
#else
#	define UNREACHABLE_CODE __builtin_unreachable()
#endif

typedef struct {
	size_t size;
	size_t capacity;
	char* data;
} String;

#include "utils.h"
#include <stddef.h>
#include <string.h>

String String_from_owned(char* str);
String String_from_duplicate(const char* str);

// TODO: mis namings between push and append
// plus push str and append formatted can be merged
void String_push(String* string, char c);
void String_push_str(String* string, const char* str);
void String_append_formatted(String* string, const char* format, ...);
void String_concat_copy(String* a, const String* b);
void String_concat_consume(String* string, String* b);

void String_destroy(String string);
bool String_equals(const String* a, const String* b);

int String_compare(const void* a, const void* b);
int String_hash(const String* string);

#define DeclareToString(type) String type##_to_string(const type* value);

#define DEFAULT_TO_STRING(type, format)                                                                                \
	String type##_to_string(const type* value) {                                                                       \
		char* str = MALLOC(100);                                                                                       \
		sprintf(str, format, *value);                                                                                  \
		return String_from_owned(str);                                                                                 \
	}

char* read_file(const char* filename);

void String_pop_n(String* string, size_t n);
void String_pop(String* string);

String String_with_capacity(size_t capacity);
String String_new();

#endif