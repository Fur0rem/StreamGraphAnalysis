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
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_high_precision.txt");
	bool result = EXPECT_F_APPROX_EQ(SGA_StreamGraph_coverage(&sg), 0.65, 1e-2);
	SGA_StreamGraph_destroy(&sg);
	return result;
}

bool test_number_of_nodes() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_high_precision.txt");
	bool result = EXPECT_F_APPROX_EQ(SGA_StreamGraph_number_of_nodes(&sg), 2.6, 1e-1);
	SGA_StreamGraph_destroy(&sg);
	return result;
}

int main() {
	return test("StreamGraph", &(Test){"load", test_load}, &(Test){"coverage", test_coverage},
				&(Test){"number_of_nodes", test_number_of_nodes}, NULL)
			   ? 0
			   : 1;
}