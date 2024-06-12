#include "../src/interval.h"
#include "../src/utils.h"
#include "test.h"

#include <stdint.h>
#include <stdlib.h>

bool test_size_1() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT_EQ(Interval_size(i), 5);
}

bool test_size_2() {
	Interval i = (Interval){.start = 0, .end = 0};
	return EXPECT_EQ(Interval_size(i), 0);
}

bool test_size_none() {
	Interval i = (Interval){.start = SIZE_MAX, .end = 0};
	return EXPECT_EQ(Interval_size(i), 0);
}

bool test_contains_1() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(Interval_contains(i, 5));
}

bool test_contains_2() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(Interval_contains(i, 10));
}

bool test_contains_3() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(!Interval_contains(i, 0));
}

bool test_intersection_1() {
	Interval a = (Interval){.start = 5, .end = 10};
	Interval b = (Interval){.start = 7, .end = 12};
	Interval intersection = Interval_intersection(a, b);
	return EXPECT_EQ(intersection.start, 7) && EXPECT_EQ(intersection.end, 10);
}

bool test_intersection_2() {
	Interval a = (Interval){.start = 5, .end = 10};
	Interval b = (Interval){.start = 10, .end = 12};
	Interval intersection = Interval_intersection(a, b);
	return EXPECT_EQ(intersection.start, 10) && EXPECT_EQ(intersection.end, 10);
}

bool test_intersection_none() {
	Interval a = (Interval){.start = 5, .end = 10};
	Interval b = (Interval){.start = 11, .end = 12};
	Interval intersection = Interval_intersection(a, b);
	return EXPECT_EQ(Interval_size(intersection), 0);
}

int main() {
	return test("Interval", &(Test){"interval_size 1", test_size_1},
				&(Test){"interval_size 2", test_size_2},
				&(Test){"interval_size none", test_size_none},
				&(Test){"interval_contains 1", test_contains_1},
				&(Test){"interval_contains 2", test_contains_2},
				&(Test){"interval_contains 3", test_contains_3},
				&(Test){"interval_intersection 1", test_intersection_1},
				&(Test){"interval_intersection 2", test_intersection_2},
				&(Test){"interval_intersection none", test_intersection_none}, NULL)
			   ? 0
			   : 1;
}