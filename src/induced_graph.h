#ifndef SGA_INDUCED_GRAPH_H
#define SGA_INDUCED_GRAPH_H

#include "stream_graph.h"
#include <stddef.h>

typedef struct {
	StreamGraph* stream_graph;
	size_t current_event;
	size_t current_node;
	// Function that takes itself and returns the next node (size_t*)
	// Obliged to put void* because of the circular dependency
	size_t* (*next)(void*);
} SGA_NodesPresentAtT;

SGA_NodesPresentAtT SGA_get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);

#define FOR_EACH_NODE_AT_TIME(stream_graph, iterated, t)                                                               \
	SGA_NodesPresentAtT nodes_present_at_t = SGA_get_nodes_present_at_t(stream_graph, t);                              \
	for (size_t * (iterated) = nodes_present_at_t.next(&nodes_present_at_t); (iterated) != NULL;                       \
		 (iterated) = nodes_present_at_t.next(&nodes_present_at_t))

typedef struct {
	StreamGraph* stream_graph;
	size_t current_event;
	size_t current_link;
	// Function that takes itself and returns the next node (size_t*)
	// Obliged to put void* because of the circular dependency
	size_t* (*next)(void*);
} SGA_LinksPresentAtT;

SGA_LinksPresentAtT SGA_get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

#define FOR_EACH_LINK_AT_TIME(stream_graph, iterated, t)                                                               \
	SGA_LinksPresentAtT links_present_at_t = SGA_get_links_present_at_t(stream_graph, t);                              \
	for (size_t * (iterated) = links_present_at_t.next(&links_present_at_t); (iterated) != NULL;                       \
		 (iterated) = links_present_at_t.next(&links_present_at_t))

#endif // SGA_INDUCED_GRAPH_H