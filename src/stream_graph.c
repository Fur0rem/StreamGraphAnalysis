#include "stream_graph.h"

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
        if ((link->nodes[0] == node1 && link->nodes[1] == node2) || (link->nodes[0] == node2 && link->nodes[1] == node1)) {
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

StreamGraph* stream_graph_from(Interval time, TemporalNodeVector nodes, LinkVector links) {
    StreamGraph* graph = malloc(sizeof(StreamGraph));
    graph->time = time;
    graph->temporal_nodes = nodes;
    graph->links = links;
    return graph;
}