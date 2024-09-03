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

bool test(const char* suite_name, Test** tests);

#define EXPECT(expr)                                                                                                   \
	({                                                                                                                 \
		bool result = (expr);                                                                                          \
		if (!result) {                                                                                                 \
			printf("\t" TEXT_RED "FAIL" TEXT_RESET " : %s at %s:%d\n", #expr, __FILE__, __LINE__);                     \
		}                                                                                                              \
		result;                                                                                                        \
	})

#define EXPECT_EQ_DEFAULT(type, format)                                                                                \
	bool EXPECT_EQ_##type(type a, type b) {                                                                            \
		bool result = EXPECT((a) == (b));                                                                              \
		if (!result) {                                                                                                 \
			printf("\t\t" TEXT_RED "Expected " format " to be equal to " format TEXT_RESET "\n", a, b);                \
		}                                                                                                              \
		return result;                                                                                                 \
	}

#define EXPECT_F_APPROX_EQ(got, expected, eps)                                                                         \
	({                                                                                                                 \
		bool result = EXPECT(F_EQUALS_APPROX(got, expected, eps));                                                     \
		if (!result) {                                                                                                 \
			printf("\t\t" TEXT_RED "Expected %f to be ~equal to %f" TEXT_RESET "\n", got, expected);                   \
		}                                                                                                              \
		else {                                                                                                         \
			printf("\t\t" TEXT_GREEN "%f is ~equal to %f" TEXT_RESET "\n", got, expected);                             \
		}                                                                                                              \
		result;                                                                                                        \
	})

bool EXPECT_EQ_int(int got, int expected);
bool EXPECT_EQ_String(char* got, char* expected);
bool EXPECT_EQ_size_t(size_t got, size_t expected);
bool EXPECT_EQ_ptr(void* got, void* expected);

#define EXPECT_EQ(a, b)                                                                                                \
	_Generic((a), int: EXPECT_EQ_int, char*: EXPECT_EQ_String, size_t: EXPECT_EQ_size_t, void*: EXPECT_EQ_ptr)(a, b)

bool EXPECT_ALL(int expr, ...);

#define TEST(test_name)                                                                                                \
	&(Test) {                                                                                                          \
		.fn = (test_name), .name = #test_name                                                                          \
	}

#endif // TEST_H