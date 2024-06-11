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
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_high_precision.txt");
	char* str = SGA_StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	SGA_StreamGraph_destroy(&sg);
	return true;
}

int main() {
	return test("StreamGraph", &(Test){"load", test_load}, &(Test){"load_slices", test_load_slices},
				NULL)
			   ? 0
			   : 1;
}