#include "../src/measures.h"
#include "test.h"

bool test_coverage() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_high_precision.txt");
	bool result = EXPECT_F_APPROX_EQ(SGA_coverage(&sg), 0.65, 1e-2);
	SGA_StreamGraph_destroy(&sg);
	return result;
}

bool test_number_of_nodes() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S_high_precision.txt");
	bool result = EXPECT_F_APPROX_EQ(SGA_number_of_nodes(&sg), 2.6, 1e-1);
	SGA_StreamGraph_destroy(&sg);
	return result;
}

int main() {
	return test("StreamGraph", &(Test){"coverage", test_coverage},
				&(Test){"number_of_nodes", test_number_of_nodes}, NULL)
			   ? 0
			   : 1;
}