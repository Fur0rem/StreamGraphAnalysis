#include "../src/measures.h"
#include "test.h"

#define TEST_MEASURE_F(name, value, graph)                                                                             \
	bool test_##name() {                                                                                               \
		StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/" #graph ".txt");                                  \
		bool result = EXPECT_F_APPROX_EQ(SGA_##name(&sg), value, 1e-2);                                                \
		SGA_StreamGraph_destroy(&sg);                                                                                  \
		return result;                                                                                                 \
	}

#define TEST_MEASURE_I(name, value, graph)                                                                             \
	bool test_##name() {                                                                                               \
		StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/" #graph ".txt");                                  \
		bool result = EXPECT_EQ(SGA_##name(&sg), value);                                                               \
		SGA_StreamGraph_destroy(&sg);                                                                                  \
		return result;                                                                                                 \
	}

TEST_MEASURE_F(coverage, 0.65, S)
TEST_MEASURE_I(number_of_nodes, 4, S)
TEST_MEASURE_F(number_of_temporal_nodes, 2.6, S)
TEST_MEASURE_F(number_of_links, 1.0, S)
TEST_MEASURE_F(node_duration, 6.5, S)
TEST_MEASURE_F(link_duration, 1.66666666666, S)

bool test_contribution_of_nodes() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	double contribution_a = SGA_contribution_of_node(&sg, 0);
	double contribution_b = SGA_contribution_of_node(&sg, 1);
	double contribution_c = SGA_contribution_of_node(&sg, 2);
	double contribution_d = SGA_contribution_of_node(&sg, 3);
	SGA_StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(contribution_a, 1.0, 1e-2) && EXPECT_F_APPROX_EQ(contribution_b, 0.9, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_c, 0.5, 1e-2) && EXPECT_F_APPROX_EQ(contribution_d, 0.2, 1e-2);
}

bool test_contributions_of_links() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	double contribution_ab = SGA_contribution_of_link(&sg, 0);
	double contribution_bd = SGA_contribution_of_link(&sg, 1);
	double contribution_ac = SGA_contribution_of_link(&sg, 2);
	double contribution_bc = SGA_contribution_of_link(&sg, 3);
	SGA_StreamGraph_destroy(&sg);
	return EXPECT_F_APPROX_EQ(contribution_ab, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bd, 0.1, 1e-2) &&
		   EXPECT_F_APPROX_EQ(contribution_ac, 0.3, 1e-2) && EXPECT_F_APPROX_EQ(contribution_bc, 0.3, 1e-2);
}

TEST_MEASURE_F(uniformity, 22.0 / 56.0, S)
TEST_MEASURE_F(density, 10.0 / 22.0, S)

int main() {
	return test("StreamGraph", &(Test){"coverage", test_coverage}, &(Test){"number_of_nodes", test_number_of_nodes},
				&(Test){"number_of_temporal_nodes", test_number_of_temporal_nodes},
				&(Test){"node_duration", test_node_duration}, &(Test){"link_duration", test_link_duration},
				&(Test){"number_of_links", test_number_of_links},
				&(Test){"contribution_of_nodes", test_contribution_of_nodes},
				&(Test){"contributions_of_links", test_contributions_of_links}, &(Test){"uniformity", test_uniformity},
				&(Test){"density", test_density}, NULL)
			   ? 0
			   : 1;
}