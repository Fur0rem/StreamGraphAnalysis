#include "../src/stream_graph.h"
#include "test.h"

bool test_load() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	return true;
}

int main() {
	return test("StreamGraph", &(Test){"load", test_load}, NULL) ? 0 : 1;
}