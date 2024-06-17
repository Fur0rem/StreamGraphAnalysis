#include "../src/stream_graph.h"
#include "test.h"

bool test_load() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	printf("Loaded graph\n");
	char* str = StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	init_events_table(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_load_slices() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S_multiple_slices.txt");
	char* str = StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	init_events_table(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_find_index_of_time() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	size_t index = KeyMomentsTable_find_time_index(&sg.key_moments, 75);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_in_slices() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S_multiple_slices.txt");
	size_t index = KeyMomentsTable_find_time_index(&sg.key_moments, 750);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(index, 9);
	return true;
}

bool test_find_index_of_time_not_found() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	size_t index = KeyMomentsTable_find_time_index(&sg.key_moments, 999999);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(index, sg.events.nb_events);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"load",						 test_load						 },
		&(Test){"load_slices",				   test_load_slices				   },
		&(Test){"find_index_of_time",			  test_find_index_of_time			 },
		&(Test){"find_index_of_time_in_slices", test_find_index_of_time_in_slices},
		&(Test){"find_index_of_time_not_found", test_find_index_of_time_not_found},
		NULL
	};

	return test("StreamGraph", tests);
}