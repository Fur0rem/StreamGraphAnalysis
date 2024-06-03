#include "test.h"
#include "../src/interval.h"
#include "../src/utils.h"

#include <stdlib.h>

bool test_size_1() {
    Interval i = (Interval) {
        .start = 5,
        .end = 10
    };
    return F_EQUALS(interval_size(i), 5);
}

bool test_size_2() {
    Interval i = (Interval) {
        .start = 0,
        .end = 0
    };
    return F_EQUALS(interval_size(i), 0);
}

bool test_contains_1() {
    Interval i = (Interval) {
        .start = 5,
        .end = 10
    };
    return interval_contains(i, 5);
}

bool test_contains_2() {
    Interval i = (Interval) {
        .start = 5,
        .end = 10
    };
    return interval_contains(i, 10);
}

bool test_contains_3() {
    Interval i = (Interval) {
        .start = 5,
        .end = 10
    };
    return !interval_contains(i, 0);
}

int main() {
    return test("Interval", 
        &(Test) { "interval_size 1", test_size_1 },
        &(Test) { "interval_size 2", test_size_2 },
        &(Test) { "interval_contains 1", test_contains_1 },
        &(Test) { "interval_contains 2", test_contains_2 },
        &(Test) { "interval_contains 3", test_contains_3 },
        NULL
    ) ? 0 : 1;
}