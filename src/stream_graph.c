#include "stream_graph.h"
#include "interval.h"
#include "utils.h"

TemporalNode TemporalNode_new(const char* label, IntervalVector present_at) {
	TemporalNode node = {.label = label, .present_at = present_at, .links = LinkRefVector_new()};
	return node;
}

bool is_node_present_at(TemporalNode node, Time time) {
	for (size_t i = 0; i < node.present_at.size; i++) {
		if (interval_contains(node.present_at.array[i], time)) {
			return true;
		}
	}
	return false;
}

bool is_link_present_at(Link link, Time time) {
	for (size_t i = 0; i < link.present_at.size; i++) {
		if (interval_contains(link.present_at.array[i], time)) {
			return true;
		}
	}
	return false;
}

Link* find_link(LinkVector links, TemporalNode* node1, TemporalNode* node2) {
	for (size_t i = 0; i < links.size; i++) {
		Link* link = &links.array[i];
		if ((link->nodes[0] == node1 && link->nodes[1] == node2) ||
			(link->nodes[0] == node2 && link->nodes[1] == node1)) {
			return link;
		}
	}
	return NULL;
}

bool are_nodes_linked_at(StreamGraph* graph, TemporalNode* node1, TemporalNode* node2, Time time) {
	Link* link = find_link(graph->links, node1, node2);
	if (link == NULL) {
		return false;
	}
	return is_link_present_at(*link, time);
}

bool TemporalNode_equals(TemporalNode a, TemporalNode b) {
	if (a.present_at.size != b.present_at.size) {
		return false;
	}
	for (size_t i = 0; i < a.present_at.size; i++) {
		if (!Interval_equals(a.present_at.array[i], b.present_at.array[i])) {
			return false;
		}
	}
	return strcmp(a.label, b.label) == 0;
}

bool Link_equals(Link a, Link b) {
	if (a.present_at.size != b.present_at.size) {
		return false;
	}
	for (size_t i = 0; i < a.present_at.size; i++) {
		if (!Interval_equals(a.present_at.array[i], b.present_at.array[i])) {
			return false;
		}
	}
	return (a.nodes[0] == b.nodes[0] && a.nodes[1] == b.nodes[1]) ||
		   (a.nodes[0] == b.nodes[1] && a.nodes[1] == b.nodes[0]);
}

char* Link_to_string(Link link) {
	char* str = malloc(1000);
	str[0] = '\0';
	strcat(str, "Link between ");
	strcat(str, link.nodes[0]->label);
	strcat(str, " and ");
	strcat(str, link.nodes[1]->label);
	strcat(str, " @ { ");
	for (size_t j = 0; j < link.present_at.size; j++) {
		Interval interval = link.present_at.array[j];
		char* interval_str = Interval_to_string(interval);
		strcat(str, interval_str);
		free(interval_str);
		if (j < link.present_at.size - 1) {
			strcat(str, ", ");
		}
	}
	strcat(str, " }");
	return str;
}

void init_neighbors(StreamGraph* graph) {
	for (size_t i = 0; i < graph->temporal_nodes.size; i++) {
		TemporalNode* node = &graph->temporal_nodes.array[i];
		for (size_t j = 0; j < graph->links.size; j++) {
			printf("Link Node 0: %s : %p\n", graph->links.array[j].nodes[0]->label,
				   graph->links.array[j].nodes[0]);
			printf("Link Node 1: %s : %p\n", graph->links.array[j].nodes[1]->label,
				   graph->links.array[j].nodes[1]);

			printf("Node: %s : %p\n", node->label, node);

			Link link = graph->links.array[j];
			if (link.nodes[0] == node || link.nodes[1] == node) {
				LinkRefVector_push(&node->links, &graph->links.array[j]);
			}
		}
	}
}

StreamGraph stream_graph_from(Interval lifespan, TemporalNodeVector nodes, LinkVector links) {
	StreamGraph graph;
	graph.lifespan = lifespan;
	graph.temporal_nodes = nodes;
	graph.links = links;
	init_neighbors(&graph);
	return graph;
}

