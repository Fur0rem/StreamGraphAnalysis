/**
 * @file tests/weighted_streams.c
 * @brief Tests regarding the different weighted streams
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_full_stream_graph() {
	bool result = true;

	SGA_W_StreamGraph wsg = SGA_W_StreamGraph_from_file("data/tests/weighted.sga");
	SGA_W_Stream stream   = SGA_W_FullStreamGraph_from(&wsg);

	// Check node weights
	SGA_Interval lifespan = SGA_Stream_lifespan(SGA_as_unweighted(&stream));
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&wsg.base);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_Stream_node_weight_at_t(&stream, node, t);
			result &= EXPECT_F_APPROX_EQ(w, 9.0, 1e-6);
		}
	}

	// Check some link weights
	SGA_LinkId link_0_1 = SGA_Stream_link_between_nodes(SGA_as_unweighted(&stream), 0, 1);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 1), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 2), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 3), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 4), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 5), 0.5, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 6), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 9), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 10), 0.0, 1e-6);

	SGA_LinkId link_1_2 = SGA_StreamGraph_link_between_nodes(&wsg.base, 1, 2);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 1), 17.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 2), 16.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 4), 14.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 7), 11.0, 1e-6);

	SGA_W_StreamGraph_destroy(wsg);
	SGA_W_Stream_destroy(stream);

	return result;
}

bool test_link_stream() {
	bool result = true;

	SGA_W_StreamGraph wsg = SGA_W_StreamGraph_from_file("data/tests/weighted.sga");
	SGA_W_Stream stream   = SGA_W_LinkStream_from(&wsg, SGA_WeightFunc_const_universally(1.0));
	SGA_Interval lifespan = SGA_Stream_lifespan(SGA_as_unweighted(&stream));

	// Check node weights
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&wsg.base);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_Stream_node_weight_at_t(&stream, node, t);
			if (SGA_Stream_is_node_present_at(SGA_as_unweighted(&stream), node, t)) {
				result &= EXPECT_F_APPROX_EQ(w, 9.0, 1e-6);
			}
			else {
				result &= EXPECT_F_APPROX_EQ(w, 1.0, 1e-6);
			}
		}
	}

	// Check some link weights
	SGA_LinkId link_0_1 = SGA_StreamGraph_link_between_nodes(&wsg.base, 0, 1);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 1), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 2), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 3), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 4), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 5), 0.5, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 6), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 9), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 10), 0.0, 1e-6);

	SGA_LinkId link_1_2 = SGA_StreamGraph_link_between_nodes(&wsg.base, 1, 2);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 1), 17.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 2), 16.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 4), 14.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 7), 11.0, 1e-6);

	SGA_W_StreamGraph_destroy(wsg);
	SGA_W_Stream_destroy(stream);
	return result;
}

bool test_timeframe_stream() {
	bool result = true;

	SGA_W_StreamGraph wsg = SGA_W_StreamGraph_from_file("data/tests/weighted.sga");
	SGA_W_Stream stream   = SGA_W_TimeFrameStream_from(&wsg, SGA_Interval_from(1, 9));

	// Check node weights
	SGA_Interval lifespan = SGA_Stream_lifespan(SGA_as_unweighted(&stream));
	result &= EXPECT(lifespan.start == 1);
	result &= EXPECT(lifespan.end == 9);
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&wsg.base);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_Stream_node_weight_at_t(&stream, node, t);
			result &= EXPECT_F_APPROX_EQ(w, 9.0, 1e-6);
		}
	}

	// Check some link weights
	SGA_LinkId link_0_1 = SGA_StreamGraph_link_between_nodes(&wsg.base, 0, 1);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 1), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 2), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 3), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 4), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 5), 0.5, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 6), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 9), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 10), 0.0, 1e-6);

	SGA_LinkId link_1_2 = SGA_StreamGraph_link_between_nodes(&wsg.base, 1, 2);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 1), 17.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 2), 16.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 4), 14.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 7), 11.0, 1e-6);

	SGA_W_StreamGraph_destroy(wsg);
	SGA_W_Stream_destroy(stream);
	return result;
}

bool test_chunk_stream() {
	bool result = true;

	SGA_W_StreamGraph wsg		     = SGA_W_StreamGraph_from_file("data/tests/weighted.sga");
	SGA_Interval chunk_interval	     = SGA_Interval_from(5, 10);
	SGA_NodeIdArrayList nodes_to_include = SGA_NodeIdArrayList_from(2, 0, 1);
	SGA_LinkIdArrayList links_to_include = SGA_LinkIdArrayList_from(2, 0, 1);
	SGA_W_Stream stream		     = SGA_W_ChunkStream_with(&wsg, &nodes_to_include, &links_to_include, chunk_interval);
	SGA_NodeIdArrayList_destroy(nodes_to_include);
	SGA_LinkIdArrayList_destroy(links_to_include);

	result &= EXPECT(SGA_Stream_distinct_cardinal_of_node_set(SGA_as_unweighted(&stream)) == 2);
	result &= EXPECT(SGA_Stream_distinct_cardinal_of_link_set(SGA_as_unweighted(&stream)) ==
			 1); // 1-2 got deleted because node 2 is not included

	// Check node weights
	SGA_Interval lifespan = SGA_Stream_lifespan(SGA_as_unweighted(&stream));
	result &= EXPECT(lifespan.start == 5);
	result &= EXPECT(lifespan.end == 10);
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&wsg.base);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_Stream_node_weight_at_t(&stream, node, t);
			result &= EXPECT_F_APPROX_EQ(w, 9.0, 1e-6);
		}
	}

	// Check some link weights
	SGA_LinkId link_0_1 = SGA_StreamGraph_link_between_nodes(&wsg.base, 0, 1);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 5), 0.5, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 6), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 9), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 10), 0.0, 1e-6);

	// Check some metrics
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_max_link_weight(&stream), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_min_link_weight(&stream), -0.0, 1e-6);

	SGA_W_StreamGraph_destroy(wsg);
	SGA_W_Stream_destroy(stream);
	return result;
}

bool test_chunk_stream_small() {
	bool result = true;

	SGA_W_StreamGraph wsg		     = SGA_W_StreamGraph_from_file("data/tests/weighted.sga");
	SGA_Interval chunk_interval	     = SGA_Interval_from(5, 10);
	SGA_NodeIdArrayList nodes_to_include = SGA_NodeIdArrayList_from(2, 0, 1);
	SGA_LinkIdArrayList links_to_include = SGA_LinkIdArrayList_from(2, 0, 1);
	SGA_W_Stream stream		     = SGA_W_ChunkStream_with(&wsg, &nodes_to_include, &links_to_include, chunk_interval);
	SGA_NodeIdArrayList_destroy(nodes_to_include);
	SGA_LinkIdArrayList_destroy(links_to_include);

	result &= EXPECT(SGA_Stream_distinct_cardinal_of_node_set(SGA_as_unweighted(&stream)) == 2);
	result &= EXPECT(SGA_Stream_distinct_cardinal_of_link_set(SGA_as_unweighted(&stream)) ==
			 1); // 1-2 got deleted because node 2 is not included

	// Check node weights
	SGA_Interval lifespan = SGA_Stream_lifespan(SGA_as_unweighted(&stream));
	result &= EXPECT(lifespan.start == 5);
	result &= EXPECT(lifespan.end == 10);
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&wsg.base);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_Stream_node_weight_at_t(&stream, node, t);
			result &= EXPECT_F_APPROX_EQ(w, 9.0, 1e-6);
		}
	}

	// Check some link weights
	SGA_LinkId link_0_1 = SGA_StreamGraph_link_between_nodes(&wsg.base, 0, 1);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 5), 0.5, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 6), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 9), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 10), 0.0, 1e-6);

	// Check some metrics
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_max_link_weight(&stream), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_min_link_weight(&stream), -0.0, 1e-6);

	SGA_W_StreamGraph_destroy(wsg);
	SGA_W_Stream_destroy(stream);
	return result;
}

bool test_delta_stream() {
	bool result = true;

	SGA_W_StreamGraph wsg = SGA_W_StreamGraph_from_file("data/tests/weighted.sga");
	SGA_W_Stream stream   = SGA_W_DeltaStream_from(&wsg, 2);

	SGA_Interval lifespan = SGA_Stream_lifespan(SGA_as_unweighted(&stream));
	result &= EXPECT(lifespan.start == 0);
	result &= EXPECT(lifespan.end == 10);
	result &= EXPECT(SGA_Stream_is_node_present_at(SGA_as_unweighted(&stream), 2, 0));

	// Check node weights
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&wsg.base);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_Stream_node_weight_at_t(&stream, node, t);
			result &= EXPECT_F_APPROX_EQ(w, 9.0, 1e-6);
		}
	}

	// Check some link weights
	SGA_LinkId link_0_1 = SGA_StreamGraph_link_between_nodes(&wsg.base, 0, 1);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 0), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 1), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 3), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 4), -1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 6), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 7), 1.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 8), 0.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_0_1, 9), 0.0, 1e-6);

	SGA_LinkId link_1_2 = SGA_StreamGraph_link_between_nodes(&wsg.base, 1, 2);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 0), 17.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 2), 16.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 5), 13.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 7), 11.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 8), 11.0, 1e-6);
	result &= EXPECT_F_APPROX_EQ(SGA_W_Stream_link_weight_at_t(&stream, link_1_2, 9), 11.0, 1e-6);

	SGA_W_StreamGraph_destroy(wsg);
	SGA_W_Stream_destroy(stream);
	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_full_stream_graph),
	    TEST(test_link_stream),
	    TEST(test_timeframe_stream),
	    TEST(test_chunk_stream),
	    TEST(test_chunk_stream_small),
	    TEST(test_delta_stream),
	    NULL,
	};

	return test("Weighted streams", tests);
}