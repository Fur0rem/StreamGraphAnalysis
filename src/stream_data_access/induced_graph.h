/**
 * @file src/stream_data_access/induced_graph.h
 * @brief Functions to get the nodes and links present at a given time in a StreamGraph.
 */

#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

#include "../iterators.h"
#include "../stream.h"
#include <stddef.h>

#ifdef SGA_INTERNAL

/**
 * @brief Returns an iterator over the nodes present at the given time in the given StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @param[in] t The time.
 * @return An iterator over the nodes present at the given time in the given StreamGraph.
 */
SGA_NodesIterator SGA_StreamGraph_nodes_present_at(SGA_StreamGraph* stream_graph, SGA_Time time);

/**
 * @brief Returns an iterator over the links present at the given time in the given StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @param[in] t The time.
 * @return An iterator over the links present at the given time in the given StreamGraph.
 */
SGA_LinksIterator SGA_StreamGraph_links_present_at(SGA_StreamGraph* stream_graph, SGA_Time time);

#endif // SGA_INTERNAL

#endif // INDUCED_GRAPH_H