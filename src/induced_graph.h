#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

/**
 * @file induced_graph.h
 * @brief Functions to get the nodes and links present at a given time in a StreamGraph.
 */

#include "iterators.h"
#include "stream_graph.h"
#include <stddef.h>

/**
 * @brief Returns an iterator over the nodes present at the given time in the given StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @param[in] t The time.
 * @return An iterator over the nodes present at the given time in the given StreamGraph.
 */
NodesIterator get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t);

/**
 * @brief The data of the iterator over the nodes present at a given time in a StreamGraph.
 */
typedef struct {
	size_t current_event;
	size_t current_node;
} NodesPresentAtTIterator;

/**
 * @brief Returns an iterator over the links present at the given time in the given StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @param[in] t The time.
 * @return An iterator over the links present at the given time in the given StreamGraph.
 */
LinksIterator get_links_present_at_t(StreamGraph* stream_graph, TimeId t);

/**
 * @brief The data of the iterator over the links present at a given time in a StreamGraph.
 */
typedef struct {
	size_t current_event;
	size_t current_link;
} LinksPresentAtTIterator;

#endif // INDUCED_GRAPH_H