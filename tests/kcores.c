#include "../StreamGraphAnalysis.h"
#include "test.h"

bool test_k_cores() {
	StreamGraph sg = StreamGraph_from_external("data/kcores_test.txt");
	Stream st	   = FullStreamGraph_from(&sg);

	KCore kcore_1 = Stream_k_cores(&st, 1);
	KCore kcore_2 = Stream_k_cores(&st, 2);
	KCore kcore_3 = Stream_k_cores(&st, 3);

#define PUSH_SINGLE_INTERVAL(kcore, node, start, end)                                                                                      \
	NodePresenceArrayList_push(&(kcore).nodes, (NodePresence){.node_id = (node), .presence = IntervalArrayList_new()});                    \
	IntervalArrayList_push(&(kcore).nodes.array[node].presence, Interval_from(start, end));

	KCore expected_kcore_1 = {NodePresenceArrayList_new()};
	PUSH_SINGLE_INTERVAL(expected_kcore_1, 0, 1, 6);
	PUSH_SINGLE_INTERVAL(expected_kcore_1, 1, 3, 10);
	PUSH_SINGLE_INTERVAL(expected_kcore_1, 2, 1, 9);
	PUSH_SINGLE_INTERVAL(expected_kcore_1, 3, 2, 10);

	KCore expected_kcore_2 = {NodePresenceArrayList_new()};
	PUSH_SINGLE_INTERVAL(expected_kcore_2, 0, 4, 5);
	NodePresenceArrayList_push(&(expected_kcore_2).nodes, (NodePresence){.node_id = 1, .presence = IntervalArrayList_new()});
	IntervalArrayList_push(&(expected_kcore_2).nodes.array[1].presence, Interval_from(4, 5));
	IntervalArrayList_push(&(expected_kcore_2).nodes.array[1].presence, Interval_from(7, 9));
	NodePresenceArrayList_push(&(expected_kcore_2).nodes, (NodePresence){.node_id = 2, .presence = IntervalArrayList_new()});
	IntervalArrayList_push(&(expected_kcore_2).nodes.array[2].presence, Interval_from(4, 5));
	IntervalArrayList_push(&(expected_kcore_2).nodes.array[2].presence, Interval_from(7, 9));
	NodePresenceArrayList_push(&(expected_kcore_2).nodes, (NodePresence){.node_id = 3, .presence = IntervalArrayList_new()});
	IntervalArrayList_push(&(expected_kcore_2).nodes.array[3].presence, Interval_from(4, 5));
	IntervalArrayList_push(&(expected_kcore_2).nodes.array[3].presence, Interval_from(7, 9));

	KCore expected_kcore_3 = {NodePresenceArrayList_new()};

	bool result = true;
	result &= EXPECT(KCore_equals(&kcore_1, &expected_kcore_1));
	result &= EXPECT(KCore_equals(&kcore_2, &expected_kcore_2));
	result &= EXPECT(KCore_equals(&kcore_3, &expected_kcore_3));

	KCore_destroy(kcore_1);
	KCore_destroy(kcore_2);
	KCore_destroy(kcore_3);
	KCore_destroy(expected_kcore_1);
	KCore_destroy(expected_kcore_2);
	KCore_destroy(expected_kcore_3);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);

	return result;
}

bool test_k_cores_chunk() {
	StreamGraph sg			= StreamGraph_from_external("data/kcores_with_L.txt");
	StreamGraph kcores_only = StreamGraph_from_external("data/kcores_test.txt");

	LinkIdArrayList links = LinkIdArrayList_new();
	NodeIdArrayList nodes = NodeIdArrayList_new();
	NodeIdArrayList_push(&nodes, 1);
	NodeIdArrayList_push(&nodes, 3);
	NodeIdArrayList_push(&nodes, 5);
	NodeIdArrayList_push(&nodes, 7);

	Interval snapshot = Interval_from(0, 10);
	Stream st		  = CS_without(&sg, &nodes, &links, snapshot.start, snapshot.end);
	Stream og		  = FullStreamGraph_from(&kcores_only);

	KCore kcore_1	 = Stream_k_cores(&st, 1);
	KCore og_kcore_1 = Stream_k_cores(&og, 1);
	KCore kcore_2	 = Stream_k_cores(&st, 2);
	KCore og_kcore_2 = Stream_k_cores(&og, 2);
	KCore kcore_3	 = Stream_k_cores(&st, 3);
	KCore og_kcore_3 = Stream_k_cores(&og, 3);

	og_kcore_1.nodes.array[0].node_id = 0;
	og_kcore_1.nodes.array[1].node_id = 2;
	og_kcore_1.nodes.array[2].node_id = 4;
	og_kcore_1.nodes.array[3].node_id = 6;
	og_kcore_2.nodes.array[0].node_id = 0;
	og_kcore_2.nodes.array[1].node_id = 2;
	og_kcore_2.nodes.array[2].node_id = 4;
	og_kcore_2.nodes.array[3].node_id = 6;

	bool result = true;
	result &= EXPECT(KCore_equals(&kcore_1, &og_kcore_1));
	result &= EXPECT(KCore_equals(&kcore_2, &og_kcore_2));
	result &= EXPECT(KCore_equals(&kcore_3, &og_kcore_3));

	KCore_destroy(kcore_1);
	KCore_destroy(kcore_2);
	KCore_destroy(kcore_3);
	KCore_destroy(og_kcore_1);
	KCore_destroy(og_kcore_2);
	KCore_destroy(og_kcore_3);

	StreamGraph_destroy(sg);
	StreamGraph_destroy(kcores_only);
	FullStreamGraph_destroy(og);
	CS_destroy(st);

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