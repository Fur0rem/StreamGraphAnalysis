#include "induced_graph.h"

InstantInducedGraph induced_graph_at_time(StreamGraph* graph, Time time) {
	TemporalNodeRefVector nodes = get_nodes_present_at(graph, time);
	LinkRefVector links = get_links_present_at(graph, time);
	InstantInducedGraph induced = {.time = time, .nodes = nodes, .links = links};
	return induced;
}

InducedGraph induced_graph(StreamGraph* graph) {
	InducedGraph induced = {.nodes = TemporalNodeRefVector_new(), .links = LinkRefVector_new()};
	for (size_t i = 0; i < graph->temporal_nodes.size; i++) {
		TemporalNode* node = &graph->temporal_nodes.array[i];
		TemporalNodeRefVector_push(&induced.nodes, node);
	}
	for (size_t i = 0; i < graph->links.size; i++) {
		Link* link = &graph->links.array[i];
		Time interval_sum = 0.0;
		for (size_t j = 0; j < link->present_at.size; j++) {
			Interval interval = link->present_at.array[j];
			interval_sum += interval_size(interval);
			if (interval_sum > 0.0) {
				LinkRefVector_push(&induced.links, link);
				break;
			}
		}
	}
	return induced;
}

char* InducedGraph_to_string(InducedGraph* induced) {
	char* str = malloc(1000);
	str[0] = '\0';
	strcat(str, "InducedGraph {\n");
	strcat(str, "  Nodes: {\n");
	for (size_t i = 0; i < induced->nodes.size; i++) {
		char* node_str = induced->nodes.array[i]->label;
		strcat(str, " ");
		strcat(str, node_str);
	}
	strcat(str, " }\n");
	strcat(str, "  Edges: {\n");
	for (size_t i = 0; i < induced->links.size; i++) {
		Link* link = induced->links.array[i];
		strcat(str, " ");
		strcat(str, link->nodes[0]->label);
		strcat(str, link->nodes[1]->label);
	}
	strcat(str, " }\n");
	strcat(str, "}\n");
	return str;
}

char* InstantInducedGraph_to_string(InstantInducedGraph* induced) {
	char* str = malloc(1000);
	str[0] = '\0';
	strcat(str, "InstantInducedGraph {\n");
	strcat(str, "  Time: ");
	char* time_str = malloc(100);
	sprintf(time_str, "%f", induced->time);
	strcat(str, time_str);
	strcat(str, "\n");
	strcat(str, "  Nodes: {\n");
	for (size_t i = 0; i < induced->nodes.size; i++) {
		char* node_str = induced->nodes.array[i]->label;
		strcat(str, " ");
		strcat(str, node_str);
	}
	strcat(str, " }\n");
	strcat(str, "  Edges: {\n");
	for (size_t i = 0; i < induced->links.size; i++) {
		Link* link = induced->links.array[i];
		strcat(str, " ");
		strcat(str, link->nodes[0]->label);
		strcat(str, link->nodes[1]->label);
	}
	strcat(str, " }\n");
	strcat(str, "}\n");
	free(time_str);
	return str;
}