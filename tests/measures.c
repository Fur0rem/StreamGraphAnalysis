#include "../src/measures.h"
#include "test.h"

#define TEST_MEASURE_F(name, value, graph, eps)                                                    \
	bool test_##name() {                                                                           \
		StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/" #graph ".txt");              \
		bool result = EXPECT_F_APPROX_EQ(SGA_##name(&sg), value, eps);                             \
		SGA_StreamGraph_destroy(&sg);                                                              \
		return result;                                                                             \
	}

#define TEST_MEASURE_I(name, value, graph)                                                         \
	bool test_##name() {                                                                           \
		StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/" #graph ".txt");              \
		bool result = EXPECT_EQ(SGA_##name(&sg), value);                                           \
		SGA_StreamGraph_destroy(&sg);                                                              \
		return result;                                                                             \
	}

TEST_MEASURE_F(coverage, 0.65, S_high_precision, 1e-2)
TEST_MEASURE_I(number_of_nodes, 4, S_high_precision)
TEST_MEASURE_F(number_of_temporal_nodes, 2.6, S_high_precision, 1e-1)
TEST_MEASURE_F(number_of_links, 1.0, S_high_precision, 1e-2)
TEST_MEASURE_F(node_duration, 6.5, S_high_precision, 1e-1)
TEST_MEASURE_F(link_duration, 1.66, S_high_precision, 1e-2)

int main() {
	return test("StreamGraph", &(Test){"coverage", test_coverage},
				&(Test){"number_of_nodes", test_number_of_nodes},
				&(Test){"number_of_temporal_nodes", test_number_of_temporal_nodes},
				&(Test){"node_duration", test_node_duration},
				&(Test){"link_duration", test_link_duration},
				&(Test){"number_of_links", test_number_of_links}, NULL)
			   ? 0
			   : 1;
}