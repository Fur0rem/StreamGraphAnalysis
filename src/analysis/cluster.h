/**
 * @file src/analysis/cluster.h
 * @brief Clusters in a stream
 * <br>
 * Clusters are a subset of the set of nodes
 */

#ifndef SGA_CLUSTER_H
#define SGA_CLUSTER_H

#include "../stream_data_access/node_access.h"

/**
 * @brief A node in a cluster
 */
typedef struct {
	SGA_NodeId node_id;		///< The id of the node.
	SGA_IntervalArrayList presence; ///< The intervals where the node is present.
} SGA_ClusterNode;

/**
 * @brief Creates a new cluster node
 * @param[in] node_id The id of the node
 * @param[in] presence The intervals where the node is present. Takes ownership of it and therefore should not be used after this call
 */
SGA_ClusterNode SGA_ClusterNode_from(SGA_NodeId node_id, SGA_IntervalArrayList presence);

DeclareDestroy(SGA_ClusterNode);
DeclareEquals(SGA_ClusterNode);
DeclareToString(SGA_ClusterNode);

DeclareArrayList(SGA_ClusterNode);
DeclareArrayListDeriveRemove(SGA_ClusterNode);
DeclareArrayListDeriveEquals(SGA_ClusterNode);
DeclareArrayListDeriveToString(SGA_ClusterNode);

/**
 * @brief Creates an empty cluster node
 * @param[in] node_id The id of the node
 * @return An empty cluster node
 */
SGA_ClusterNode SGA_ClusterNode_empty(SGA_NodeId node_id);

/**
 * @brief Creates a cluster node with the given presence
 * @param[in] node_id The id of the node
 * @param[in] presence The intervals where the node is present. Takes ownership of it and therefore should not be used after this call
 * @return A cluster node with the given presence
 */
SGA_ClusterNode SGA_ClusterNode_with_presence(SGA_NodeId node_id, SGA_IntervalArrayList presence);

/**
 * @brief Creates a cluster node present in one interval
 * @param[in] node_id The id of the node
 * @param[in] interval The interval where the node is present
 * @return A cluster node present in one interval
 */
SGA_ClusterNode SGA_ClusterNode_single_interval(SGA_NodeId node_id, SGA_Interval interval);

/**
 * @brief Adds an interval to the presence of the node
 * @param[in, out] node The node to add the interval to
 * @param[in] interval The interval to add
 */
void SGA_ClusterNode_add_interval(SGA_ClusterNode* node, SGA_Interval interval);

/**
 * @brief A cluster of nodes
 */
typedef struct {
	SGA_ClusterNodeArrayList nodes; ///< The nodes in the cluster.
} SGA_Cluster;

/**
 * @brief Creates a new cluster from the nodes
 * @param[in] nodes The nodes in the cluster. Takes ownership of it and therefore should not be used after this call
 */
SGA_Cluster SGA_Cluster_from_nodes(SGA_ClusterNodeArrayList nodes);

DeclareEquals(SGA_Cluster);
DeclareDestroy(SGA_Cluster);
DeclareToString(SGA_Cluster);

/**
 * @brief Creates an empty cluster
 * @return An empty cluster
 */
SGA_Cluster SGA_Cluster_empty();

/**
 * @brief Adds a node to the cluster
 * Checks and panics in debug mode if the node is already in the cluster. Does not check in release mode.
 * @param[in, out] cluster The cluster to add the node to
 * @param[in] node The node to add
 */
void SGA_Cluster_add_node(SGA_Cluster* cluster, SGA_ClusterNode node);

/**
 * @brief Checks if a node is in the cluster
 * @param[in] cluster The cluster to check
 * @param[in] node_id The id of the node to check
 * @return true if the node is in the cluster, false otherwise
 */
bool SGA_Cluster_contains_node(const SGA_Cluster* cluster, SGA_NodeId node_id);

/**
 * @brief Simplifies the intervals of all the nodes in the cluster
 * It merges the intervals and sorts them to have a more compact representation
 * @param cluseter[in, out] The cluster to simplify
 */
void SGA_Cluster_simplify(SGA_Cluster* cluseter);

#endif // SGA_CLUSTER_H