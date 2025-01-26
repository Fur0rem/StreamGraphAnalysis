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

bool test_links_at_time_8() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_external("data/L.txt");
	SGA_Stream ls	      = SGA_LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	SGA_LinkIdArrayList links    = SGA_collect_link_ids(funcs.links_present_at_t(ls.stream_data, 8));
	SGA_LinkIdArrayList expected = SGA_LinkIdArrayList_new();
	String str		     = SGA_LinkIdArrayList_to_string(&links);
	printf("All links present at 8 : %s\n", str.data);
	String_destroy(str);
	SGA_LinkIdArrayList_destroy(links);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);
	SGA_LinkStream_destroy(ls);
	return true;
}

bool test_nodes_present() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream ls	      = SGA_LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	SGA_NodeIdArrayList nodes_expected = SGA_collect_node_ids(funcs.nodes_set(ls.stream_data));
	bool nodes_present		   = true;
	for (SGA_Time t = 0; t < SGA_StreamGraph_lifespan(&sg).end; t++) {
		SGA_NodeIdArrayList nodes_present_at_t = SGA_collect_node_ids(funcs.nodes_present_at_t(ls.stream_data, t));
		nodes_present &= EXPECT(SGA_NodeIdArrayList_equals(&nodes_present_at_t, &nodes_expected));
		SGA_NodeIdArrayList_destroy(nodes_present_at_t);
	}

	SGA_NodeIdArrayList_destroy(nodes_expected);
	SGA_StreamGraph_destroy(sg);
	SGA_LinkStream_destroy(ls);
	return nodes_present;
}

bool test_nodes_presence() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream ls	      = SGA_LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	SGA_Interval interval		= SGA_StreamGraph_lifespan(&sg);
	SGA_IntervalArrayList intervals = SGA_IntervalArrayList_with_capacity(1);
	SGA_IntervalArrayList_push(&intervals, interval);

	SGA_NodesIterator set	  = funcs.nodes_set(ls.stream_data);
	bool nodes_always_present = true;
	SGA_FOR_EACH_NODE(node_id, set) {
		SGA_IntervalArrayList intervals_node = SGA_collect_times(funcs.times_node_present(ls.stream_data, node_id));
		nodes_always_present &= EXPECT(SGA_IntervalArrayList_equals(&intervals_node, &intervals));
		SGA_IntervalArrayList_destroy(intervals_node);
	}

	SGA_IntervalArrayList_destroy(intervals);
	SGA_StreamGraph_destroy(sg);
	SGA_LinkStream_destroy(ls);

	return nodes_always_present;
}

bool test_neighbours_of_node() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream ls	      = SGA_LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	SGA_LinkIdArrayList neighbours_of_0 = SGA_LinkIdArrayList_new();
	SGA_LinkIdArrayList_push(&neighbours_of_0, 1);
	SGA_LinkIdArrayList_push(&neighbours_of_0, 2);
	SGA_LinkIdArrayList neighbours_of_1 = SGA_LinkIdArrayList_new();
	SGA_LinkIdArrayList_push(&neighbours_of_1, 0);
	SGA_LinkIdArrayList_push(&neighbours_of_1, 2);
	SGA_LinkIdArrayList_push(&neighbours_of_1, 3);
	SGA_LinkIdArrayList neighbours_of_2 = SGA_LinkIdArrayList_new();
	SGA_LinkIdArrayList_push(&neighbours_of_2, 0);
	SGA_LinkIdArrayList_push(&neighbours_of_2, 1);
	SGA_LinkIdArrayList neighbours_of_3 = SGA_LinkIdArrayList_new();
	SGA_LinkIdArrayList_push(&neighbours_of_3, 1);
	SGA_LinkIdArrayList neighbours[] = {neighbours_of_0, neighbours_of_1, neighbours_of_2, neighbours_of_3};

	SGA_NodesIterator set	  = funcs.nodes_set(ls.stream_data);
	bool has_right_neighbours = true;

	SGA_FOR_EACH_NODE(node_id, set) {
		SGA_LinksIterator neighbours_of_node = funcs.neighbours_of_node(ls.stream_data, node_id);
		SGA_LinkIdArrayList neighbours_ids   = SGA_collect_link_ids(neighbours_of_node);

		// Map neighbouring links to neighbouring nodes
		for (size_t i = 0; i < neighbours_ids.length; i++) {
			SGA_LinkId link_id	= neighbours_ids.array[i];
			SGA_Link link		= funcs.link_by_id(ls.stream_data, link_id);
			SGA_NodeId other_node	= SGA_Link_get_other_node(&link, node_id);
			neighbours_ids.array[i] = other_node;
		}

		has_right_neighbours &= EXPECT(SGA_LinkIdArrayList_equals(&neighbours_ids, &neighbours[node_id]));
		SGA_LinkIdArrayList_destroy(neighbours_ids);
	}

	for (size_t i = 0; i < 4; i++) {
		SGA_LinkIdArrayList_destroy(neighbours[i]);
	}
	SGA_StreamGraph_destroy(sg);
	SGA_LinkStream_destroy(ls);

	return has_right_neighbours;
}

int main() {
	Test* tests[] = {
	    TEST(test_links_at_time_8),
	    TEST(test_nodes_present),
	    TEST(test_nodes_presence),
	    TEST(test_neighbours_of_node),
	    NULL,
	};

	return test("SGA_LinkStream", tests);
}