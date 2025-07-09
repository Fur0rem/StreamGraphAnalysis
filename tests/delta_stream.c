/**
 * @file tests/link_stream.c
 * @brief Tests regarding LinkStream
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_same_distinct_nodes() {
	SGA_StreamGraph original = SGA_StreamGraph_from_file("data/tests/delta_stream/original.sga");
	SGA_Stream delta_stream	 = SGA_DeltaStream_from(&original, 2);

	SGA_StreamGraph _expected = SGA_StreamGraph_from_file("data/tests/delta_stream/expected_2delta_stream.sga");
	SGA_Stream expected	  = SGA_FullStreamGraph_from(&_expected);

	SGA_NodesIterator _original_nodes = SGA_Stream_nodes_set(&delta_stream);
	SGA_NodesIterator _delta_nodes	  = SGA_Stream_nodes_set(&expected);

	SGA_NodeIdArrayList original_nodes = SGA_collect_node_ids(_original_nodes);
	SGA_NodeIdArrayList delta_nodes	   = SGA_collect_node_ids(_delta_nodes);

	bool same_nodes = EXPECT(SGA_NodeIdArrayList_equals(&original_nodes, &delta_nodes));
	SGA_NodeIdArrayList_destroy(original_nodes);
	SGA_NodeIdArrayList_destroy(delta_nodes);
	SGA_StreamGraph_destroy(original);
	SGA_StreamGraph_destroy(_expected);
	SGA_Stream_destroy(delta_stream);
	SGA_Stream_destroy(expected);
	return same_nodes;
}

bool test_same_temporal_nodes() {
	SGA_StreamGraph original = SGA_StreamGraph_from_file("data/tests/delta_stream/original.sga");
	SGA_Stream delta_stream	 = SGA_DeltaStream_from(&original, 2);

	SGA_StreamGraph _expected = SGA_StreamGraph_from_file("data/tests/delta_stream/expected_2delta_stream.sga");
	SGA_Stream expected	  = SGA_FullStreamGraph_from(&_expected);

	bool result = true;
	for (size_t node = 0; node < original.nodes.nb_nodes; node++) {
		SGA_TimesIterator _original_times = SGA_Stream_times_node_present(&delta_stream, node);
		SGA_TimesIterator _expected_times = SGA_Stream_times_node_present(&expected, node);

		SGA_IntervalArrayList original_times = SGA_collect_times(_original_times);
		SGA_IntervalArrayList expected_times = SGA_collect_times(_expected_times);

		bool same_times = EXPECT(SGA_IntervalArrayList_equals(&original_times, &expected_times));
		SGA_IntervalArrayList_destroy(original_times);
		SGA_IntervalArrayList_destroy(expected_times);

		result &= same_times;
	}

	SGA_StreamGraph_destroy(original);
	SGA_StreamGraph_destroy(_expected);
	SGA_Stream_destroy(delta_stream);
	SGA_Stream_destroy(expected);
	return result;
}

bool test_isomorphism() {
	SGA_StreamGraph original = SGA_StreamGraph_from_file("data/tests/delta_stream/original.sga");
	SGA_Stream delta_stream	 = SGA_DeltaStream_from(&original, 2);

	SGA_StreamGraph _expected = SGA_StreamGraph_from_file("data/tests/delta_stream/expected_2delta_stream.sga");
	SGA_Stream expected	  = SGA_FullStreamGraph_from(&_expected);

	bool isomorphic = EXPECT(SGA_Stream_are_isomorphic(&delta_stream, &expected));
	SGA_StreamGraph_destroy(original);
	SGA_StreamGraph_destroy(_expected);
	SGA_Stream_destroy(delta_stream);
	SGA_Stream_destroy(expected);

	return isomorphic;
}

bool test_present_at_t() {
	SGA_StreamGraph original = SGA_StreamGraph_from_file("data/tests/delta_stream/original.sga");
	SGA_Stream delta_stream	 = SGA_DeltaStream_from(&original, 2);

	SGA_StreamGraph _expected = SGA_StreamGraph_from_file("data/tests/delta_stream/expected_2delta_stream.sga");
	SGA_Stream expected	  = SGA_FullStreamGraph_from(&_expected);

	bool result = true;
	for (size_t t = 0; t < 10; t++) {

		// Test nodes present at time t
		SGA_NodesIterator _nodes_present_at_t_delta	= SGA_Stream_nodes_present_at_t(&delta_stream, t);
		SGA_NodesIterator _nodes_present_at_t_expected	= SGA_Stream_nodes_present_at_t(&expected, t);
		SGA_NodeIdArrayList nodes_present_at_t_delta	= SGA_collect_node_ids(_nodes_present_at_t_delta);
		SGA_NodeIdArrayList nodes_present_at_t_expected = SGA_collect_node_ids(_nodes_present_at_t_expected);
		SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_t_delta);
		SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_t_expected);

		result &= EXPECT(SGA_NodeIdArrayList_equals(&nodes_present_at_t_delta, &nodes_present_at_t_expected));
		SGA_NodeIdArrayList_destroy(nodes_present_at_t_delta);
		SGA_NodeIdArrayList_destroy(nodes_present_at_t_expected);

		// Test links present at time t
		SGA_LinksIterator _links_present_at_t_delta	= SGA_Stream_links_present_at_t(&delta_stream, t);
		SGA_LinksIterator _links_present_at_t_expected	= SGA_Stream_links_present_at_t(&expected, t);
		SGA_LinkIdArrayList links_present_at_t_delta	= SGA_collect_link_ids(_links_present_at_t_delta);
		SGA_LinkIdArrayList links_present_at_t_expected = SGA_collect_link_ids(_links_present_at_t_expected);
		SGA_LinkIdArrayList_sort_unstable(&links_present_at_t_delta);
		SGA_LinkIdArrayList_sort_unstable(&links_present_at_t_expected);

		result &= EXPECT(SGA_LinkIdArrayList_equals(&links_present_at_t_delta, &links_present_at_t_expected));
		SGA_LinkIdArrayList_destroy(links_present_at_t_delta);
		SGA_LinkIdArrayList_destroy(links_present_at_t_expected);
	}

	SGA_StreamGraph_destroy(original);
	SGA_StreamGraph_destroy(_expected);
	SGA_Stream_destroy(delta_stream);
	SGA_Stream_destroy(expected);
	return result;
}

int main() {
	Test* tests[] = {

	    TEST(test_same_distinct_nodes),
	    TEST(test_same_temporal_nodes),
	    TEST(test_isomorphism),
	    TEST(test_present_at_t),
	    NULL,
	};

	return test("SGA_LinkStream", tests);
}