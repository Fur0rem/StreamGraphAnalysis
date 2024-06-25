#include "../src/stream_graph.h"
#include "test.h"

bool test_load() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	printf("Loaded graph\n");
	char* str = StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	StreamGraph_destroy(sg);
	return true;
}

bool test_load_slices() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S_multiple_slices.txt");
	char* str = StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	EXPECT(StreamGraph_lifespan_begin(&sg) == 0);
	EXPECT(StreamGraph_lifespan_end(&sg) == 1000);
	StreamGraph_destroy(sg);
	return true;
}

bool test_find_index_of_time() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	size_t index = KeyMomentsTable_find_time_index(&sg.key_moments, 75);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_in_slices() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S_multiple_slices.txt");
	size_t index = KeyMomentsTable_find_time_index(&sg.key_moments, 750);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_not_found() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	size_t index = KeyMomentsTable_find_time_index(&sg.key_moments, 999999);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(index, sg.events.nb_events);
}

bool test_init_events_table() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_external_format() {
	// Read the file
	char* filename = "tests/test_data/S_external.txt";
	// Open the file
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error: could not open file %s\n", filename);
		return false;
	}
	// Read everything into a buffer
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = (char*)malloc(length + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Error: could not allocate memory for file %s\n", filename);
		fclose(file);
		return false;
	}
	fread(buffer, 1, length, file);
	fclose(file);
	buffer[length] = '\0';

	// Parse the buffer
	char* internal_format = InternalFormat_from_External_str(buffer);
	// printf("%s\n", internal_format);
	StreamGraph sg = StreamGraph_from_string(internal_format);
	free(internal_format);
	char* external_format = StreamGraph_to_string(&sg);
	// printf("%s\n", external_format);
	StreamGraph_destroy(sg);
	free(external_format);
	free(buffer);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"load",						 test_load						 },
		&(Test){"load_slices",				   test_load_slices				   },
		&(Test){"find_index_of_time",			  test_find_index_of_time			 },
		&(Test){"find_index_of_time_in_slices", test_find_index_of_time_in_slices},
		&(Test){"find_index_of_time_not_found", test_find_index_of_time_not_found},
		&(Test){"init_events_table",			 test_init_events_table		   },
		&(Test){"external_format",			   test_external_format			   },

		NULL
	};

	return test("StreamGraph", tests);
}