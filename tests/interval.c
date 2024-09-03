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

bool test_contains() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(Interval_contains(i, 7));
}

bool test_contains_start() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(Interval_contains(i, 5));
}

bool test_doesnt_contains_end() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(!Interval_contains(i, 10));
}

bool test_doesnt_contain() {
	Interval i = (Interval){.start = 5, .end = 10};
	return EXPECT(!Interval_contains(i, 0));
}

bool test_intersection_1() {
	Interval a			  = (Interval){.start = 5, .end = 10};
	Interval b			  = (Interval){.start = 7, .end = 12};
	Interval intersection = Interval_intersection(a, b);
	return EXPECT_EQ(intersection.start, 7) && EXPECT_EQ(intersection.end, 10);
}

bool test_intersection_2() {
	Interval a			  = (Interval){.start = 5, .end = 10};
	Interval b			  = (Interval){.start = 10, .end = 12};
	Interval intersection = Interval_intersection(a, b);
	return EXPECT_EQ(intersection.start, 10) && EXPECT_EQ(intersection.end, 10);
}

bool test_intersection_none() {
	Interval a			  = (Interval){.start = 5, .end = 10};
	Interval b			  = (Interval){.start = 11, .end = 12};
	Interval intersection = Interval_intersection(a, b);
	return EXPECT_EQ(Interval_size(intersection), 0);
}

bool test_intervals_set_merge_contained() {
	IntervalsSet a = IntervalsSet_alloc(2);
	a.intervals[0] = (Interval){.start = 0, .end = 10};
	a.intervals[1] = (Interval){.start = 5, .end = 7};
	IntervalsSet_merge(&a);
	return EXPECT_EQ(a.nb_intervals, 1) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 10);
}

bool test_intervals_set_merge_overlap() {
	IntervalsSet a = IntervalsSet_alloc(2);
	a.intervals[0] = (Interval){.start = 0, .end = 10};
	a.intervals[1] = (Interval){.start = 5, .end = 15};
	IntervalsSet_merge(&a);
	return EXPECT_EQ(a.nb_intervals, 1) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 15);
}

bool test_intervals_set_merge_contiguous() {
	IntervalsSet a = IntervalsSet_alloc(2);
	a.intervals[0] = (Interval){.start = 0, .end = 10};
	a.intervals[1] = (Interval){.start = 10, .end = 15};
	IntervalsSet_merge(&a);
	return EXPECT_EQ(a.nb_intervals, 1) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 15);
}

bool test_intervals_set_merge_independent() {
	IntervalsSet a = IntervalsSet_alloc(2);
	a.intervals[0] = (Interval){.start = 0, .end = 10};
	a.intervals[1] = (Interval){.start = 15, .end = 20};
	IntervalsSet_merge(&a);
	return EXPECT_EQ(a.nb_intervals, 2) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 10) &&
		   EXPECT_EQ(a.intervals[1].start, 15) && EXPECT_EQ(a.intervals[1].end, 20);
}

bool test_intervals_set_union_overlap() {
	IntervalsSet a		  = IntervalsSet_alloc(1);
	a.intervals[0]		  = (Interval){.start = 0, .end = 10};
	IntervalsSet b		  = IntervalsSet_alloc(2);
	b.intervals[0]		  = (Interval){.start = 0, .end = 4};
	b.intervals[1]		  = (Interval){.start = 5, .end = 10};
	IntervalsSet union_ab = IntervalsSet_union(a, b);
	for (size_t i = 0; i < union_ab.nb_intervals; i++) {
		printf("[%lu, %lu]\n", union_ab.intervals[i].start, union_ab.intervals[i].end);
	}
	return EXPECT_EQ(union_ab.nb_intervals, 1) && EXPECT_EQ(union_ab.intervals[0].start, 0) &&
		   EXPECT_EQ(union_ab.intervals[0].end, 10);
}

int main() {
	Test* tests[] = {
		TEST(test_size_1),
		TEST(test_size_2),
		TEST(test_size_none),
		TEST(test_contains),
		TEST(test_contains_start),
		TEST(test_doesnt_contains_end),
		TEST(test_doesnt_contain),
		TEST(test_intersection_1),
		TEST(test_intersection_2),
		TEST(test_intersection_none),
		TEST(test_intervals_set_merge_contained),
		TEST(test_intervals_set_merge_overlap),
		TEST(test_intervals_set_merge_contiguous),
		TEST(test_intervals_set_merge_independent),
		TEST(test_intervals_set_union_overlap),
		NULL,
	};

	return test("Interval", tests);
}