/**
 * @file tests/stream.c
 * @brief Tests regarding StreamGraph
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"

// TODO : more tests for neighbours and such
bool load() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	printf("Loaded graph\n");
	String str = SGA_StreamGraph_to_string(&sg);
	String_push(&str, '\0');
	printf("%s\n", str.data);
	String_destroy(str);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_load_slices() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S_multiple_slices.txt");
	String str	   = SGA_StreamGraph_to_string(&sg);
	String_push(&str, '\0');
	printf("%s\n", str.data);
	String_destroy(str);
	// EXPECT(SGA_StreamGraph_lifespan_begin(&sg) == 0);
	// EXPECT(SGA_StreamGraph_lifespan_end(&sg) == 1000);
	Interval lifespan = SGA_StreamGraph_lifespan(&sg);
	EXPECT_EQ(lifespan.start, 0);
	EXPECT_EQ(lifespan.end, 1000);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_find_index_of_time() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	size_t index	   = KeyMomentsTable_find_time_index(&sg.key_moments, 75);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_in_slices() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S_multiple_slices.txt");
	size_t index	   = KeyMomentsTable_find_time_index(&sg.key_moments, 750);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, 9);
}

bool test_find_index_of_time_not_found() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	size_t index	   = KeyMomentsTable_find_time_index(&sg.key_moments, 999999);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(index, sg.events.nb_events);
}

bool test_external_format() {
	// Read the file
	char* filename = "data/S_external.txt";
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
	char* internal_format = SGA_InternalFormat_from_External_str(buffer);
	SGA_StreamGraph sg    = SGA_StreamGraph_from_string(internal_format);
	free(internal_format);
	SGA_StreamGraph_destroy(sg);
	free(buffer);
	return true;
}

bool test_from_external_format() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_external("data/S_external.txt");
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
	    TEST(test_external_format),
	    TEST(test_from_external_format),
	    NULL,
	};

	return test("SGA_StreamGraph", tests);
}