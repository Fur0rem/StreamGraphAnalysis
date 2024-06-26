#include "../src/metrics.h"
#include "../src/stream/chunk_stream.h"
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

	FSG_destroy(st);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 100);
}
bool test_cardinal_of_T_S_1() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 1);
	size_t cardinal = total_time_of(times_iter);
	FSG_destroy(st);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 90);
}

bool test_cardinal_of_T_S_2() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 2);
	size_t cardinal = total_time_of(times_iter);
	FSG_destroy(st);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 50);
}

bool test_cardinal_of_T_S_3() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	TimesIterator times_iter = funcs.times_node_present(st.stream, 3);
	size_t cardinal = total_time_of(times_iter);
	FSG_destroy(st);
	StreamGraph_destroy(sg);
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
	FSG_destroy(st);
	StreamGraph_destroy(sg);
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
	FSG_destroy(st);
	StreamGraph_destroy(sg);
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
	LS_destroy(st);
	StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 400);
}

bool test_coverage_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FullStreamGraph fsg = FullStreamGraph_from(&sg);
	printf("udsg = %p\n", fsg.underlying_stream_graph);
	Stream st = (Stream){.type = FULL_STREAM_GRAPH, .stream = &fsg};
	printf("st udsg = %p\n", ((FullStreamGraph*)st.stream)->underlying_stream_graph);
	double coverage = Stream_coverage(st);
	StreamGraph_destroy(sg);
	return EXPECT_F_APPROX_EQ(coverage, 0.65, 1e-6);
}

bool test_coverage_L() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	LinkStream ls = LinkStream_from(&sg);
	Stream st = (Stream){.type = LINK_STREAM, .stream = &ls};
	double coverage = Stream_coverage(st);
	StreamGraph_destroy(sg);
	return EXPECT_F_APPROX_EQ(coverage, 1.0, 1e-6);
}

bool test_node_duration_S() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	double duration = Stream_node_duration(st);
	FSG_destroy(st);
	StreamGraph_destroy(sg);
	return EXPECT_F_APPROX_EQ(duration, 6.5, 1e-6);
}

#define TEST_METRIC_F(name, value, graph, type)                                                                        \
	bool test_##name() {                                                                                               \
		StreamGraph sg = StreamGraph_from_file("tests/test_data/" #graph ".txt");                                      \
		Stream st = type##_from(&sg);                                                                                  \
		bool result = EXPECT_F_APPROX_EQ(Stream_##name(st), value, 1e-2);                                              \
		StreamGraph_destroy(sg);                                                                                       \
		type##_destroy(st);                                                                                            \
		return result;                                                                                                 \
	}

#define TEST_METRIC_I(name, value, graph, type)                                                                        \
	bool test_##name() {                                                                                               \
		StreamGraph sg = StreamGraph_from_file("tests/test_data/" #graph ".txt");                                      \
		Stream st = type##_from(&sg);                                                                                  \
		bool result = EXPECT_EQ(Stream_##name(st), value);                                                             \
		StreamGraph_destroy(sg);                                                                                       \
		type##_destroy(st);                                                                                            \
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
	StreamGraph_destroy(sg);
	FSG_destroy(st);
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
	StreamGraph_destroy(sg);
	FSG_destroy(st);
	return EXPECT_F_APPROX_EQ(contribution_ab, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bd, 0.1, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_ac, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bc, 0.3, 1e-2);
}

TEST_METRIC_F(uniformity, 22.0 / 56.0, S, FSG)
TEST_METRIC_F(density, 10.0 / 22.0, S, FSG)

bool test_density_of_link() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	double density_ab = Stream_density_of_link(st, 0);
	double density_bd = Stream_density_of_link(st, 1);
	double density_ac = Stream_density_of_link(st, 2);
	double density_bc = Stream_density_of_link(st, 3);
	StreamGraph_destroy(sg);
	FSG_destroy(st);
	return EXPECT_F_APPROX_EQ(density_ab, 1.0 / 3.0, 1e-2) && EXPECT_F_APPROX_EQ(density_bd, 1.0 / 2.0, 1e-2);
}

bool test_density_of_node() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	double density_d = Stream_density_of_node(st, 3);
	StreamGraph_destroy(sg);
	FSG_destroy(st);
	return EXPECT_F_APPROX_EQ(density_d, 1.0 / 4.0, 1e-2);
}

