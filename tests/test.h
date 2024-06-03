#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

typedef struct {
    const char* name;
    bool (*fn)();
} Test;

bool test(const char* suite_name, ...);

#define EXPECT(expr) ({ \
    bool result = (expr); \
    if (!result) { \
        printf("  " TEXT_RED "FAIL" TEXT_RESET " : %s\n", #expr); \
    } \
    result; \
})

#define EXPECT_EQ(a, b) ({ \
    bool result = EXPECT((a) == (b)); \
    if (!result) { \
        printf("    " TEXT_RED "Expected %d to be equal to %d" TEXT_RESET "\n", a, b); \
    } \
    result; \
})

bool EXPECT_ALL(int expr, ...);

#endif // TEST_H