#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

#include "iterators.h"
#include "stream_graph.h"
#include <stddef.h>

NodesIterator get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);

typedef struct {
	size_t current_event;
	size_t current_node;
} NodesPresentAtTIterator;

LinksIterator get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

typedef struct {
	size_t current_event;
	size_t current_link;
} LinksPresentAtTIterator;

#endif // INDUCED_GRAPH_H