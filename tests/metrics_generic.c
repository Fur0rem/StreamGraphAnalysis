#include "../src/metrics_generic.h"
#include "../src/full_stream_graph.h"
#include "../src/link_stream.h"
#include "../src/stream_graph.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool test_cardinal_of_T_S_0() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	TimesIterator times_iter = full_stream_graph_base_functions.times_node_present(st.stream, 0);
	size_t cardinal = cardinalOfT(times_iter);
	printf("cardinal = %zu\n", cardinal);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 100);
}

bool test_cardinal_of_T_S_1() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	TimesIterator times_iter = full_stream_graph_base_functions.times_node_present(st.stream, 1);
	size_t cardinal = cardinalOfT(times_iter);
	printf("cardinal = %zu\n", cardinal);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 90);
}

bool test_cardinal_of_T_S_2() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	TimesIterator times_iter = full_stream_graph_base_functions.times_node_present(st.stream, 2);
	size_t cardinal = cardinalOfT(times_iter);
	printf("cardinal = %zu\n", cardinal);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 50);
}

bool test_cardinal_of_T_S_3() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	TimesIterator times_iter = full_stream_graph_base_functions.times_node_present(st.stream, 3);
	size_t cardinal = cardinalOfT(times_iter);
	printf("cardinal = %zu\n", cardinal);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 20);
}

bool test_times_node_present() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	TimesIterator times_iter = full_stream_graph_base_functions.times_node_present(st.stream, 3);
	printf("Present from : \n");
	FOR_EACH_TIME(times_iter, interval) {
		printf("[%lu, %lu] U ", interval.start, interval.end);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_cardinal_of_W_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	NodesIterator nodes_iter = full_stream_graph_base_functions.nodes_set(st.stream);
	/*size_t cardinal_1 = 0;
	FOR_EACH_NODE(nodes_iter, node_id) {
		printf("Node %lu\n", node_id);
		cardinal_1 += cardinalOfT(full_stream_graph_base_functions.times_node_present(st.stream, node_id));
	}
	nodes_iter = full_stream_graph_base_functions.nodes_set(st.stream);*/
	size_t cardinal = cardinalOfW(nodes_iter);
	// printf("cardinal = %zu, cardinal_1 = %zu\n", cardinal, cardinal_1);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 260);
}

bool test_cardinal_of_W_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	stream_t st = (stream_t){.type = LINK_STREAM, .stream = &ls};
	NodesIterator nodes_iter = link_stream_base_functions.nodes_set(st.stream);
	size_t cardinal = cardinalOfW(nodes_iter);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 400);
}

bool test_coverage_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	printf("udsg = %p\n", fsg.underlying_stream_graph);
	stream_t st = (stream_t){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	printf("st udsg = %p\n", ((FullStreamGraph*)st.stream)->underlying_stream_graph);
	double coverage = coverage_stream(st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(coverage, 0.65, 1e-6);
}

bool test_coverage_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	stream_t st = (stream_t){.type = LINK_STREAM, .stream = &ls};
	double coverage = coverage_stream(st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(coverage, 1.0, 1e-6);
}

int main() {
	/*Test* tests[] = {
		&(Test){"cardinal_of_W_S", test_cardinal_of_W_S},
		  &(Test){"coverage_S",		test_coverage_S	   },
		&(Test){"coverage_L",	  test_coverage_L	 },
		  &(Test){"node_duration_S", test_node_duration_S},
		&(Test){"node_duration_L", test_node_duration_L},
		  NULL,
	};*/

	Test* tests[] = {
		&(Test){"cardinal_of_T_S_0", test_cardinal_of_T_S_0},
		&(Test){"cardinal_of_T_S_1", test_cardinal_of_T_S_1},
		&(Test){"cardinal_of_T_S_2", test_cardinal_of_T_S_2},
		&(Test){"cardinal_of_T_S_3", test_cardinal_of_T_S_3},
		&(Test){"cardinal_of_W_S",   test_cardinal_of_W_S  },
 //&(Test){"times_node_present", test_times_node_present},
		&(Test){"cardinal_of_W_L",   test_cardinal_of_W_L  },
		&(Test){"coverage_S",		  test_coverage_S		 },
		&(Test){"coverage_L",		  test_coverage_L		 },

		NULL,
	};

	return test("Metrics", tests);
}