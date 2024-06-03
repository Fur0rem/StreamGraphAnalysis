#ifndef TEST_H
#define TEST_H

#include <stdbool.h>

typedef struct {
    const char* name;
    bool (*fn)();
} Test;

bool test(const char* suite_name, ...);

#endif // TEST_H