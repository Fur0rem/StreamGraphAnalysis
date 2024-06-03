#include "test.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

bool test(const char* suite_name, ...) {
    // For each test in the suite
    va_list args;
    va_start(args, suite_name);
    Test* test;
    while ((test = va_arg(args, Test*))) {
        // Run the test
        printf("Running test %s\n", test->name);
        if (!test->fn()) {
            printf("Test failed\n");
            return false;
        }
    }
    va_end(args);
    return true;
}