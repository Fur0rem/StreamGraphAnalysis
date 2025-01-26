/**
 * @file src/stream_graph/nodes_set.h
 * @brief A node and a set of nodes of a StreamGraph.
 */

#ifndef SGA_STREAM_GRAPH_NODES_SET_H
#define SGA_STREAM_GRAPH_NODES_SET_H

#include "../interval.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The structure of a node in a StreamGraph.
 */
typedef struct {
	SGA_IntervalsSet presence; ///< All the time intervals the node is present.
	size_t nb_neighbours;	   ///< Number of neighbours of the node.
	SGA_LinkId* neighbours;	   ///< Array of neighbours of the node.
} SGA_Node;

/**
 * @brief Get the first time the node appears in the StreamGraph.
 * @param[in] node The node.
 */
size_t SGA_Node_first_appearance(SGA_Node* node);

/**
 * @brief Get the last time the node disappears in the StreamGraph.
 * @param[in] node The node.
 */
size_t SGA_Node_last_disappearance(SGA_Node* node);

/**
 * @brief The structure of a set of nodes in a StreamGraph.
 */
typedef struct {
	size_t nb_nodes; ///< Number of nodes in the set.
	SGA_Node* nodes; ///< Array of nodes.
} NodesSet;

#ifdef SGA_INTERNAL

/**
 * @brief Allocate a set of nodes.
 * @param[in] nb_nodes The number of nodes in the set.
 * @return The allocated set of nodes.
 */
NodesSet NodesSet_alloc(size_t nb_nodes);

#endif // SGA_INTERNAL

#endif // SGA_STREAM_GRAPH_NODES_SET_H