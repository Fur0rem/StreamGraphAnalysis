/**
 * @file tests/snaphot_stream.c
 * @brief Tests regarding SnapshotStream
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_split_nodes() {
	SGA_StreamGraph S	= SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream split_first	= SGA_SnapshotStream_from(&S, Interval_from(0, 50));
	SGA_Stream split_second = SGA_SnapshotStream_from(&S, Interval_from(50, 100));

	StreamFunctions fns	       = SnapshotStream_stream_functions;
	NodesIterator nodes_iter_first = fns.nodes_set(split_first.stream_data);
	bool okay		       = true;
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

	SGA_SnapshotStream_destroy(split_first);
	SGA_SnapshotStream_destroy(split_second);
	SGA_StreamGraph_destroy(S);
	return okay;
}

bool test_split_links() {
	SGA_StreamGraph S	= SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream split_first	= SGA_SnapshotStream_from(&S, Interval_from(0, 50));
	SGA_Stream split_second = SGA_SnapshotStream_from(&S, Interval_from(50, 100));

	StreamFunctions fns	       = SnapshotStream_stream_functions;
	LinksIterator links_iter_first = fns.links_set(split_first.stream_data);
	bool okay		       = true;
	FOR_EACH_LINK(link_id, links_iter_first) {
		Interval interval = fns.lifespan(split_first.stream_data);
		okay &= EXPECT(Interval_contains_interval(Interval_from(0, 50), interval));
	}

	LinksIterator links_iter_second = fns.links_set(split_second.stream_data);
	FOR_EACH_LINK(link_id, links_iter_second) {
		Interval interval = fns.lifespan(split_second.stream_data);
		okay &= EXPECT(Interval_contains_interval(Interval_from(50, 100), interval));
	}

	SGA_SnapshotStream_destroy(split_first);
	SGA_SnapshotStream_destroy(split_second);
	SGA_StreamGraph_destroy(S);
	return okay;
}

bool test_same_nodes() {
	SGA_StreamGraph S	= SGA_StreamGraph_from_external("data/S_external.txt");
	SGA_Stream split_first	= SGA_SnapshotStream_from(&S, Interval_from(0, 50));
	SGA_Stream split_second = SGA_SnapshotStream_from(&S, Interval_from(50, 100));
	SGA_Stream full		= SGA_FullStreamGraph_from(&S);

	StreamFunctions fns		  = SnapshotStream_stream_functions;
	NodeIdArrayList nodes_iter_first  = SGA_collect_node_ids(fns.nodes_set(split_first.stream_data));
	NodeIdArrayList nodes_iter_second = SGA_collect_node_ids(fns.nodes_set(split_second.stream_data));

	StreamFunctions fns_full	= FullStreamGraph_stream_functions;
	NodeIdArrayList nodes_iter_full = SGA_collect_node_ids(fns_full.nodes_set(full.stream_data));

	bool okay = EXPECT(NodeIdArrayList_equals(&nodes_iter_first, &nodes_iter_full));
	okay &= EXPECT(NodeIdArrayList_equals(&nodes_iter_second, &nodes_iter_full));

	NodeIdArrayList_destroy(nodes_iter_first);
	NodeIdArrayList_destroy(nodes_iter_second);
	NodeIdArrayList_destroy(nodes_iter_full);
	SGA_SnapshotStream_destroy(split_first);
	SGA_SnapshotStream_destroy(split_second);
	SGA_FullStreamGraph_destroy(full);
	SGA_StreamGraph_destroy(S);

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