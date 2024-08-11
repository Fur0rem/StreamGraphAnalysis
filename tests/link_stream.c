#include "../src/stream/link_stream.h"
#include "../src/interval.h"
#include "../src/stream.h"
#include "../src/units.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_links_at_time_8() {
	StreamGraph sg = StreamGraph_from_external("data/L.txt");
	init_events_table(&sg);
	Stream ls = LS_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	LinkIdVector links = SGA_collect_link_ids(funcs.links_present_at_t(ls.stream_data, 8));
	LinkIdVector expected = LinkIdVector_new();
	String str = LinkIdVector_to_string(&links);
	printf("All links present at 8 : %s\n", str.data);
	String_destroy(str);
	LinkIdVector_destroy(links);
	LinkIdVector_destroy(expected);
	StreamGraph_destroy(sg);
	// Stream_destroy(ls);
	return true;
}

bool test_nodes_present() {
	StreamGraph sg = StreamGraph_from_external("data/S_external.txt");
	init_events_table(&sg);
	Stream ls = LS_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	NodeIdVector nodes_expected = SGA_collect_node_ids(funcs.nodes_set(ls.stream_data));
	bool nodes_present = true;
	for (TimeId t = 0; t < StreamGraph_lifespan_end(&sg); t++) {
		NodeIdVector nodes_present_at_t = SGA_collect_node_ids(funcs.nodes_present_at_t(ls.stream_data, t));
		nodes_present &= EXPECT(NodeIdVector_equals(&nodes_present_at_t, &nodes_expected));
		NodeIdVector_destroy(nodes_present_at_t);
	}
	return nodes_present;
}

bool test_nodes_presence() {
	StreamGraph sg = StreamGraph_from_external("data/S_external.txt");
	Stream ls = LS_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	Interval interval = Interval_from(StreamGraph_lifespan_begin(&sg), StreamGraph_lifespan_end(&sg));
	IntervalVector intervals = IntervalVector_with_capacity(1);
	IntervalVector_push(&intervals, interval);

	NodesIterator set = funcs.nodes_set(ls.stream_data);
	bool nodes_always_present = true;
	FOR_EACH_NODE(node_id, set) {
		IntervalVector intervals_node = SGA_collect_times(funcs.times_node_present(ls.stream_data, node_id));
		nodes_always_present &= EXPECT(IntervalVector_equals(&intervals_node, &intervals));
		IntervalVector_destroy(intervals_node);
	}
	return nodes_always_present;
}

bool test_neighbours_of_node() {
	StreamGraph sg = StreamGraph_from_external("data/S_external.txt");
	Stream ls = LS_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	LinkIdVector neighbours_of_0 = LinkIdVector_new();
	LinkIdVector_push(&neighbours_of_0, 1);
	LinkIdVector_push(&neighbours_of_0, 2);
	LinkIdVector neighbours_of_1 = LinkIdVector_new();
	LinkIdVector_push(&neighbours_of_1, 0);
	LinkIdVector_push(&neighbours_of_1, 2);
	LinkIdVector_push(&neighbours_of_1, 3);
	LinkIdVector neighbours_of_2 = LinkIdVector_new();
	LinkIdVector_push(&neighbours_of_2, 0);
	LinkIdVector_push(&neighbours_of_2, 1);
	LinkIdVector neighbours_of_3 = LinkIdVector_new();
	LinkIdVector_push(&neighbours_of_3, 1);
	LinkIdVector neighbours[] = {neighbours_of_0, neighbours_of_1, neighbours_of_2, neighbours_of_3};

	NodesIterator set = funcs.nodes_set(ls.stream_data);
	bool has_right_neighbours = true;

	FOR_EACH_NODE(node_id, set) {
		LinksIterator neighbours_of_node = funcs.neighbours_of_node(ls.stream_data, node_id);
		LinkIdVector neighbours_ids = SGA_collect_link_ids(neighbours_of_node);

		// Map neighbouring links to neighbouring nodes
		for (size_t i = 0; i < neighbours_ids.size; i++) {
			LinkId link_id = neighbours_ids.array[i];
			Link link = funcs.nth_link(ls.stream_data, link_id);
			NodeId other_node = Link_get_other_node(&link, node_id);
			neighbours_ids.array[i] = other_node;
		}

		has_right_neighbours &= EXPECT(LinkIdVector_equals(&neighbours_ids, &neighbours[node_id]));
		LinkIdVector_destroy(neighbours_ids);
	}

	return has_right_neighbours;
}

int main() {
	Test* tests[] = {
		&(Test){"test_links_at_time_8",	test_links_at_time_8	},
 // TODO: Better test names
		&(Test){"test_nodes_present",	  test_nodes_present		},
		   &(Test){"test_nodes_presence",	  test_nodes_presence	 },
		&(Test){"test_neighbours_of_node", test_neighbours_of_node},

		NULL
	};

	return test("LinkStream", tests);
}