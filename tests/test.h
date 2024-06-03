#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <string.h>
#include "../src/utils.h"
#include <stdio.h>

typedef struct {
    const char* name;
    bool (*fn)();
} Test;

bool test(const char* suite_name, ...);

#define EXPECT(expr) ({ \
    bool result = (expr); \
    if (!result) { \
        printf("  " TEXT_RED "FAIL" TEXT_RESET " : %s at %s:%d\n", #expr, __FILE__, __LINE__); \
    } \
    result; \
})

bool EXPECT_EQ_INT(int a, int b);

bool EXPECT_EQ_STRING(char* a, char* b);

#define EXPECT_EQ(a, b) _Generic((a), \
    int: EXPECT_EQ_INT, \
    char*: EXPECT_EQ_STRING \
)(a, b)

bool EXPECT_ALL(int expr, ...);

#endif // TEST_H