#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

#include "stream_graph.h"
#include <stddef.h>

NodesIterator get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);

typedef struct {
	size_t current_event;
	size_t current_node;
} NodesPresentAtTIterator;

#define FOR_EACH_NODE(iterator, iterated)                                                                              \
	for (size_t iterated = (iterator).next(&(iterator)); (iterated) != SIZE_MAX || ({                                  \
															 (iterator).destroy(&(iterator));                          \
															 0;                                                        \
														 });                                                           \
		 (iterated) = (iterator).next(&(iterator)))

LinksIterator get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

typedef struct {
	size_t current_event;
	size_t current_link;
} LinksPresentAtTIterator;

#define FOR_EACH_LINK(iterator, iterated)                                                                              \
	for (size_t iterated = (iterator).next(&(iterator)); (iterated) != SIZE_MAX || ({                                  \
															 (iterator).destroy(&(iterator));                          \
															 0;                                                        \
														 });                                                           \
		 (iterated) = (iterator).next(&(iterator)))

#endif // INDUCED_GRAPH_H