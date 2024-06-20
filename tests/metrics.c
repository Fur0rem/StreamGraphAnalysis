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
	double coverage = Stream_coverage(st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(coverage, 0.65, 1e-6);
}

bool test_coverage_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	Stream st = (Stream){.type = LINK_STREAM, .stream = &ls};
	double coverage = Stream_coverage(st);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(coverage, 1.0, 1e-6);
}

bool test_node_duration_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream fsg = FSG_from(&sg);
	double duration = Stream_node_duration(fsg);
	FSG_destroy(&fsg);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(duration, 6.5, 1e-6);
}

#define TEST_METRIC_F(name, value, graph, type)                                                                        \
	bool test_##name() {                                                                                               \
		StreamGraph sg = StreamGraph_from_file("tests/test_data/" #graph ".txt");                                      \
		Stream st = type##_from(&sg);                                                                                  \
		bool result = EXPECT_F_APPROX_EQ(Stream_##name(st), value, 1e-2);                                              \
		StreamGraph_destroy(&sg);                                                                                      \
		type##_destroy(&st);                                                                                           \
		return result;                                                                                                 \
	}

#define TEST_METRIC_I(name, value, graph, type)                                                                        \
	bool test_##name() {                                                                                               \
		StreamGraph sg = StreamGraph_from_file("tests/test_data/" #graph ".txt");                                      \
		Stream st = type##_from(&sg);                                                                                  \
		bool result = EXPECT_EQ(Stream_##name(st), value);                                                             \
		StreamGraph_destroy(&sg);                                                                                      \
		type##_destroy(&st);                                                                                           \
		return result;                                                                                                 \
	}

TEST_METRIC_F(coverage, 0.65, S, FSG)
TEST_METRIC_F(number_of_nodes, 2.6, S, FSG)
TEST_METRIC_F(number_of_links, 1.0, S, FSG)
TEST_METRIC_F(node_duration, 6.5, S, FSG)
TEST_METRIC_F(link_duration, 1.66666666666, S, FSG)

bool test_contribution_of_nodes() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	double contribution_a = Stream_contribution_of_node(st, 0);
	double contribution_b = Stream_contribution_of_node(st, 1);
	double contribution_c = Stream_contribution_of_node(st, 2);
	double contribution_d = Stream_contribution_of_node(st, 3);
	StreamGraph_destroy(&sg);
	FSG_destroy(&st);
	return EXPECT_F_APPROX_EQ(contribution_a, 1.0, 1e-2) && EXPECT_F_APPROX_EQ(contribution_b, 0.9, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_c, 0.5, 1e-2) && EXPECT_F_APPROX_EQ(contribution_d, 0.2, 1e-2);
}

bool test_contributions_of_links() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	double contribution_ab = Stream_contribution_of_link(st, 0);
	double contribution_bd = Stream_contribution_of_link(st, 1);
	double contribution_ac = Stream_contribution_of_link(st, 2);
	double contribution_bc = Stream_contribution_of_link(st, 3);
	StreamGraph_destroy(&sg);
	FSG_destroy(&st);
	return EXPECT_F_APPROX_EQ(contribution_ab, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bd, 0.1, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_ac, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bc, 0.3, 1e-2);
}

TEST_METRIC_F(uniformity, 22.0 / 56.0, S, FSG)
// TEST_METRIC_F(density, 10.0 / 22.0, S)
// TEST_METRIC_F(compactness, 26.0 / 40.0, S)

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
		&(Test){"cardinal_of_T_S_0",		 test_cardinal_of_T_S_0	   },
		&(Test){"cardinal_of_T_S_1",		 test_cardinal_of_T_S_1	   },
		&(Test){"cardinal_of_T_S_2",		 test_cardinal_of_T_S_2	   },
		&(Test){"cardinal_of_T_S_3",		 test_cardinal_of_T_S_3	   },
		&(Test){"cardinal_of_W_S",		   test_cardinal_of_W_S	   },
 //&(Test){"times_node_present", test_times_node_present},
		&(Test){"cardinal_of_W_L",		   test_cardinal_of_W_L	   },
		&(Test){"coverage_S",			  test_coverage_S			 },
		&(Test){"coverage_L",			  test_coverage_L			 },

		&(Test){"number_of_nodes",		   test_number_of_nodes	   },
		&(Test){"number_of_links",		   test_number_of_links	   },
		&(Test){"node_duration",			 test_node_duration		   },
		&(Test){"link_duration",			 test_link_duration		   },
		&(Test){"contribution_of_nodes",	 test_contribution_of_nodes },
		&(Test){"contributions_of_links", test_contributions_of_links},
		&(Test){"uniformity",			  test_uniformity			 },

		NULL,
	};

	return test("Metrics", tests);
}