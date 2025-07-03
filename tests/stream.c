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
	size_t index	   = KeyInstantsTable_find_time_index_if_pushed(&sg.key_instants, 75);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_in_slices() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S_multiple_slices.sga");
	size_t index	   = KeyInstantsTable_find_time_index_if_pushed(&sg.key_instants, 750);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_not_found() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");
	size_t index	   = KeyInstantsTable_find_time_index_if_pushed(&sg.key_instants, 999999);
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

bool test_access_key_instants() {
	SGA_StreamGraph sg	= SGA_StreamGraph_from_file("data/tests/parsing_empty_slices.sga");
	size_t nb_expected	= 4;
	SGA_Interval expected[] = {
	    SGA_Interval_from(0, 1100),
	    SGA_Interval_from(1100, 5755),
	    SGA_Interval_from(5755, 11510),
	    SGA_Interval_from(11510, 21245),
	};

	bool result = true;

	size_t i		       = 0;
	SGA_TimesIterator key_instants = SGA_StreamGraph_key_instants(&sg);
	SGA_FOR_EACH_TIME(time, key_instants) {
		if (i >= nb_expected) {
			printf("Found more key instants than expected (%zu > %zu)\n", i + 1, nb_expected);
			result = false;
			key_instants.destroy(&key_instants);
			break;
		}
		result &= EXPECT_EQ(time.start, expected[i].start);
		result &= EXPECT_EQ(time.end, expected[i].end);
		i++;
	}

	SGA_StreamGraph_destroy(sg);
	return result & EXPECT_EQ(i, nb_expected);
}

bool test_parse_empty_lines() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/parsing_empty_lines.sga");
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
	    TEST(test_parse_empty_lines),
	    TEST(test_access_key_instants),
	    NULL,
	};

	return test("SGA_StreamGraph", tests);
}