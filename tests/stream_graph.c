#include "../src/stream_graph.h"
#include "test.h"

bool test_load() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	char* str = SGA_StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);
	printf("coverage: %f\n", SGA_StreamGraph_coverage(&sg));
	SGA_StreamGraph_destroy(&sg);
	return true;
}

int main() {
	return test("StreamGraph", &(Test){"load", test_load}, NULL) ? 0 : 1;
}