bool test_density_of_time() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	Stream st = FSG_from(&sg);
	double density_2 = Stream_density_of_time(st, 20); // TODO : add automatic traduction with scaling
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	FSG_destroy(st);
	return EXPECT_F_APPROX_EQ(density_2, 2.0 / 3.0, 1e-2);
}

bool test_chunk_stream_nodes_set() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	NodeIdVector nodes = NodeIdVector_with_capacity(2);
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 2);

	LinkIdVector links = LinkIdVector_with_capacity(4);
	LinkIdVector_push(&links, 0);
	LinkIdVector_push(&links, 1);
	LinkIdVector_push(&links, 2);
	LinkIdVector_push(&links, 3);

	Stream st = CS_from(&sg, &nodes, &links, 0, 100);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_set(st.stream);
	FOR_EACH_NODE(nodes_iter, node_id) {
		printf("NODE %zu\n", node_id);
	}

	LinksIterator links_iter = funcs.links_set(st.stream);
	FOR_EACH_LINK(links_iter, link_id) {
		printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdVector_destroy(nodes);
	LinkIdVector_destroy(links);
	return true;
}

bool test_neighbours_of_node_chunk_stream() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	NodeIdVector nodes = NodeIdVector_with_capacity(2);
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 1);
	NodeIdVector_push(&nodes, 3);

	LinkIdVector links = LinkIdVector_with_capacity(4);
	LinkIdVector_push(&links, 0);
	LinkIdVector_push(&links, 1);
	LinkIdVector_push(&links, 2);
	LinkIdVector_push(&links, 3);

	Stream st = CS_from(&sg, &nodes, &links, 0, 100);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_set(st.stream);
	FOR_EACH_NODE(nodes_iter, node_id) {
		LinksIterator neighbours_iter = funcs.neighbours_of_node(st.stream, node_id);
		printf("NODE %zu\n", node_id);
		FOR_EACH_LINK(neighbours_iter, link_id) {
			printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0],
				   sg.links.links[link_id].nodes[1]);
		}
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdVector_destroy(nodes);
	LinkIdVector_destroy(links);
	return true;
}

bool test_times_node_present_chunk_stream() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	NodeIdVector nodes = NodeIdVector_with_capacity(3);
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 1);
	NodeIdVector_push(&nodes, 3);

	LinkIdVector links = LinkIdVector_with_capacity(4);
	LinkIdVector_push(&links, 0);
	LinkIdVector_push(&links, 1);
	LinkIdVector_push(&links, 2);
	LinkIdVector_push(&links, 3);

	Stream st = CS_from(&sg, &nodes, &links, 20, 80);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_set(st.stream);
	FOR_EACH_NODE(nodes_iter, node_id) {
		TimesIterator times_iter = funcs.times_node_present(st.stream, node_id);
		printf("NODE %zu : ", node_id);
		FOR_EACH_TIME(times_iter, interval) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdVector_destroy(nodes);
	LinkIdVector_destroy(links);
	return true;
}

bool test_times_node_present_chunk_stream_2() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	NodeIdVector nodes = NodeIdVector_with_capacity(3);
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 1);
	NodeIdVector_push(&nodes, 3);

	LinkIdVector links = LinkIdVector_with_capacity(4);
	LinkIdVector_push(&links, 0);
	LinkIdVector_push(&links, 1);
	LinkIdVector_push(&links, 2);
	LinkIdVector_push(&links, 3);

	Stream st = CS_from(&sg, &nodes, &links, 40, 45);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_set(st.stream);
	FOR_EACH_NODE(nodes_iter, node_id) {
		TimesIterator times_iter = funcs.times_node_present(st.stream, node_id);
		printf("NODE %zu : ", node_id);
		FOR_EACH_TIME(times_iter, interval) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdVector_destroy(nodes);
	LinkIdVector_destroy(links);
	return true;
}

bool test_link_presence_chunk_stream() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	NodeIdVector nodes = NodeIdVector_with_capacity(3);
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 1);
	NodeIdVector_push(&nodes, 3);

	LinkIdVector links = LinkIdVector_with_capacity(4);
	LinkIdVector_push(&links, 0);
	LinkIdVector_push(&links, 1);
	LinkIdVector_push(&links, 2);
	LinkIdVector_push(&links, 3);

	Stream st = CS_from(&sg, &nodes, &links, 20, 80);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	LinksIterator links_iter = funcs.links_set(st.stream);
	FOR_EACH_LINK(links_iter, link_id) {
		TimesIterator times_iter = funcs.times_link_present(st.stream, link_id);
		printf("LINK %zu : ", link_id);
		FOR_EACH_TIME(times_iter, interval) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	CS_destroy(st);
	StreamGraph_destroy(sg);
	NodeIdVector_destroy(nodes);
	LinkIdVector_destroy(links);
	return true;
}