char* StreamGraph_to_string(StreamGraph* graph) {
	char* str = malloc(1000);
	str[0] = '\0';
	strcat(str, "StreamGraph {\n");
	Time start = graph->lifespan.start;
	Time end = graph->lifespan.end;
	char* time_str = malloc(100);
	char* interval_str = Interval_to_string(interval_from(start, end));
	sprintf(time_str, "  Lifespan : %s", interval_str);
	free(interval_str);
	strcat(str, time_str);
	strcat(str, "\n");
	strcat(str, "  Nodes:\n");
	for (size_t i = 0; i < graph->temporal_nodes.size; i++) {
		char* node_str = TemporalNode_to_string(graph->temporal_nodes.array[i]);
		strcat(str, "    ");
		strcat(str, node_str);
		strcat(str, "\n");
		free(node_str);
	}

	strcat(str, "  Links:\n");
	for (size_t i = 0; i < graph->links.size; i++) {
		Link link = graph->links.array[i];
		strcat(str, "    ");
		strcat(str, "Link between ");
		strcat(str, link.nodes[0]->label);
		strcat(str, " and ");
		strcat(str, link.nodes[1]->label);
		strcat(str, " @ { ");
		for (size_t j = 0; j < link.present_at.size; j++) {
			Interval interval = link.present_at.array[j];
			char* interval_str = Interval_to_string(interval);
			sprintf(time_str, "%s ", interval_str);
			free(interval_str);
			strcat(str, time_str);
			if (j < link.present_at.size - 1) {
				strcat(str, ", ");
			}
		}
		strcat(str, "}\n");
	}

	strcat(str, "}\n");
	free(time_str);
	return str;
}

TemporalNodeRefVector get_nodes_present_at(StreamGraph* graph, Time time) {
	TemporalNodeRefVector nodes = TemporalNodeRefVector_new();
	for (size_t i = 0; i < graph->temporal_nodes.size; i++) {
		TemporalNode* node = &graph->temporal_nodes.array[i];
		if (is_node_present_at(*node, time)) {
			TemporalNodeRefVector_push(&nodes, node);
		}
	}
	return nodes;
}

LinkRefVector get_links_present_at(StreamGraph* graph, Time time) {
	LinkRefVector links = LinkRefVector_new();
	for (size_t i = 0; i < graph->links.size; i++) {
		Link link = graph->links.array[i];
		if (is_link_present_at(link, time)) {
			LinkRefVector_push(&links, &link);
		}
	}
	return links;
}

char* TemporalNode_to_string(TemporalNode value) {
	char* str = malloc(1000);
	str[0] = '\0';
	strcat(str, value.label);
	strcat(str, " @ { ");
	for (size_t j = 0; j < value.present_at.size; j++) {
		Interval interval = value.present_at.array[j];
		char* interval_str = Interval_to_string(interval);
		strcat(str, interval_str);
		free(interval_str);
		if (j < value.present_at.size - 1) {
			strcat(str, ", ");
		}
	}
	strcat(str, " }");

	strcat(str, " Neighbors: { ");
	for (size_t j = 0; j < value.links.size; j++) {
		Link* link = value.links.array[j];
		const char* link_str =
			(link->nodes[0] == &value) ? link->nodes[1]->label : link->nodes[0]->label;
		strcat(str, link_str);
		if (j < value.links.size - 1) {
			strcat(str, ", ");
		}
	}
	strcat(str, " }");
	return str;
}

// TODO : better error handling
Link link_from_labels(TemporalNodeVector* nodes_set, const char* label1, const char* label2) {
	if (strcmp(label1, label2) == 0) {
		fprintf(stderr, "Error: Cannot link a node to itself\n");
		exit(1);
	}
	TemporalNode* node1 = NULL;
	TemporalNode* node2 = NULL;
	for (size_t i = 0; i < nodes_set->size; i++) {
		TemporalNode* node = &nodes_set->array[i];
		if (strcmp(node->label, label1) == 0) {
			node1 = node;
		}
		if (strcmp(node->label, label2) == 0) {
			node2 = node;
		}
	}
	if (node1 == NULL || node2 == NULL) {
		fprintf(stderr, "Error: Node not found\n");
		exit(1);
	}
	Link link = {.nodes = {node1, node2}, .present_at = IntervalVector_new()};
	return link;
}

DEFAULT_COMPARE(TemporalNodeRef)
DEFAULT_COMPARE(LinkRef)
DEFAULT_TO_STRING(TemporalNodeRef, "%p")
DEFAULT_TO_STRING(LinkRef, "%p")