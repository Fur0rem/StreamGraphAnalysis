/**
 * @file src/stream_data_access/node_access.h
 * @brief Functions to access the nodes of a StreamGraph.
 */

#ifndef SGA_NODE_ACCESS_H
#define SGA_NODE_ACCESS_H

#include "../iterators.h"

#ifdef SGA_INTERNAL

/**
 * @brief Get all the neighbours of a node in a StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @param[in] node_id The id of the node.
 * @return An iterator over the links between the node and its neighbours.
 */
LinksIterator SGA_StreamGraph_neighbours_of_node(SGA_StreamGraph* stream_graph, NodeId node_id);

/**
 * @brief Get the times a node is present in a StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @param[in] node_id The id of the node.
 * @return An iterator over the times the node is present.
 */
TimesIterator SGA_StreamGraph_times_node_present(SGA_StreamGraph* stream_graph, NodeId node_id);

/**
 * @brief Get the set of nodes in a StreamGraph.
 * @param[in] stream_graph The StreamGraph to get the nodes of.
 * @return An iterator over all the nodes of the StreamGraph.
 */
NodesIterator SGA_StreamGraph_nodes_set(SGA_StreamGraph* stream_graph);

/**
 * @brief Get a node by its id.
 * @param[in] stream_graph The StreamGraph to get the node from.
 * @param[in] node_id The id of the node to get.
 * @return The node with the given id.
 */
Node SGA_StreamGraph_node_by_id(SGA_StreamGraph* stream_graph, NodeId node_id);

#endif // SGA_INTERNAL

#endif