bool test_nodes_and_links_present_at_t_chunk_stream() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	NodeIdVector nodes = NodeIdVector_with_capacity(3);
	NodeIdVector_push(&nodes, 0);
	NodeIdVector_push(&nodes, 1);
	NodeIdVector_push(&nodes, 3);

	LinkIdVector links = LinkIdVector_with_capacity(4);
	LinkIdVector_push(&links, 0);
	LinkIdVector_push(&links, 1);
	LinkIdVector_push(&links, 2);
	LinkIdVector_push(&links, 3);

	Stream st = CS_from(&sg, &nodes, &links, 20, 80);
	StreamFunctions funcs = STREAM_FUNCS(funcs, st);
	NodesIterator nodes_iter = funcs.nodes_present_at_t(st.stream, 40);
	printf("STUFF PRESENT AT 40\n");
	FOR_EACH_NODE(nodes_iter, node_id) {
		printf("NODE %zu : ", node_id);
		TimesIterator times_iter = funcs.times_node_present(st.stream, node_id);
		FOR_EACH_TIME(times_iter, interval) {
			printf("[%lu, %lu] U ", interval.start, interval.end);
		}
		printf("\n");
	}

	LinksIterator links_iter = funcs.links_present_at_t(st.stream, 40);
	FOR_EACH_LINK(links_iter, link_id) {
		printf("LINK %zu (%zu, %zu)\n", link_id, sg.links.links[link_id].nodes[0], sg.links.links[link_id].nodes[1]);
	}

	CS_destroy(st);
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	NodeIdVector_destroy(nodes);
	LinkIdVector_destroy(links);
	return true;
}

bool test_degree_of_node() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	Stream st = FSG_from(&sg);
	double degree_a = Stream_degree_of_node(st, 0);
	StreamGraph_destroy(sg);
	FSG_destroy(st);
	return EXPECT_F_APPROX_EQ(degree_a, 0.6, 1e-2);
}

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
		&(Test){"cardinal_of_T_S_0",						 test_cardinal_of_T_S_0						   },
		&(Test){"cardinal_of_T_S_1",						 test_cardinal_of_T_S_1						   },
		&(Test){"cardinal_of_T_S_2",						 test_cardinal_of_T_S_2						   },
		&(Test){"cardinal_of_T_S_3",						 test_cardinal_of_T_S_3						   },
		&(Test){"cardinal_of_W_S",						   test_cardinal_of_W_S						   },
 //&(Test){"times_node_present", test_times_node_present},
		&(Test){"cardinal_of_W_L",						   test_cardinal_of_W_L						   },
		&(Test){"coverage_S",								test_coverage_S								 },
		&(Test){"coverage_L",								test_coverage_L								 },

		&(Test){"number_of_nodes",						   test_number_of_nodes						   },
		&(Test){"number_of_links",						   test_number_of_links						   },
		&(Test){"node_duration",							 test_node_duration							   },
		&(Test){"link_duration",							 test_link_duration							   },
		&(Test){"contribution_of_nodes",					 test_contribution_of_nodes					   },
		&(Test){"contributions_of_links",					  test_contributions_of_links					 },
		&(Test){"uniformity",								test_uniformity								 },

		&(Test){"density",								   test_density								   },
		&(Test){"density_of_link",						   test_density_of_link						   },
		&(Test){"density_of_node",						   test_density_of_node						   },
		&(Test){"density_of_time",						   test_density_of_time						   },

		&(Test){"chunk_stream_nodes_set",					  test_chunk_stream_nodes_set					 },
		&(Test){"neighbours_of_node_chunk_stream",		   test_neighbours_of_node_chunk_stream		   },
		&(Test){"times_node_present_chunk_stream",		   test_times_node_present_chunk_stream		   },
		&(Test){"times_node_present_chunk_stream_2",		 test_times_node_present_chunk_stream_2		   },
		&(Test){"link_presence_chunk_stream",				  test_link_presence_chunk_stream				 },
		&(Test){"nodes_and_links_present_at_t_chunk_stream", test_nodes_and_links_present_at_t_chunk_stream},

		NULL,
	};

	return test("Metrics", tests);
}