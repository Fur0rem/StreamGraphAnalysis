#ifndef SGA_INDUCED_GRAPH_H
#define SGA_INDUCED_GRAPH_H

#include "stream_graph.h"
#include <stddef.h>

// typedef size_t NodesPresentAtTimeIter;
// typedef size_t LinksPresentAtTimeIter;

typedef struct {
	StreamGraph* stream_graph;
	size_t current_event;
	size_t current_node;
} SGA_NodesPresentAtT;

typedef struct {
	StreamGraph* stream_graph;
	size_t current_event;
	size_t current_link;
} SGA_LinksPresentAtT;

SGA_NodesPresentAtT SGA_get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);
SGA_LinksPresentAtT SGA_get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

size_t* SGA_NodesPresentAtT_next(SGA_NodesPresentAtT* nodes_iter);
size_t* SGA_LinksPresentAtT_next(SGA_LinksPresentAtT* links_iter);

#define FOR_EACH_NODE_AT_TIME(stream_graph, iterated, t)                                                               \
	SGA_NodesPresentAtT nodes_present_at_t = SGA_get_nodes_present_at_t(stream_graph, t);                              \
	for (size_t * (iterated) = SGA_NodesPresentAtT_next(&nodes_present_at_t); (iterated) != NULL;                      \
		 (iterated) = SGA_NodesPresentAtT_next(&nodes_present_at_t))

#endif // SGA_INDUCED_GRAPH_H