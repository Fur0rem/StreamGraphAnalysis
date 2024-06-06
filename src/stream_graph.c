#include "stream_graph.h"
#include "interval.h"

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

Link* find_link(LinkVector links, size_t node1, size_t node2) {
	for (size_t i = 0; i < links.size; i++) {
		Link* link = &links.array[i];
		if ((link->nodes[0] == node1 && link->nodes[1] == node2) ||
			(link->nodes[0] == node2 && link->nodes[1] == node1)) {
			return link;
		}
	}
	return NULL;
}

bool are_nodes_linked_at(StreamGraph* graph, size_t node1, size_t node2, Time time) {
	Link* link = find_link(graph->links, node1, node2);
	if (link == NULL) {
		return false;
	}
	return is_link_present_at(*link, time);
}

StreamGraph* stream_graph_from(Interval lifespan, TemporalNodeVector nodes, LinkVector links) {
	StreamGraph* graph = malloc(sizeof(StreamGraph));
	graph->lifespan = lifespan;
	graph->temporal_nodes = nodes;
	graph->links = links;
	return graph;
}

char* stream_graph_to_string(StreamGraph* graph) {
	char* str = malloc(1000);
	str[0] = '\0';
	strcat(str, "StreamGraph {\n");
	Time start = graph->lifespan.start;
	Time end = graph->lifespan.end;
	char* time_str = malloc(100);
	char* interval_str = interval_to_string(interval_from(start, end));
	sprintf(time_str, "  Lifespan : %s", interval_str);
	free(interval_str);
	strcat(str, time_str);
	strcat(str, "\n");
	strcat(str, "  Nodes:\n");
	for (size_t i = 0; i < graph->temporal_nodes.size; i++) {
		TemporalNode node = graph->temporal_nodes.array[i];
		strcat(str, "    ");
		strcat(str, node.label);
		strcat(str, " @ { ");
		for (size_t j = 0; j < node.present_at.size; j++) {
			Interval interval = node.present_at.array[j];
			char* interval_str = interval_to_string(interval);
			sprintf(time_str, "%s ", interval_str);
			free(interval_str);
			strcat(str, time_str);
			if (j < node.present_at.size - 1) {
				strcat(str, ", ");
			}
		}
		strcat(str, "}\n");
	}

	strcat(str, "  Links:\n");
	for (size_t i = 0; i < graph->links.size; i++) {
		Link link = graph->links.array[i];
		strcat(str, "    ");
		strcat(str, "Link between ");
		strcat(str, graph->temporal_nodes.array[link.nodes[0]].label);
		strcat(str, " and ");
		strcat(str, graph->temporal_nodes.array[link.nodes[1]].label);
		strcat(str, " @ { ");
		for (size_t j = 0; j < link.present_at.size; j++) {
			Interval interval = link.present_at.array[j];
			char* interval_str = interval_to_string(interval);
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