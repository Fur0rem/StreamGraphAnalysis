#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_split_nodes() {
	StreamGraph S		= StreamGraph_from_external("data/S_external.txt");
	Stream split_first	= SnapshotStream_from(&S, Interval_from(0, 50));
	Stream split_second = SnapshotStream_from(&S, Interval_from(50, 100));

	StreamFunctions fns			   = SnapshotStream_stream_functions;
	NodesIterator nodes_iter_first = fns.nodes_set(split_first.stream_data);
	bool okay					   = true;
	FOR_EACH_NODE(node_id, nodes_iter_first) {
		TimesIterator times_iter = fns.times_node_present(split_first.stream_data, node_id);
		FOR_EACH_TIME(interval, times_iter) {
			okay &= EXPECT(Interval_contains_interval(Interval_from(0, 50), interval));
		}
	}

	NodesIterator nodes_iter_second = fns.nodes_set(split_second.stream_data);
	FOR_EACH_NODE(node_id, nodes_iter_second) {
		TimesIterator times_iter = fns.times_node_present(split_second.stream_data, node_id);
		FOR_EACH_TIME(interval, times_iter) {
			okay &= EXPECT(Interval_contains_interval(Interval_from(50, 100), interval));
		}
	}

	SnapshotStream_destroy(split_first);
	SnapshotStream_destroy(split_second);
	StreamGraph_destroy(S);
	return okay;
}

bool test_split_links() {
	StreamGraph S		= StreamGraph_from_external("data/S_external.txt");
	Stream split_first	= SnapshotStream_from(&S, Interval_from(0, 50));
	Stream split_second = SnapshotStream_from(&S, Interval_from(50, 100));

	StreamFunctions fns			   = SnapshotStream_stream_functions;
	LinksIterator links_iter_first = fns.links_set(split_first.stream_data);
	bool okay					   = true;
	FOR_EACH_LINK(link_id, links_iter_first) {
		Interval interval = fns.lifespan(split_first.stream_data);
		okay &= EXPECT(Interval_contains_interval(Interval_from(0, 50), interval));
	}

	LinksIterator links_iter_second = fns.links_set(split_second.stream_data);
	FOR_EACH_LINK(link_id, links_iter_second) {
		Interval interval = fns.lifespan(split_second.stream_data);
		okay &= EXPECT(Interval_contains_interval(Interval_from(50, 100), interval));
	}

	SnapshotStream_destroy(split_first);
	SnapshotStream_destroy(split_second);
	StreamGraph_destroy(S);
	return okay;
}

bool test_same_nodes() {
	StreamGraph S		= StreamGraph_from_external("data/S_external.txt");
	Stream split_first	= SnapshotStream_from(&S, Interval_from(0, 50));
	Stream split_second = SnapshotStream_from(&S, Interval_from(50, 100));
	Stream full			= FullStreamGraph_from(&S);

	StreamFunctions fns			   = SnapshotStream_stream_functions;
	NodeIdVector nodes_iter_first  = SGA_collect_node_ids(fns.nodes_set(split_first.stream_data));
	NodeIdVector nodes_iter_second = SGA_collect_node_ids(fns.nodes_set(split_second.stream_data));

	StreamFunctions fns_full	 = FullStreamGraph_stream_functions;
	NodeIdVector nodes_iter_full = SGA_collect_node_ids(fns_full.nodes_set(full.stream_data));

	bool okay = EXPECT(NodeIdVector_equals(&nodes_iter_first, &nodes_iter_full));
	okay &= EXPECT(NodeIdVector_equals(&nodes_iter_second, &nodes_iter_full));

	NodeIdVector_destroy(nodes_iter_first);
	NodeIdVector_destroy(nodes_iter_second);
	NodeIdVector_destroy(nodes_iter_full);
	SnapshotStream_destroy(split_first);
	SnapshotStream_destroy(split_second);
	FullStreamGraph_destroy(full);
	StreamGraph_destroy(S);

	return okay;
}

int main() {
	Test* tests[] = {
		TEST(test_split_nodes),
		TEST(test_split_links),
		TEST(test_same_nodes),
		NULL,
	};

	return test("SnapshotStream", tests);
}