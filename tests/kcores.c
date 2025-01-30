/**
 * @file tests/kcores.c
 * @brief Tests regarding K-Cores
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"

#define PUSH_SINGLE_INTERVAL(kcore, node, start, end)                                                                                      \
	NodePresenceArrayList_push(&(kcore).nodes, (NodePresence){.node_id = (node), .presence = SGA_IntervalArrayList_new()});            \
	SGA_IntervalArrayList_push(&(kcore).nodes.array[node].presence, SGA_Interval_from(start, end));

bool test_k_cores() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/kcores.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	SGA_Cluster kcore_1 = SGA_Stream_k_core(&st, 1);
	SGA_Cluster kcore_2 = SGA_Stream_k_core(&st, 2);
	SGA_Cluster kcore_3 = SGA_Stream_k_core(&st, 3);

	// SGA_Cluster expected_kcore_1 = {NodePresenceArrayList_new()};
	// PUSH_SINGLE_INTERVAL(expected_kcore_1, 0, 1, 6);
	// PUSH_SINGLE_INTERVAL(expected_kcore_1, 1, 3, 10);
	// PUSH_SINGLE_INTERVAL(expected_kcore_1, 2, 1, 9);
	// PUSH_SINGLE_INTERVAL(expected_kcore_1, 3, 2, 10);
	SGA_Cluster expected_kcore_1 = SGA_Cluster_empty();
	SGA_Cluster_add_node(&expected_kcore_1, SGA_ClusterNode_single_interval(0, SGA_Interval_from(1, 6)));
	SGA_Cluster_add_node(&expected_kcore_1, SGA_ClusterNode_single_interval(1, SGA_Interval_from(3, 10)));
	SGA_Cluster_add_node(&expected_kcore_1, SGA_ClusterNode_single_interval(2, SGA_Interval_from(1, 9)));
	SGA_Cluster_add_node(&expected_kcore_1, SGA_ClusterNode_single_interval(3, SGA_Interval_from(2, 10)));

	// SGA_Cluster expected_kcore_2 = {NodePresenceArrayList_new()};
	// PUSH_SINGLE_INTERVAL(expected_kcore_2, 0, 4, 5);
	// NodePresenceArrayList_push(&(expected_kcore_2).nodes, (NodePresence){.node_id = 1, .presence = SGA_IntervalArrayList_new()});
	// SGA_IntervalArrayList_push(&(expected_kcore_2).nodes.array[1].presence, SGA_Interval_from(4, 5));
	// SGA_IntervalArrayList_push(&(expected_kcore_2).nodes.array[1].presence, SGA_Interval_from(7, 9));
	// NodePresenceArrayList_push(&(expected_kcore_2).nodes, (NodePresence){.node_id = 2, .presence = SGA_IntervalArrayList_new()});
	// SGA_IntervalArrayList_push(&(expected_kcore_2).nodes.array[2].presence, SGA_Interval_from(4, 5));
	// SGA_IntervalArrayList_push(&(expected_kcore_2).nodes.array[2].presence, SGA_Interval_from(7, 9));
	// NodePresenceArrayList_push(&(expected_kcore_2).nodes, (NodePresence){.node_id = 3, .presence = SGA_IntervalArrayList_new()});
	// SGA_IntervalArrayList_push(&(expected_kcore_2).nodes.array[3].presence, SGA_Interval_from(4, 5));
	// SGA_IntervalArrayList_push(&(expected_kcore_2).nodes.array[3].presence, SGA_Interval_from(7, 9));
	SGA_Cluster expected_kcore_2 = SGA_Cluster_empty();
	SGA_ClusterNode node0	     = SGA_ClusterNode_empty(0);
	SGA_ClusterNode_add_interval(&node0, SGA_Interval_from(4, 5));
	SGA_Cluster_add_node(&expected_kcore_2, node0);

	SGA_ClusterNode node1 = SGA_ClusterNode_empty(1);
	SGA_ClusterNode_add_interval(&node1, SGA_Interval_from(4, 5));
	SGA_ClusterNode_add_interval(&node1, SGA_Interval_from(7, 9));
	SGA_Cluster_add_node(&expected_kcore_2, node1);

	SGA_ClusterNode node2 = SGA_ClusterNode_empty(2);
	SGA_ClusterNode_add_interval(&node2, SGA_Interval_from(4, 5));
	SGA_ClusterNode_add_interval(&node2, SGA_Interval_from(7, 9));
	SGA_Cluster_add_node(&expected_kcore_2, node2);

	SGA_ClusterNode node3 = SGA_ClusterNode_empty(3);
	SGA_ClusterNode_add_interval(&node3, SGA_Interval_from(4, 5));
	SGA_ClusterNode_add_interval(&node3, SGA_Interval_from(7, 9));
	SGA_Cluster_add_node(&expected_kcore_2, node3);

	SGA_Cluster expected_kcore_3 = SGA_Cluster_empty();

	bool result = true;
	result &= EXPECT(SGA_Cluster_equals(&kcore_1, &expected_kcore_1));
	result &= EXPECT(SGA_Cluster_equals(&kcore_2, &expected_kcore_2));
	result &= EXPECT(SGA_Cluster_equals(&kcore_3, &expected_kcore_3));

	SGA_Cluster_destroy(kcore_1);
	SGA_Cluster_destroy(kcore_2);
	SGA_Cluster_destroy(kcore_3);
	SGA_Cluster_destroy(expected_kcore_1);
	SGA_Cluster_destroy(expected_kcore_2);
	SGA_Cluster_destroy(expected_kcore_3);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);

	return result;
}

bool test_k_cores_chunk() {
	SGA_StreamGraph sg	    = SGA_StreamGraph_from_file("data/tests/kcores_fused_with_L.sga");
	SGA_StreamGraph kcores_only = SGA_StreamGraph_from_file("data/tests/kcores.sga");

	SGA_LinkIdArrayList links = SGA_LinkIdArrayList_new();
	SGA_NodeIdArrayList nodes = SGA_NodeIdArrayList_new();
	SGA_NodeIdArrayList_push(&nodes, 1);
	SGA_NodeIdArrayList_push(&nodes, 3);
	SGA_NodeIdArrayList_push(&nodes, 5);
	SGA_NodeIdArrayList_push(&nodes, 7);

	SGA_Interval timeframe = SGA_Interval_from(0, 10);
	SGA_Stream st	       = SGA_ChunkStream_without(&sg, &nodes, &links, timeframe);
	SGA_Stream og	       = SGA_FullStreamGraph_from(&kcores_only);

	SGA_NodeIdArrayList_destroy(nodes);
	SGA_LinkIdArrayList_destroy(links);

	SGA_Cluster kcore_1    = SGA_Stream_k_core(&st, 1);
	SGA_Cluster og_kcore_1 = SGA_Stream_k_core(&og, 1);
	SGA_Cluster kcore_2    = SGA_Stream_k_core(&st, 2);
	SGA_Cluster og_kcore_2 = SGA_Stream_k_core(&og, 2);
	SGA_Cluster kcore_3    = SGA_Stream_k_core(&st, 3);
	SGA_Cluster og_kcore_3 = SGA_Stream_k_core(&og, 3);

	og_kcore_1.nodes.array[0].node_id = 0;
	og_kcore_1.nodes.array[1].node_id = 2;
	og_kcore_1.nodes.array[2].node_id = 4;
	og_kcore_1.nodes.array[3].node_id = 6;
	og_kcore_2.nodes.array[0].node_id = 0;
	og_kcore_2.nodes.array[1].node_id = 2;
	og_kcore_2.nodes.array[2].node_id = 4;
	og_kcore_2.nodes.array[3].node_id = 6;

	bool result = true;
	result &= EXPECT(SGA_Cluster_equals(&kcore_1, &og_kcore_1));
	result &= EXPECT(SGA_Cluster_equals(&kcore_2, &og_kcore_2));
	result &= EXPECT(SGA_Cluster_equals(&kcore_3, &og_kcore_3));

	SGA_Cluster_destroy(kcore_1);
	SGA_Cluster_destroy(kcore_2);
	SGA_Cluster_destroy(kcore_3);
	SGA_Cluster_destroy(og_kcore_1);
	SGA_Cluster_destroy(og_kcore_2);
	SGA_Cluster_destroy(og_kcore_3);

	SGA_FullStreamGraph_destroy(og);
	SGA_ChunkStream_destroy(st);
	SGA_StreamGraph_destroy(sg);
	SGA_StreamGraph_destroy(kcores_only);

	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_k_cores),
	    TEST(test_k_cores_chunk),
	    NULL,
	};

	return test("K-Cores", tests);
}