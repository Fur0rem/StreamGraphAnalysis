#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

#include "stream_graph.h"
#include <stddef.h>

typedef struct {
	StreamGraph* stream_graph;
	size_t current_event;
	size_t current_node;
	// Function that takes itself and returns the next node (size_t*)
	// Obliged to put void* because of the circular dependency
	size_t* (*next)(void*);
} NodesPresentAtT;

NodesPresentAtT get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);

#define FOR_EACH_NODE_AT_TIME(stream_graph, iterated, t)                                                               \
	NodesPresentAtT nodes_present_at_t = get_nodes_present_at_t(stream_graph, t);                                      \
	for (size_t * (iterated) = nodes_present_at_t.next(&nodes_present_at_t); (iterated) != NULL;                       \
		 (iterated) = nodes_present_at_t.next(&nodes_present_at_t))

typedef struct {
	StreamGraph* stream_graph;
	size_t current_event;
	size_t current_link;
	// Function that takes itself and returns the next node (size_t*)
	// Obliged to put void* because of the circular dependency
	size_t* (*next)(void*);
} LinksPresentAtT;

LinksPresentAtT get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

#define FOR_EACH_LINK_AT_TIME(stream_graph, iterated, t)                                                               \
	LinksPresentAtT links_present_at_t = get_links_present_at_t(stream_graph, t);                                      \
	for (size_t * (iterated) = links_present_at_t.next(&links_present_at_t); (iterated) != NULL;                       \
		 (iterated) = links_present_at_t.next(&links_present_at_t))

#endif // INDUCED_GRAPH_H