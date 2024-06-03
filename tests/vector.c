#include "test.h"
#include "../src/vector.h"

DefVector(int);

bool test_create() {
    intVector v = intVector_with_capacity(5);
    return EXPECT_EQ(v.size, 0) && EXPECT_EQ(v.capacity, 5);
}

bool test_push_1() {
    intVector v = intVector_with_capacity(5);
    intVector_push(&v, 1);
    intVector_push(&v, 2);
    intVector_push(&v, 3);
    intVector_push(&v, 4);
    intVector_push(&v, 5);
    return EXPECT_ALL(
        EXPECT_EQ(v.size, 5),
        EXPECT_EQ(v.capacity, 5),
        EXPECT_EQ(v.array[0], 1),
        EXPECT_EQ(v.array[1], 2),
        EXPECT_EQ(v.array[2], 3),
        EXPECT_EQ(v.array[3], 4),
        EXPECT_EQ(v.array[4], 5));
}

bool test_push_2() {
    intVector v = intVector_with_capacity(5);
    intVector_push(&v, 1);
    intVector_push(&v, 2);
    intVector_push(&v, 3);
    intVector_push(&v, 4);
    intVector_push(&v, 5);
    intVector_push(&v, 6);
    return v.size == 6 && v.capacity == 10 && v.array[0] == 1 && v.array[1] == 2 && v.array[2] == 3 && v.array[3] == 4 && v.array[4] == 5 && v.array[5] == 6;
}

bool test_destroy() {
    intVector v = intVector_with_capacity(5);
    intVector_push(&v, 1);
    intVector_push(&v, 2);
    intVector_push(&v, 3);
    intVector_push(&v, 4);
    intVector_push(&v, 5);
    intVector_destroy(v);
    return true;
}

int main() {
    return test("Vector", 
        &(Test) { "create", test_create },
        &(Test) { "push 1", test_push_1 },
        &(Test) { "push 2", test_push_2 },
        &(Test) { "destroy", test_destroy },
        NULL
    ) ? 0 : 1;
}
