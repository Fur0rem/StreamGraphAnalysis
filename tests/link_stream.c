#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_links_at_time_8() {
	StreamGraph sg = StreamGraph_from_external("data/L.txt");
	init_events_table(&sg);
	Stream ls	      = LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	LinkIdArrayList links	 = SGA_collect_link_ids(funcs.links_present_at_t(ls.stream_data, 8));
	LinkIdArrayList expected = LinkIdArrayList_new();
	String str		 = LinkIdArrayList_to_string(&links);
	printf("All links present at 8 : %s\n", str.data);
	String_destroy(str);
	LinkIdArrayList_destroy(links);
	LinkIdArrayList_destroy(expected);
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	LinkStream_destroy(ls);
	return true;
}

bool test_nodes_present() {
	StreamGraph sg = StreamGraph_from_external("data/S_external.txt");
	init_events_table(&sg);
	Stream ls	      = LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	NodeIdArrayList nodes_expected = SGA_collect_node_ids(funcs.nodes_set(ls.stream_data));
	bool nodes_present	       = true;
	for (TimeId t = 0; t < StreamGraph_lifespan(&sg).end; t++) {
		NodeIdArrayList nodes_present_at_t = SGA_collect_node_ids(funcs.nodes_present_at_t(ls.stream_data, t));
		nodes_present &= EXPECT(NodeIdArrayList_equals(&nodes_present_at_t, &nodes_expected));
		NodeIdArrayList_destroy(nodes_present_at_t);
	}

	NodeIdArrayList_destroy(nodes_expected);
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	LinkStream_destroy(ls);
	return nodes_present;
}

bool test_nodes_presence() {
	StreamGraph sg	      = StreamGraph_from_external("data/S_external.txt");
	Stream ls	      = LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	Interval interval	    = StreamGraph_lifespan(&sg);
	IntervalArrayList intervals = IntervalArrayList_with_capacity(1);
	IntervalArrayList_push(&intervals, interval);

	NodesIterator set	  = funcs.nodes_set(ls.stream_data);
	bool nodes_always_present = true;
	FOR_EACH_NODE(node_id, set) {
		IntervalArrayList intervals_node = SGA_collect_times(funcs.times_node_present(ls.stream_data, node_id));
		nodes_always_present &= EXPECT(IntervalArrayList_equals(&intervals_node, &intervals));
		IntervalArrayList_destroy(intervals_node);
	}

	IntervalArrayList_destroy(intervals);
	StreamGraph_destroy(sg);
	LinkStream_destroy(ls);

	return nodes_always_present;
}

bool test_neighbours_of_node() {
	StreamGraph sg	      = StreamGraph_from_external("data/S_external.txt");
	Stream ls	      = LinkStream_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	LinkIdArrayList neighbours_of_0 = LinkIdArrayList_new();
	LinkIdArrayList_push(&neighbours_of_0, 1);
	LinkIdArrayList_push(&neighbours_of_0, 2);
	LinkIdArrayList neighbours_of_1 = LinkIdArrayList_new();
	LinkIdArrayList_push(&neighbours_of_1, 0);
	LinkIdArrayList_push(&neighbours_of_1, 2);
	LinkIdArrayList_push(&neighbours_of_1, 3);
	LinkIdArrayList neighbours_of_2 = LinkIdArrayList_new();
	LinkIdArrayList_push(&neighbours_of_2, 0);
	LinkIdArrayList_push(&neighbours_of_2, 1);
	LinkIdArrayList neighbours_of_3 = LinkIdArrayList_new();
	LinkIdArrayList_push(&neighbours_of_3, 1);
	LinkIdArrayList neighbours[] = {neighbours_of_0, neighbours_of_1, neighbours_of_2, neighbours_of_3};

	NodesIterator set	  = funcs.nodes_set(ls.stream_data);
	bool has_right_neighbours = true;

	FOR_EACH_NODE(node_id, set) {
		LinksIterator neighbours_of_node = funcs.neighbours_of_node(ls.stream_data, node_id);
		LinkIdArrayList neighbours_ids	 = SGA_collect_link_ids(neighbours_of_node);

		// Map neighbouring links to neighbouring nodes
		for (size_t i = 0; i < neighbours_ids.length; i++) {
			LinkId link_id		= neighbours_ids.array[i];
			Link link		= funcs.link_by_id(ls.stream_data, link_id);
			NodeId other_node	= Link_get_other_node(&link, node_id);
			neighbours_ids.array[i] = other_node;
		}

		has_right_neighbours &= EXPECT(LinkIdArrayList_equals(&neighbours_ids, &neighbours[node_id]));
		LinkIdArrayList_destroy(neighbours_ids);
	}

	for (size_t i = 0; i < 4; i++) {
		LinkIdArrayList_destroy(neighbours[i]);
	}
	StreamGraph_destroy(sg);
	LinkStream_destroy(ls);

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

	return test("LinkStream", tests);
}