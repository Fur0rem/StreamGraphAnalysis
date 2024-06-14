#include "../src/metrics.h"
#include "test.h"

#define TEST_METRIC_F(name, value, graph)                                                                              \
	bool test_##name() {                                                                                               \
		StreamGraph sg = StreamGraph_from_file("tests/test_data/" #graph ".txt");                                      \
		bool result = EXPECT_F_APPROX_EQ(name(&sg), value, 1e-2);                                                      \
		StreamGraph_destroy(&sg);                                                                                      \
		return result;                                                                                                 \
	}

#define TEST_METRIC_I(name, value, graph)                                                                              \
	bool test_##name() {                                                                                               \
		StreamGraph sg = StreamGraph_from_file("tests/test_data/" #graph ".txt");                                      \
		bool result = EXPECT_EQ(name(&sg), value);                                                                     \
		StreamGraph_destroy(&sg);                                                                                      \
		return result;                                                                                                 \
	}

TEST_METRIC_F(coverage, 0.65, S)
TEST_METRIC_I(number_of_nodes, 4, S)
TEST_METRIC_F(number_of_temporal_nodes, 2.6, S)
TEST_METRIC_F(number_of_links, 1.0, S)
TEST_METRIC_F(node_duration, 6.5, S)
TEST_METRIC_F(link_duration, 1.66666666666, S)

bool test_contribution_of_nodes() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	double contribution_a = contribution_of_node(&sg, 0);
	double contribution_b = contribution_of_node(&sg, 1);
	double contribution_c = contribution_of_node(&sg, 2);
	double contribution_d = contribution_of_node(&sg, 3);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(contribution_a, 1.0, 1e-2) && EXPECT_F_APPROX_EQ(contribution_b, 0.9, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_c, 0.5, 1e-2) && EXPECT_F_APPROX_EQ(contribution_d, 0.2, 1e-2);
}

bool test_contributions_of_links() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	double contribution_ab = contribution_of_link(&sg, 0);
	double contribution_bd = contribution_of_link(&sg, 1);
	double contribution_ac = contribution_of_link(&sg, 2);
	double contribution_bc = contribution_of_link(&sg, 3);
	StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(contribution_ab, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bd, 0.1, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_ac, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bc, 0.3, 1e-2);
}

TEST_METRIC_F(uniformity, 22.0 / 56.0, S)
TEST_METRIC_F(density, 10.0 / 22.0, S)
TEST_METRIC_F(compactness, 26.0 / 40.0, S)

int main() {
	Test* tests[] = {
		&(Test){"coverage",					test_coverage				 },
		&(Test){"number_of_nodes",		   test_number_of_nodes		   },
		&(Test){"number_of_temporal_nodes", test_number_of_temporal_nodes},
		&(Test){"node_duration",			 test_node_duration		   },
		&(Test){"link_duration",			 test_link_duration		   },
		&(Test){"number_of_links",		   test_number_of_links		   },
		&(Test){"contribution_of_nodes",	 test_contribution_of_nodes   },
		&(Test){"contributions_of_links",	  test_contributions_of_links	 },
		&(Test){"uniformity",				  test_uniformity				 },
		&(Test){"density",				   test_density				   },
		&(Test){"compactness",			   test_compactness			   },
		(Test*)NULL
	   };

	return test("Metrics", tests);
}