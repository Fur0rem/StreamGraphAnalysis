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
#define MALLOC_CHECK(size)                                                                                                                 \
	({                                                                                                                                     \
		if ((size) == 0) {                                                                                                                 \
			fprintf(stderr, "Memory allocation with size 0 at %s:%d\n", __FILE__, __LINE__);                                               \
		}                                                                                                                                  \
		void* ptr = malloc(size);                                                                                                          \
		if (ptr == NULL) {                                                                                                                 \
			fprintf(stderr, "Memory allocation failed\n");                                                                                 \
			exit(1);                                                                                                                       \
		}                                                                                                                                  \
		ptr;                                                                                                                               \
	})

#ifdef DEBUG
#	define MALLOC(size) MALLOC_CHECK(size)
#else
#	define MALLOC(size) malloc(size)
#endif

// Floating point comparison
#define F_EQUALS(left, right)			  (fabs((left) - (right)) < 1e-9)
#define F_EQUALS_APPROX(left, right, eps) (fabs((left) - (right)) < (eps))

#define NO_FREE(type)                                                                                                                      \
	void type##_destroy(type self) {}
#define DEFAULT_EQUALS(type)                                                                                                               \
	bool type##_equals(const type* left, const type* right) {                                                                              \
		return *left == *right;                                                                                                            \
	}
#define DEFAULT_COMPARE(type)                                                                                                              \
	int type##_compare(const type* left, const type* right) {                                                                              \
		return *left - *right;                                                                                                             \
	}

#define DeclareEquals(type)	 bool type##_equals(const type* left, const type* right);
#define DeclareCompare(type) int type##_compare(const type* left, const type* right);
#define DeclareDestroy(type) void type##_destroy(type self);
#define DeclareHash(type)	 int type##_hash(const type* self);

#define DEFAULT_MIN_MAX(type)                                                                                                              \
	type type##_min(type left, type right) {                                                                                               \
		return left < right ? left : right;                                                                                                \
	}                                                                                                                                      \
	type type##_max(type left, type right) {                                                                                               \
		return left > right ? left : right;                                                                                                \
	}

// TODO : optionnal message
#ifdef DEBUG
#	define ASSERT(expr)                                                                                                                   \
		({                                                                                                                                 \
			bool __assert_result = (expr);                                                                                                 \
			if (!(__assert_result)) {                                                                                                      \
				fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #expr, __FILE__, __LINE__);                                    \
				assert(__assert_result);                                                                                                   \
			}                                                                                                                              \
		})
#else
#	ifdef __clang__
#		define ASSERT(expr) __builtin_expect((expr), 1)
#	elif defined(__GNUC__)
#		define ASSERT(expr) __attribute__((assume(expr)))
#	endif
#endif

// TODO: add message to signal it's my fault and not the user's
#define DEV_ASSERT(expr) ASSERT(expr)

#ifdef DEBUG
#	define UNREACHABLE_CODE                                                                                                               \
		({                                                                                                                                 \
			fprintf(stderr, "Unreachable code reached at %s:%d\n", __FILE__, __LINE__);                                                    \
			assert(false);                                                                                                                 \
		})
#else
#	define UNREACHABLE_CODE __builtin_unreachable()
#endif

#if defined(__clang__) || defined(__GNUC__)
#	define LIKELY(cond)   (__builtin_expect(!!(cond), 1))
#	define UNLIKELY(cond) (__builtin_expect(!!(cond), 0))
#else
#	define LIKELY(cond)   (cond)
#	define UNLIKELY(cond) (cond)
#endif

#ifdef __clang__
#	define DONT_OPTIMISE __attribute__((optnone))
#elif defined(__GNUC__)
#	define DONT_OPTIMISE __attribute__((optimize("O0")))
#else
#	define DONT_OPTIMISE
#endif

typedef struct {
	// TODO: rename those to the same as arraylist
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
void String_push(String* self, char character);
void String_push_str(String* self, const char* str);
void String_append_formatted(String* self, const char* format, ...);
void String_concat_copy(String* self, const String* with);
void String_concat_consume(String* self, String with);

DeclareDestroy(String);
DeclareEquals(String);
DeclareCompare(String);
DeclareHash(String);

#define DeclareToString(type) String type##_to_string(const type* self);

#define DEFAULT_TO_STRING(type, format)                                                                                                    \
	String type##_to_string(const type* self) {                                                                                            \
		char* str = MALLOC(100);                                                                                                           \
		sprintf(str, format, *self);                                                                                                       \
		return String_from_owned(str);                                                                                                     \
	}

char* read_file(const char* filename);

void String_pop_n(String* self, size_t n);
void String_pop(String* self);

String String_with_capacity(size_t capacity);

#endif