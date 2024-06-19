#include "../src/metrics.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "../src/stream_graph.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool test_cardinal_of_T_S_0() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");

	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);

	TimesIterator times_iter = funcs.times_node_present(st.stream, 0);
	size_t cardinal = total_time_of(times_iter);

	FSG_destroy(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 100);
}
bool test_cardinal_of_T_S_1() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 1);
	size_t cardinal = total_time_of(times_iter);
	FSG_destroy(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 90);
}

bool test_cardinal_of_T_S_2() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 2);
	size_t cardinal = total_time_of(times_iter);
	FSG_destroy(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 50);
}

bool test_cardinal_of_T_S_3() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 3);
	size_t cardinal = total_time_of(times_iter);
	FSG_destroy(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 20);
}

bool test_times_node_present() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 3);
	printf("Present from : \n");
	FOR_EACH_TIME(times_iter, interval) {
		printf("[%lu, %lu] U ", interval.start, interval.end);
	}
	FSG_destroy(&st);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_cardinal_of_W_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_set(st.stream);
	size_t cardinal = 0;
	FOR_EACH_NODE(nodes_iter, node_id) {
		cardinal += total_time_of(funcs.times_node_present(st.stream, node_id));
	}
	FSG_destroy(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 260);
}

bool test_cardinal_of_W_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = LS_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_set(st.stream);
	size_t cardinal = 0;
	FOR_EACH_NODE(nodes_iter, node_id) {
		cardinal += total_time_of(funcs.times_node_present(st.stream, node_id));
	}
	LS_destroy(&st);
	StreamGraph_destroy(&sg);
	return EXPECT_EQ(cardinal, 400);
}

bool test_coverage_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	printf("udsg = %p\n", fsg.underlying_stream_graph);
	Stream st = (Stream){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	printf("st udsg = %p\n", ((FullStreamGraph*)st.stream)->underlying_stream_graph);
	double coverage = coverage_stream(st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(coverage, 0.65, 1e-6);
}

bool test_coverage_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	Stream st = (Stream){.type = LINK_STREAM, .stream = &ls};
	double coverage = coverage_stream(st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(coverage, 1.0, 1e-6);
}

bool test_node_duration_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream fsg = FSG_from(&sg);
	double duration = node_duration_stream(fsg);
	FSG_destroy(&fsg);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(duration, 6.5, 1e-6);
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