#include "../src/metrics_generic.h"
#include "../src/full_stream_graph.h"
#include "../src/link_stream.h"
#include "../src/stream_graph.h"
#include "test.h"

bool test_coverage_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	double cov = coverage_stream(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(cov, 0.65, 1e-6);
}

bool test_coverage_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	stream_t st = (stream_t){.type = LINK_STREAM, .stream = &ls};
	double cov = coverage_stream(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(cov, 1.0, 1e-6);
}

bool test_node_duration_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	double node_duration = node_duration_stream(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(node_duration, 26.0 / 4.0, 1e-6);
}

bool test_node_duration_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	stream_t st = (stream_t){.type = LINK_STREAM, .stream = &ls};
	double node_duration = node_duration_stream(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(node_duration, 10.0, 1e-6);
}

int main() {
	Test* tests[] = {
		&(Test){"coverage_S",	  test_coverage_S	 },
		&(Test){"coverage_L",	  test_coverage_L	 },
		&(Test){"node_duration_S", test_node_duration_S},
		&(Test){"node_duration_L", test_node_duration_L},
		NULL,
	};

	return test("Metrics", tests);
}