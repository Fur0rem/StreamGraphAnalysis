/**
 * @file tests/metrics.c
 * @brief Tests regarding metrics
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool test_cardinal_of_T_S_0() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");

	SGA_Stream st	      = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, &st);

	SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, 0);
	size_t cardinal		     = SGA_total_time_of(times_iter);

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 100);
}
bool test_cardinal_of_T_S_1() {
	SGA_StreamGraph sg	     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st		     = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs	     = STREAM_FUNCS(funcs, &st);
	SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, 1);
	size_t cardinal		     = SGA_total_time_of(times_iter);
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 90);
}

bool test_cardinal_of_T_S_2() {
	SGA_StreamGraph sg	     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st		     = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs	     = STREAM_FUNCS(funcs, &st);
	SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, 2);
	size_t cardinal		     = SGA_total_time_of(times_iter);
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 50);
}

bool test_cardinal_of_T_S_3() {
	SGA_StreamGraph sg	     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st		     = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs	     = STREAM_FUNCS(funcs, &st);
	SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, 3);
	size_t cardinal		     = SGA_total_time_of(times_iter);
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 20);
}

bool test_times_node_present() {
	SGA_StreamGraph sg	     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st		     = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs	     = STREAM_FUNCS(funcs, &st);
	SGA_TimesIterator times_iter = funcs.times_node_present(st.stream_data, 3);
	printf("Present from : \n");
	SGA_FOR_EACH_TIME(interval, times_iter) {
		printf("[%lu, %lu] U ", interval.start, interval.end);
	}
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return true;
}

bool test_cardinal_of_W_S() {
	SGA_StreamGraph sg	     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st		     = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs	     = STREAM_FUNCS(funcs, &st);
	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	size_t cardinal		     = 0;
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		printf("NODE %zu\n", node_id);
		size_t before = cardinal;
		// for each interval of presence of the node
		SGA_Node node = sg.nodes.nodes[node_id];
		// for each interval of presence of the node
		for (size_t i = 0; i < node.presence.nb_intervals; i++) {
			printf("Interval %zu : [%lu, %lu]\n", i, node.presence.intervals[i].start, node.presence.intervals[i].end);
		}
		cardinal += SGA_total_time_of(funcs.times_node_present(st.stream_data, node_id));
		printf("Cardinal : %zu\n", cardinal - before);
	}
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 260);
}

bool test_cardinal_of_W_L() {
	SGA_StreamGraph sg	     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st		     = SGA_LinkStream_from(&sg);
	StreamFunctions funcs	     = STREAM_FUNCS(funcs, &st);
	SGA_NodesIterator nodes_iter = funcs.nodes_set(st.stream_data);
	size_t cardinal		     = 0;
	SGA_FOR_EACH_NODE(node_id, nodes_iter) {
		cardinal += SGA_total_time_of(funcs.times_node_present(st.stream_data, node_id));
	}
	SGA_LinkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_EQ(cardinal, 400);
}

bool test_coverage_S() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	double coverage	   = SGA_Stream_coverage(&st);
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_F_APPROX_EQ(coverage, 0.65, 1e-6);
}

bool test_coverage_L() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream ls	   = SGA_LinkStream_from(&sg);
	double coverage	   = SGA_Stream_coverage(&ls);
	SGA_StreamGraph_destroy(sg);
	SGA_LinkStream_destroy(ls);
	return EXPECT_F_APPROX_EQ(coverage, 1.0, 1e-6);
}

bool test_node_duration_S() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	double duration	   = SGA_Stream_node_duration(&st);
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return EXPECT_F_APPROX_EQ(duration, 6.5, 1e-6);
}

#define TEST_METRIC_F(name, value, graph, type)                                                                                            \
	bool test_##name() {                                                                                                               \
		SGA_StreamGraph sg = SGA_StreamGraph_from_external("data/" #graph ".txt");                                                 \
		SGA_Stream st	   = type##_from(&sg);                                                                                     \
		bool result	   = EXPECT_F_APPROX_EQ(SGA_Stream_##name(&st), value, 1e-5);                                              \
		SGA_StreamGraph_destroy(sg);                                                                                               \
		type##_destroy(st);                                                                                                        \
		return result;                                                                                                             \
	}

#define TEST_METRIC_I(name, value, graph, type)                                                                                            \
	bool test_##name() {                                                                                                               \
		SGA_StreamGraph sg = SGA_StreamGraph_from_external("data/" #graph ".txt");                                                 \
		SGA_Stream st	   = type##_from(&sg);                                                                                     \
		bool result	   = EXPECT_EQ(SGA_Stream_##name(st), value);                                                              \
		SGA_StreamGraph_destroy(sg);                                                                                               \
		type##_destroy(st);                                                                                                        \
		return result;                                                                                                             \
	}

TEST_METRIC_F(coverage, 0.65, S_external, SGA_FullStreamGraph)
TEST_METRIC_F(number_of_nodes, 2.6, S_external, SGA_FullStreamGraph)
TEST_METRIC_F(number_of_links, 1.0, S_external, SGA_FullStreamGraph)
TEST_METRIC_F(node_duration, 6.5, S_external, SGA_FullStreamGraph)
TEST_METRIC_F(link_duration, 1.66666666666, S_external, SGA_FullStreamGraph)

bool test_contribution_of_nodes() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	      = SGA_FullStreamGraph_from(&sg);
	double contribution_a = SGA_Stream_contribution_of_node(&st, 0);
	double contribution_b = SGA_Stream_contribution_of_node(&st, 1);
	double contribution_c = SGA_Stream_contribution_of_node(&st, 2);
	double contribution_d = SGA_Stream_contribution_of_node(&st, 3);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return EXPECT_F_APPROX_EQ(contribution_a, 1.0, 1e-2) && EXPECT_F_APPROX_EQ(contribution_b, 0.9, 1e-2) &&
	       EXPECT_F_APPROX_EQ(contribution_c, 0.5, 1e-2) && EXPECT_F_APPROX_EQ(contribution_d, 0.2, 1e-2);
}

bool test_contributions_of_links() {
	SGA_StreamGraph sg     = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	       = SGA_FullStreamGraph_from(&sg);
	double contribution_ab = SGA_Stream_contribution_of_link(&st, 0);
	double contribution_bd = SGA_Stream_contribution_of_link(&st, 1);
	double contribution_ac = SGA_Stream_contribution_of_link(&st, 2);
	double contribution_bc = SGA_Stream_contribution_of_link(&st, 3);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return EXPECT_F_APPROX_EQ(contribution_ab, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bd, 0.1, 1e-2) &&
	       EXPECT_F_APPROX_EQ(contribution_ac, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bc, 0.3, 1e-2);
}

TEST_METRIC_F(uniformity, 22.0 / 56.0, S_external, SGA_FullStreamGraph)
TEST_METRIC_F(density, 10.0 / 22.0, S_external, SGA_FullStreamGraph)

bool test_density_linkstream() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_external("data/L.txt");
	SGA_Stream ls	   = SGA_LinkStream_from(&sg);
	SGA_Stream fsg	   = SGA_FullStreamGraph_from(&sg);
	double density_ls  = SGA_Stream_density(&ls);
	double density_fsg = SGA_Stream_density(&fsg);
	SGA_StreamGraph_destroy(sg);
	SGA_LinkStream_destroy(ls);
	SGA_FullStreamGraph_destroy(fsg);
	printf("Density LS : %f\n", density_ls);
	printf("Density FSG : %f\n", density_fsg);
	return true;
}

bool test_density_of_link() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	double density_ab  = SGA_Stream_density_of_link(&st, 0);
	double density_bd  = SGA_Stream_density_of_link(&st, 1);
	double density_ac  = SGA_Stream_density_of_link(&st, 2);
	double density_bc  = SGA_Stream_density_of_link(&st, 3);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return EXPECT_F_APPROX_EQ(density_ab, 1.0 / 3.0, 1e-2) && EXPECT_F_APPROX_EQ(density_bd, 1.0 / 2.0, 1e-2);
}

bool test_density_of_node() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	// SGA_FullStreamGraph* fsg     = malloc(sizeof(FullStreamGraph));
	// fsg->underlying_stream_graph = &sg;
	// SGA_Stream* st		     = malloc(sizeof(SGA_Stream));
	// st->type		     = FULL_STREAM_GRAPH;
	// st->stream_data		     = fsg;
	double density_d = SGA_Stream_density_of_node(&st, 3);
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	// free(st);
	//	free(fsg);
	return EXPECT_F_APPROX_EQ(density_d, 1.0 / 4.0, 1e-2);
}

bool test_density_at_instant() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	double density_2   = SGA_Stream_density_at_instant(&st, 20); // TODO : add automatic traduction with scaling
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return EXPECT_F_APPROX_EQ(density_2, 2.0 / 3.0, 1e-2);
}

bool test_degree_of_node() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	double degree_a	   = SGA_Stream_degree_of_node(&st, 0);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return EXPECT_F_APPROX_EQ(degree_a, 0.6, 1e-2);
}

bool test_cache() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_file("data/S.txt");
	SGA_Stream st	      = SGA_FullStreamGraph_from(&sg);
	StreamFunctions funcs = STREAM_FUNCS(funcs, &st);
	size_t t	      = SGA_Stream_duration(&st);
	bool result	      = st.cache.duration.present && st.cache.duration.data == t;

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);

	return result;
}

bool test_clustering_coeff_of_node() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_external("data/Figure_8.txt");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	double clustering_coeff_c = SGA_Stream_clustering_coeff_of_node(&st, 2);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return EXPECT_F_APPROX_EQ(clustering_coeff_c, 3.0 / 5.0, 1e-6);
}

bool test_compactness() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_external("data/S.txt");

	SGA_Stream fsg	   = SGA_FullStreamGraph_from(&sg);
	double compactness = SGA_Stream_compactness(&fsg);
	bool result	   = EXPECT_F_APPROX_EQ(compactness, 26.0 / 40.0, 1e-6);

	SGA_Stream ls = SGA_LinkStream_from(&sg);
	compactness   = SGA_Stream_compactness(&ls);
	result &= EXPECT_F_APPROX_EQ(compactness, 1.0, 1e-6);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(fsg);
	SGA_LinkStream_destroy(ls);
	return result;
}

TEST_METRIC_F(transitivity_ratio, 9.0 / 17.0, Figure_8, SGA_FullStreamGraph)

int main() {
	Test* tests[] = {
	    TEST(test_cardinal_of_T_S_0),
	    TEST(test_cardinal_of_T_S_1),
	    TEST(test_cardinal_of_T_S_2),
	    TEST(test_cardinal_of_T_S_3),
	    TEST(test_cardinal_of_W_S),
	    TEST(test_cardinal_of_W_L),
	    TEST(test_coverage_S),
	    TEST(test_coverage_L),
	    TEST(test_number_of_nodes),
	    TEST(test_number_of_links),
	    TEST(test_node_duration),
	    TEST(test_link_duration),
	    TEST(test_contribution_of_nodes),
	    TEST(test_contributions_of_links),
	    TEST(test_uniformity),
	    TEST(test_density),
	    TEST(test_density_of_link),
	    TEST(test_density_of_node),
	    TEST(test_density_at_instant),
	    TEST(test_degree_of_node),
	    TEST(test_cache),
	    TEST(test_clustering_coeff_of_node),
	    TEST(test_transitivity_ratio),
	    TEST(test_density_linkstream),
	    NULL,
	};

	return test("Metrics", tests);
}