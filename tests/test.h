#ifndef TEST_H
#define TEST_H

#include "../src/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	const char* name;
	bool (*fn)();
} Test;

bool test(const char* suite_name, ...);

#define EXPECT(expr)                                                                               \
	({                                                                                             \
		bool result = (expr);                                                                      \
		if (!result) {                                                                             \
			printf("\t" TEXT_RED "FAIL" TEXT_RESET " : %s at %s:%d\n", #expr, __FILE__, __LINE__); \
		}                                                                                          \
		result;                                                                                    \
	})

#define EXPECT_EQ_DEFAULT(type, format)                                                            \
	bool EXPECT_EQ_##type(type a, type b) {                                                        \
		bool result = EXPECT((a) == (b));                                                          \
		if (!result) {                                                                             \
			printf("\t\t" TEXT_RED "Expected " format " to be equal to " format TEXT_RESET "\n",   \
				   a, b);                                                                          \
		}                                                                                          \
		return result;                                                                             \
	}

#define EXPECT_F_APPROX_EQ(a, b, eps)                                                              \
	({                                                                                             \
		bool result = EXPECT(F_EQUALS_APPROX(a, b, eps));                                          \
		if (!result) {                                                                             \
			printf("\t\t" TEXT_RED "Expected %f to be ~equal to %f" TEXT_RESET "\n", a, b);        \
		}                                                                                          \
		else {                                                                                     \
			printf("\t\t" TEXT_GREEN "%f is ~equal to %f" TEXT_RESET "\n", a, b);                  \
		}                                                                                          \
		result;                                                                                    \
	})

bool EXPECT_EQ_int(int a, int b);
bool EXPECT_EQ_String(char* a, char* b);
bool EXPECT_EQ_size_t(size_t a, size_t b);
bool EXPECT_EQ_ptr(void* a, void* b);

#define EXPECT_EQ(a, b)                                                                            \
	_Generic((a),                                                                                  \
		int: EXPECT_EQ_int,                                                                        \
		char*: EXPECT_EQ_String,                                                                   \
		size_t: EXPECT_EQ_size_t,                                                                  \
		void*: EXPECT_EQ_ptr)(a, b)

bool EXPECT_ALL(int expr, ...);

#endif // TEST_H