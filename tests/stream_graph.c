#include "../src/stream_graph.h"
#include "../src/interval.h"

#include "test.h"

bool test_create_empty() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();
	StreamGraph graph = stream_graph_from(interval_from(0.0, 0.0), nodes, links);
	char* str = StreamGraph_to_string(&graph);
	printf("%s\n", str);
	free(str);
	return EXPECT(F_EQUALS(graph.lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph.lifespan.end, 0.0)) && EXPECT(graph.temporal_nodes.size == 0) &&
		   EXPECT(graph.links.size == 0);
}

bool test_create_1() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();

	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, interval_from(0.0, 2.0));
	TemporalNode nodeB = (TemporalNode){.label = "B", .present_at = intervalsB};

	TemporalNodeVector_push(&nodes, nodeA);
	TemporalNodeVector_push(&nodes, nodeB);

	Link link = (Link){.nodes = {&nodeA, &nodeB}};
	IntervalVector_push(&link.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, link);

	StreamGraph graph = stream_graph_from(interval_from(0.0, 2.0), nodes, links);
	char* str = StreamGraph_to_string(&graph);
	printf("%s\n", str);
	free(str);

	return EXPECT(F_EQUALS(graph.lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph.lifespan.end, 2.0)) && EXPECT(graph.temporal_nodes.size == 2) &&
		   EXPECT(graph.links.size == 1);
}

bool test_create_multiple_intervals() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();

	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	IntervalVector_push(&intervalsA, interval_from(2.0, 3.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, interval_from(0.0, 2.0));
	IntervalVector_push(&intervalsB, interval_from(3.0, 4.0));
	TemporalNode nodeB = (TemporalNode){.label = "B", .present_at = intervalsB};

	TemporalNodeVector_push(&nodes, nodeA);
	TemporalNodeVector_push(&nodes, nodeB);

	Link link = (Link){.nodes = {&nodeA, &nodeB}};
	IntervalVector_push(&link.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, link);

	StreamGraph graph = stream_graph_from(interval_from(0.0, 4.0), nodes, links);
	char* str = StreamGraph_to_string(&graph);
	printf("%s\n", str);
	free(str);

	return EXPECT(F_EQUALS(graph.lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph.lifespan.end, 4.0)) && EXPECT(graph.temporal_nodes.size == 2) &&
		   EXPECT(graph.links.size == 1);
}

bool test_create_multiple_links() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();

	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	IntervalVector_push(&intervalsA, interval_from(2.0, 3.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, interval_from(0.0, 2.0));
	IntervalVector_push(&intervalsB, interval_from(3.0, 4.0));
	TemporalNode nodeB = (TemporalNode){.label = "B", .present_at = intervalsB};

	IntervalVector intervalsC = IntervalVector_new();
	IntervalVector_push(&intervalsC, interval_from(0.0, 1.0));
	IntervalVector_push(&intervalsC, interval_from(2.0, 3.0));
	TemporalNode nodeC = (TemporalNode){.label = "C", .present_at = intervalsC};

	TemporalNodeVector_push(&nodes, nodeA);
	TemporalNodeVector_push(&nodes, nodeB);
	TemporalNodeVector_push(&nodes, nodeC);

	Link linkAB = (Link){.nodes = {&nodeA, &nodeB}};
	IntervalVector_push(&linkAB.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, linkAB);

	Link linkBC = (Link){.nodes = {&nodeB, &nodeC}};
	IntervalVector_push(&linkBC.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, linkBC);

	StreamGraph graph = stream_graph_from(interval_from(0.0, 4.0), nodes, links);
	char* str = StreamGraph_to_string(&graph);
	printf("%s\n", str);
	free(str);

	return EXPECT(F_EQUALS(graph.lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph.lifespan.end, 4.0)) && EXPECT(graph.temporal_nodes.size == 3) &&
		   EXPECT(graph.links.size == 2);
}

bool test_is_node_present_at() {
	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	return EXPECT(is_node_present_at(nodeA, 0.0)) && EXPECT(is_node_present_at(nodeA, 1.0));
}

bool test_is_node_not_present_at() {
	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	return EXPECT(!is_node_present_at(nodeA, 2.0));
}

bool test_is_link_present_at() {
	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, interval_from(0.0, 1.0));
	TemporalNode nodeB = (TemporalNode){.label = "B", .present_at = intervalsB};

	Link link = (Link){.nodes = {&nodeA, &nodeB}};
	IntervalVector_push(&link.present_at, interval_from(0.0, 1.0));

	return EXPECT(is_link_present_at(link, 0.0)) && EXPECT(is_link_present_at(link, 1.0));
}

