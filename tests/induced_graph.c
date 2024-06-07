#include "../src/induced_graph.h"
#include "../src/stream_graph.h"
#include "test.h"

// Example taken from Figure 1 of the paper, G(2)
StreamGraph example_S() {
	Interval graph_lifespan = (Interval){.start = 0, .end = 10};

	IntervalVector intervalsA = IntervalVector_new();
	IntervalVector_push(&intervalsA, (Interval){.start = 0, .end = 10});

	IntervalVector intervalsB = IntervalVector_new();
	IntervalVector_push(&intervalsB, (Interval){.start = 0, .end = 4});
	IntervalVector_push(&intervalsB, (Interval){.start = 5, .end = 10});

	IntervalVector intervalsC = IntervalVector_new();
	IntervalVector_push(&intervalsC, (Interval){.start = 4, .end = 9});

	IntervalVector intervalsD = IntervalVector_new();
	IntervalVector_push(&intervalsD, (Interval){.start = 1, .end = 3});

	TemporalNodePtrVector nodes = TemporalNodePtrVector_new();
	TemporalNodePtrVector_push(&nodes, TemporalNodePtr_new(strdup("a"), intervalsA));
	TemporalNodePtrVector_push(&nodes, TemporalNodePtr_new(strdup("b"), intervalsB));
	TemporalNodePtrVector_push(&nodes, TemporalNodePtr_new(strdup("c"), intervalsC));
	TemporalNodePtrVector_push(&nodes, TemporalNodePtr_new(strdup("d"), intervalsD));

	Link linkAB = link_from_labels(&nodes, "a", "b");
	Link linkBC = link_from_labels(&nodes, "b", "c");
	Link linkAC = link_from_labels(&nodes, "a", "c");
	Link linkBD = link_from_labels(&nodes, "b", "d");

	IntervalVector_push(&linkAB.present_at, (Interval){.start = 1, .end = 3});
	IntervalVector_push(&linkAB.present_at, (Interval){.start = 7, .end = 8});

	IntervalVector_push(&linkBC.present_at, (Interval){.start = 6, .end = 9});

	IntervalVector_push(&linkBD.present_at, (Interval){.start = 2, .end = 3});

	IntervalVector_push(&linkAC.present_at, (Interval){.start = 4.5, .end = 7.5});

	LinkVector links = LinkVector_new();
	LinkVector_push(&links, linkAB);
	LinkVector_push(&links, linkBC);
	LinkVector_push(&links, linkAC);
	LinkVector_push(&links, linkBD);

	return stream_graph_from(graph_lifespan, nodes, links);
}

bool test_induced_graph_at_time() {
	StreamGraph graph = example_S();
	InstantInducedGraph induced = induced_graph_at_time(&graph, 2);
	char* str = InstantInducedGraph_to_string(&induced);
	printf("%s\n", str);
	free(str);
	bool found_a = false, found_b = false, found_c = false, found_d = false;
	for (size_t i = 0; i < induced.nodes.size; i++) {
		TemporalNode* node = induced.nodes.array[i];
		if (strcmp(node->label, "a") == 0) {
			found_a = true;
		}
		else if (strcmp(node->label, "b") == 0) {
			found_b = true;
		}
		else if (strcmp(node->label, "c") == 0) {
			found_c = true;
		}
		else if (strcmp(node->label, "d") == 0) {
			found_d = true;
		}
	}
	return EXPECT(found_a) && EXPECT(found_b) && EXPECT(!found_c) && EXPECT(found_d);
}

bool test_induced_graph_S() {
	return true;
}

bool test_induced_graph_L() {
	return true;
}

int main() {
	return test("Induced Graph", &(Test){"induced_graph_at_time", test_induced_graph_at_time},
				&(Test){"induced_graph_S", test_induced_graph_S},
				&(Test){"induced_graph_L", test_induced_graph_L}, NULL)
			   ? 0
			   : 1;
}