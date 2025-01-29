/**
 * @file tests/stream.c
 * @brief Tests regarding StreamGraph
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"

// TODO : more tests for neighbours and such
bool load() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_load_slices() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S_multiple_slices.sga");
	String str	   = SGA_StreamGraph_to_string(&sg);
	String_push(&str, '\0');
	printf("%s\n", str.data);
	String_destroy(str);
	SGA_Interval lifespan = SGA_StreamGraph_lifespan(&sg);
	EXPECT_EQ(lifespan.start, 0);
	EXPECT_EQ(lifespan.end, 1000);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_find_index_of_time() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");
	size_t index	   = KeyMomentsTable_find_time_index_if_pushed(&sg.key_moments, 75);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_in_slices() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S_multiple_slices.sga");
	size_t index	   = KeyMomentsTable_find_time_index_if_pushed(&sg.key_moments, 750);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_not_found() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");
	size_t index	   = KeyMomentsTable_find_time_index_if_pushed(&sg.key_moments, 999999);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, sg.events.nb_events);
}

bool test_missing_nodes() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/missing_nodes.sga");
	String str	   = SGA_StreamGraph_to_string(&sg);
	String_push(&str, '\0');
	printf("%s\n", str.data);
	String_destroy(str);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_no_links() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/no_links.sga");
	String str	   = SGA_StreamGraph_to_string(&sg);
	String_push(&str, '\0');
	printf("%s\n", str.data);
	String_destroy(str);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_wrong_order_internal() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S_internal_unordered.sga");
	String str	   = SGA_StreamGraph_to_string(&sg);
	String_push(&str, '\0');
	printf("%s\n", str.data);
	String_destroy(str);
	SGA_StreamGraph_destroy(sg);
	return true;
}

int main() {
	Test* tests[] = {
	    TEST(load),
	    TEST(test_load_slices),
	    TEST(test_find_index_of_time),
	    TEST(test_find_index_of_time_in_slices),
	    TEST(test_find_index_of_time_not_found),
	    TEST(test_missing_nodes),
	    TEST(test_no_links),
	    TEST(test_wrong_order_internal),
	    NULL,
	};

	return test("SGA_StreamGraph", tests);
}