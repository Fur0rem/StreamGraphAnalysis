#include "test.h"
#include "../src/utils.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

bool test(const char* suite_name, Test** tests) {
	printf(" -- Running test suite : " TEXT_BOLD "%s" TEXT_RESET " --\n", suite_name);
	// For each test in the suite
	bool success = true;
	for (int i = 0; true; i++) {

		// Last test is NULL
		Test* tested = tests[i];
		if (tested == NULL) {
			break;
		}
		// Run the test
		printf("Testing " TEXT_BOLD "%s" TEXT_RESET "\n", tested->name);
		bool result = tested->fn();
		printf("%s : ", tested->name);
		if (!result) {
			printf(TEXT_BOLD TEXT_RED "FAIL\n" TEXT_RESET);
			success = false;
		}
		else {
			printf(TEXT_BOLD TEXT_GREEN "SUCCESS\n" TEXT_RESET);
		}
	}

	printf("-- " TEXT_BOLD "%s : ", suite_name);
	if (success) {
		printf(TEXT_GREEN "SUCCESS");
	}
	else {
		printf(TEXT_RED "FAIL");
	}
	printf(TEXT_RESET " --\n");
	return !success;
}

bool EXPECT_ALL(int expr, ...) {
	va_list args;
	va_start(args, expr);
	int success = expr;
	int arg;
	while ((arg = va_arg(args, int))) {
		success = success && arg;
	}
	va_end(args);
	return success;
}

EXPECT_EQ_DEFAULT(int, "%d")
EXPECT_EQ_DEFAULT(size_t, "%zu")

typedef void* ptr;
EXPECT_EQ_DEFAULT(ptr, "%p")

bool EXPECT_EQ_String(char* a, char* b) {
	bool result = EXPECT(strcmp(a, b) == 0);
	if (!result) {
		printf("\t\t" TEXT_RED "Expected (Got) %s to be equal to (Expected) %s" TEXT_RESET "\n", a, b);
	}
	return result;
}