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

bool test_coverage() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	bool result = EXPECT(F_EQUALS_APPROX(SGA_StreamGraph_coverage(&sg), 0.65, 1e-2));
	SGA_StreamGraph_destroy(&sg);
	return result;
}

int main() {
	return test("StreamGraph", &(Test){"load", test_load}, &(Test){"coverage", test_coverage}, NULL)
			   ? 0
			   : 1;
}