#include "../src/interval.h"
#include "../src/utils.h"
#include "test.h"

#include <stdint.h>
#include <stdlib.h>

bool test_size_1() {
	SGA_Interval i = (SGA_Interval){.start = 5, .end = 10};
	return EXPECT_EQ(SGA_Interval_duration(i), 5);
}

bool test_size_2() {
	SGA_Interval i = (SGA_Interval){.start = 0, .end = 0};
	return EXPECT_EQ(SGA_Interval_duration(i), 0);
}

bool test_size_none() {
	SGA_Interval i = (SGA_Interval){.start = SIZE_MAX, .end = 0};
	return EXPECT_EQ(SGA_Interval_duration(i), 0);
}

bool test_contains() {
	SGA_Interval i = (SGA_Interval){.start = 5, .end = 10};
	return EXPECT(SGA_Interval_contains(i, 7));
}

bool test_contains_start() {
	SGA_Interval i = (SGA_Interval){.start = 5, .end = 10};
	return EXPECT(SGA_Interval_contains(i, 5));
}

bool test_doesnt_contains_end() {
	SGA_Interval i = (SGA_Interval){.start = 5, .end = 10};
	return EXPECT(!SGA_Interval_contains(i, 10));
}

bool test_doesnt_contain() {
	SGA_Interval i = (SGA_Interval){.start = 5, .end = 10};
	return EXPECT(!SGA_Interval_contains(i, 0));
}

bool test_intersection_1() {
	SGA_Interval a		  = (SGA_Interval){.start = 5, .end = 10};
	SGA_Interval b		  = (SGA_Interval){.start = 7, .end = 12};
	SGA_Interval intersection = SGA_Interval_intersection(a, b);
	return EXPECT_EQ(intersection.start, 7) && EXPECT_EQ(intersection.end, 10);
}

bool test_intersection_2() {
	SGA_Interval a		  = (SGA_Interval){.start = 5, .end = 10};
	SGA_Interval b		  = (SGA_Interval){.start = 10, .end = 12};
	SGA_Interval intersection = SGA_Interval_intersection(a, b);
	return EXPECT_EQ(intersection.start, 10) && EXPECT_EQ(intersection.end, 10);
}

bool test_intersection_none() {
	SGA_Interval a		  = (SGA_Interval){.start = 5, .end = 10};
	SGA_Interval b		  = (SGA_Interval){.start = 11, .end = 12};
	SGA_Interval intersection = SGA_Interval_intersection(a, b);
	return EXPECT_EQ(SGA_Interval_duration(intersection), 0);
}

bool test_intervals_set_merge_contained() {
	SGA_IntervalsSet a = SGA_IntervalsSet_alloc(2);
	a.intervals[0]	   = (SGA_Interval){.start = 0, .end = 10};
	a.intervals[1]	   = (SGA_Interval){.start = 5, .end = 7};
	SGA_IntervalsSet_merge(&a);
	bool result = EXPECT_EQ(a.nb_intervals, 1) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 10);
	SGA_IntervalsSet_destroy(a);
	return result;
}

bool test_intervals_set_merge_overlap() {
	SGA_IntervalsSet a = SGA_IntervalsSet_alloc(2);
	a.intervals[0]	   = (SGA_Interval){.start = 0, .end = 10};
	a.intervals[1]	   = (SGA_Interval){.start = 5, .end = 15};
	SGA_IntervalsSet_merge(&a);
	bool result = EXPECT_EQ(a.nb_intervals, 1) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 15);
	SGA_IntervalsSet_destroy(a);
	return result;
}

bool test_intervals_set_merge_contiguous() {
	SGA_IntervalsSet a = SGA_IntervalsSet_alloc(2);
	a.intervals[0]	   = (SGA_Interval){.start = 0, .end = 10};
	a.intervals[1]	   = (SGA_Interval){.start = 10, .end = 15};
	SGA_IntervalsSet_merge(&a);
	bool result = EXPECT_EQ(a.nb_intervals, 1) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 15);
	SGA_IntervalsSet_destroy(a);
	return result;
}

bool test_intervals_set_merge_independent() {
	SGA_IntervalsSet a = SGA_IntervalsSet_alloc(2);
	a.intervals[0]	   = (SGA_Interval){.start = 0, .end = 10};
	a.intervals[1]	   = (SGA_Interval){.start = 15, .end = 20};
	SGA_IntervalsSet_merge(&a);
	bool result = EXPECT_EQ(a.nb_intervals, 2) && EXPECT_EQ(a.intervals[0].start, 0) && EXPECT_EQ(a.intervals[0].end, 10) &&
		      EXPECT_EQ(a.intervals[1].start, 15) && EXPECT_EQ(a.intervals[1].end, 20);
	SGA_IntervalsSet_destroy(a);
	return result;
}

bool test_intervals_set_union_overlap() {
	SGA_IntervalsSet a	  = SGA_IntervalsSet_alloc(1);
	a.intervals[0]		  = (SGA_Interval){.start = 0, .end = 10};
	SGA_IntervalsSet b	  = SGA_IntervalsSet_alloc(2);
	b.intervals[0]		  = (SGA_Interval){.start = 0, .end = 4};
	b.intervals[1]		  = (SGA_Interval){.start = 5, .end = 10};
	SGA_IntervalsSet union_ab = SGA_IntervalsSet_union(a, b);
	for (size_t i = 0; i < union_ab.nb_intervals; i++) {
		printf("[%lu, %lu]\n", union_ab.intervals[i].start, union_ab.intervals[i].end);
	}
	bool result =
	    EXPECT_EQ(union_ab.nb_intervals, 1) && EXPECT_EQ(union_ab.intervals[0].start, 0) && EXPECT_EQ(union_ab.intervals[0].end, 10);

	SGA_IntervalsSet_destroy(a);
	SGA_IntervalsSet_destroy(b);
	SGA_IntervalsSet_destroy(union_ab);

	return result;
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

	return test("SGA_Interval", tests);
}