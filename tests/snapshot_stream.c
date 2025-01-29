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
	SGA_StreamGraph sg	= SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream split_first	= SGA_SnapshotStream_from(&sg, SGA_Interval_from(0, 50));
	SGA_Stream split_second = SGA_SnapshotStream_from(&sg, SGA_Interval_from(50, 100));

	StreamFunctions fns		   = SnapshotStream_stream_functions;
	SGA_NodesIterator nodes_iter_first = fns.nodes_set(split_first.stream_data);
	bool okay			   = true;
	SGA_FOR_EACH_NODE(node_id, nodes_iter_first) {
		SGA_TimesIterator times_iter = fns.times_node_present(split_first.stream_data, node_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			okay &= EXPECT(SGA_Interval_contains_interval(SGA_Interval_from(0, 50), interval));
		}
	}

	SGA_NodesIterator nodes_iter_second = fns.nodes_set(split_second.stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes_iter_second) {
		SGA_TimesIterator times_iter = fns.times_node_present(split_second.stream_data, node_id);
		SGA_FOR_EACH_TIME(interval, times_iter) {
			okay &= EXPECT(SGA_Interval_contains_interval(SGA_Interval_from(50, 100), interval));
		}
	}

	SGA_SnapshotStream_destroy(split_first);
	SGA_SnapshotStream_destroy(split_second);
	SGA_StreamGraph_destroy(sg);
	return okay;
}

bool test_split_links() {
	SGA_StreamGraph sg	= SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream split_first	= SGA_SnapshotStream_from(&sg, SGA_Interval_from(0, 50));
	SGA_Stream split_second = SGA_SnapshotStream_from(&sg, SGA_Interval_from(50, 100));

	StreamFunctions fns		   = SnapshotStream_stream_functions;
	SGA_LinksIterator links_iter_first = fns.links_set(split_first.stream_data);
	bool okay			   = true;
	SGA_FOR_EACH_LINK(link_id, links_iter_first) {
		SGA_Interval interval = fns.lifespan(split_first.stream_data);
		okay &= EXPECT(SGA_Interval_contains_interval(SGA_Interval_from(0, 50), interval));
	}

	SGA_LinksIterator links_iter_second = fns.links_set(split_second.stream_data);
	SGA_FOR_EACH_LINK(link_id, links_iter_second) {
		SGA_Interval interval = fns.lifespan(split_second.stream_data);
		okay &= EXPECT(SGA_Interval_contains_interval(SGA_Interval_from(50, 100), interval));
	}

	SGA_SnapshotStream_destroy(split_first);
	SGA_SnapshotStream_destroy(split_second);
	SGA_StreamGraph_destroy(sg);
	return okay;
}

bool test_same_nodes() {
	SGA_StreamGraph sg	= SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_Stream split_first	= SGA_SnapshotStream_from(&sg, SGA_Interval_from(0, 50));
	SGA_Stream split_second = SGA_SnapshotStream_from(&sg, SGA_Interval_from(50, 100));
	SGA_Stream full		= SGA_FullStreamGraph_from(&sg);

	StreamFunctions fns		      = SnapshotStream_stream_functions;
	SGA_NodeIdArrayList nodes_iter_first  = SGA_collect_node_ids(fns.nodes_set(split_first.stream_data));
	SGA_NodeIdArrayList nodes_iter_second = SGA_collect_node_ids(fns.nodes_set(split_second.stream_data));

	StreamFunctions fns_full	    = FullStreamGraph_stream_functions;
	SGA_NodeIdArrayList nodes_iter_full = SGA_collect_node_ids(fns_full.nodes_set(full.stream_data));

	bool okay = EXPECT(SGA_NodeIdArrayList_equals(&nodes_iter_first, &nodes_iter_full));
	okay &= EXPECT(SGA_NodeIdArrayList_equals(&nodes_iter_second, &nodes_iter_full));

	SGA_NodeIdArrayList_destroy(nodes_iter_first);
	SGA_NodeIdArrayList_destroy(nodes_iter_second);
	SGA_NodeIdArrayList_destroy(nodes_iter_full);
	SGA_SnapshotStream_destroy(split_first);
	SGA_SnapshotStream_destroy(split_second);
	SGA_FullStreamGraph_destroy(full);
	SGA_StreamGraph_destroy(sg);

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