#include "../src/stream_graph.h"
#include "../src/interval.h"

#include "test.h"

bool test_create_empty() {
	TemporalNodeVector nodes = TemporalNodeVector_new();
	LinkVector links = LinkVector_new();
	StreamGraph* graph = stream_graph_from(interval_from(0.0, 0.0), nodes, links);
	char* str = stream_graph_to_string(graph);
	printf("%s\n", str);
	free(str);
	return EXPECT(F_EQUALS(graph->lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph->lifespan.end, 0.0)) && EXPECT(graph->temporal_nodes.size == 0) &&
		   EXPECT(graph->links.size == 0);
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

	Link link = (Link){.nodes = {0, 1}};
	IntervalVector_push(&link.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, link);

	StreamGraph* graph = stream_graph_from(interval_from(0.0, 2.0), nodes, links);
	char* str = stream_graph_to_string(graph);
	printf("%s\n", str);
	free(str);

	return EXPECT(F_EQUALS(graph->lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph->lifespan.end, 2.0)) && EXPECT(graph->temporal_nodes.size == 2) &&
		   EXPECT(graph->links.size == 1);
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

	Link link = (Link){.nodes = {0, 1}};
	IntervalVector_push(&link.present_at, interval_from(0.0, 1.0));
	LinkVector_push(&links, link);

	StreamGraph* graph = stream_graph_from(interval_from(0.0, 4.0), nodes, links);
	char* str = stream_graph_to_string(graph);
	printf("%s\n", str);
	free(str);

	return EXPECT(F_EQUALS(graph->lifespan.start, 0.0)) &&
		   EXPECT(F_EQUALS(graph->lifespan.end, 4.0)) && EXPECT(graph->temporal_nodes.size == 2) &&
		   EXPECT(graph->links.size == 1);
}

int main() {
	return test("StreamGraph", &(Test){"create_empty", test_create_empty},
				&(Test){"create_1", test_create_1},
				&(Test){"create_multiple_intervals", test_create_multiple_intervals}, NULL)
			   ? 0
			   : 1;
}