bool test_is_link_not_present_at() {
	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	TemporalNode nodeA = (TemporalNode){.label = "A", .present_at = intervalsA};

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, interval_from(0.0, 1.0));
	TemporalNode nodeB = (TemporalNode){.label = "B", .present_at = intervalsB};

	Link link = (Link){.nodes = {&nodeA, &nodeB}};
	IntervalVector_push(&link.present_at, interval_from(0.0, 1.0));

	return EXPECT(!is_link_present_at(link, 2.0));
}

bool test_get_nodes_present_at() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();

	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, interval_from(0.0, 1.0));
	IntervalVector_push(&intervalsA, interval_from(2.0, 3.0));
	TemporalNode nodeA = TemporalNode_new("A", intervalsA);

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, interval_from(0.0, 2.0));
	IntervalVector_push(&intervalsB, interval_from(3.0, 4.0));
	TemporalNode nodeB = TemporalNode_new("B", intervalsB);

	IntervalVector intervalsC = IntervalVector_new();
	IntervalVector_push(&intervalsC, interval_from(0.0, 1.0));
	IntervalVector_push(&intervalsC, interval_from(2.0, 3.0));
	TemporalNode nodeC = TemporalNode_new("C", intervalsC);

	TemporalNodeVector_push(&nodes, nodeA);
	TemporalNodeVector_push(&nodes, nodeB);
	TemporalNodeVector_push(&nodes, nodeC);

	StreamGraph graph = stream_graph_from(interval_from(0.0, 4.0), nodes, links);

	TemporalNodeRefVector nodes_present_at_0 = get_nodes_present_at(&graph, 0.0);
	TemporalNodeRefVector nodes_present_at_2 = get_nodes_present_at(&graph, 2.0);
	TemporalNodeRefVector nodes_present_at_2_5 = get_nodes_present_at(&graph, 2.5);

	TemporalNodeRef nodeA_r = &nodes.array[0];
	TemporalNodeRef nodeB_r = &nodes.array[1];
	TemporalNodeRef nodeC_r = &nodes.array[2];

	return EXPECT_ALL(EXPECT(TemporalNodeRefVector_contains(nodes_present_at_0, nodeA_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_0, nodeB_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_0, nodeC_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_2, nodeA_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_2, nodeB_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_2, nodeC_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_2_5, nodeA_r)),
					  EXPECT(!TemporalNodeRefVector_contains(nodes_present_at_2_5, nodeB_r)),
					  EXPECT(TemporalNodeRefVector_contains(nodes_present_at_2_5, nodeC_r)));
}

bool test_init_neighbors() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();

	TemporalNode nodeA = TemporalNode_new("A", IntervalVector_new());
	TemporalNode nodeB = TemporalNode_new("B", IntervalVector_new());

	TemporalNodeVector_push(&nodes, nodeA);
	TemporalNodeVector_push(&nodes, nodeB);

	Link linkAB = (Link){.nodes = {&nodes.array[0], &nodes.array[1]}};
	IntervalVector_push(&linkAB.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, linkAB);

	StreamGraph graph = stream_graph_from(interval_from(0.0, 1.0), nodes, links);

	printf("A links: %zu\n", graph.temporal_nodes.array[0].links.size);
	printf("B links: %zu\n", graph.temporal_nodes.array[1].links.size);

	return EXPECT(graph.temporal_nodes.array[0].links.size == 1) &&
		   EXPECT(graph.temporal_nodes.array[1].links.size == 1) &&
		   EXPECT(graph.temporal_nodes.array[0].links.array[0] == &graph.links.array[0]) &&
		   EXPECT(graph.temporal_nodes.array[1].links.array[0] == &graph.links.array[0]);
}

int main() {
	return test("StreamGraph", &(Test){"create_empty", test_create_empty},
				&(Test){"create_1", test_create_1},
				&(Test){"create_multiple_intervals", test_create_multiple_intervals},
				&(Test){"create_multiple_links", test_create_multiple_links},
				&(Test){"is_node_present_at", test_is_node_present_at},
				&(Test){"is_node_not_present_at", test_is_node_not_present_at},
				&(Test){"is_link_present_at", test_is_link_present_at},
				&(Test){"is_link_not_present_at", test_is_link_not_present_at},
				&(Test){"get_nodes_present_at", test_get_nodes_present_at},
				&(Test){"init_neighbors", test_init_neighbors}, NULL)
			   ? 0
			   : 1;
}