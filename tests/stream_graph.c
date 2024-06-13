#include "../src/stream_graph.h"
#include "test.h"

bool test_load() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	char* str = SGA_StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	SGA_StreamGraph_destroy(&sg);
	return true;
}

bool test_load_slices() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_multiple_slices.txt");
	char* str = SGA_StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	SGA_StreamGraph_destroy(&sg);
	return true;
}

bool test_find_index_of_time() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	size_t index = SGA_KeyMomentsTable_find_time_index(&sg.key_moments, 75);
	SGA_StreamGraph_destroy(&sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_in_slices() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_multiple_slices.txt");
	size_t index = SGA_KeyMomentsTable_find_time_index(&sg.key_moments, 750);
	SGA_StreamGraph_destroy(&sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_not_found() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	size_t index = SGA_KeyMomentsTable_find_time_index(&sg.key_moments, 999999);
	SGA_StreamGraph_destroy(&sg);
	return EXPECT_EQ(index, SIZE_MAX);
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