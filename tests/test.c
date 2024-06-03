#include "test.h"
#include "../src/utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

bool test(const char* suite_name, ...) {
    printf(" -- Running test suite : " TEXT_BOLD"%s"TEXT_RESET " --\n", suite_name);
    // For each test in the suite
    va_list args;
    va_start(args, suite_name);
    Test* test;
    bool success = true;
    while ((test = va_arg(args, Test*))) {
        // Run the test
        printf("%s : ", test->name);
        if (!test->fn()) {
            printf(TEXT_RED"FAIL\n"TEXT_RESET);
            success = false;
        }
        else {
            printf(TEXT_GREEN"SUCESS\n"TEXT_RESET);
        }
        
    }
    va_end(args);

    printf("-- " TEXT_BOLD "%s : ", suite_name);
    if (success) {
        printf(TEXT_GREEN "SUCCESS");
    }
    else {
        printf(TEXT_RED "FAIL");
    }
    printf(TEXT_RESET " --\n");
    return success;
}