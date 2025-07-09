/**
 * @file src/analysis/cluster.c
 * @brief Implementation of clusters in a stream
 */

#include "cluster.h"

SGA_ClusterNode SGA_ClusterNode_from(SGA_NodeId node_id, SGA_IntervalArrayList presence) {
	return (SGA_ClusterNode){
	    .node_id  = node_id,
	    .presence = presence,
	};
}

void SGA_ClusterNode_destroy(SGA_ClusterNode self) {
	SGA_IntervalArrayList_destroy(self.presence);
}

String SGA_ClusterNode_to_string(const SGA_ClusterNode* self) {
	String str = String_with_capacity(80 + (self->presence.length * 10));
	String_append_formatted(&str, "%zu : ", self->node_id);
	String intervals_str = SGA_IntervalArrayList_to_string(&self->presence);
	String_concat_copy(&str, &intervals_str);
	String_destroy(intervals_str);
	return str;
}

bool SGA_ClusterNode_equals(const SGA_ClusterNode* left, const SGA_ClusterNode* right) {
	return left->node_id == right->node_id && SGA_IntervalArrayList_equals(&left->presence, &right->presence);
}

DefineArrayList(SGA_ClusterNode);
DefineArrayListDeriveRemove(SGA_ClusterNode);
DefineArrayListDeriveEquals(SGA_ClusterNode);
DefineArrayListDeriveToString(SGA_ClusterNode);

SGA_ClusterNode SGA_ClusterNode_empty(SGA_NodeId node_id) {
	return (SGA_ClusterNode){
	    .node_id  = node_id,
	    .presence = SGA_IntervalArrayList_new(),
	};
}

SGA_ClusterNode SGA_ClusterNode_with_presence(SGA_NodeId node_id, SGA_IntervalArrayList presence) {
	return (SGA_ClusterNode){
	    .node_id  = node_id,
	    .presence = presence,
	};
}

SGA_ClusterNode SGA_ClusterNode_single_interval(SGA_NodeId node_id, SGA_Interval interval) {
	SGA_ClusterNode node = SGA_ClusterNode_empty(node_id);
	SGA_IntervalArrayList_push(&node.presence, interval);
	return node;
}

void SGA_ClusterNode_add_interval(SGA_ClusterNode* node, SGA_Interval interval) {
	SGA_IntervalArrayList_push(&node->presence, interval);
}

void SGA_Cluster_destroy(SGA_Cluster self) {
	SGA_ClusterNodeArrayList_destroy(self.nodes);
}

String SGA_Cluster_to_string(const SGA_Cluster* cluster) {
	return SGA_ClusterNodeArrayList_to_string(&cluster->nodes);
}

bool SGA_Cluster_equals(const SGA_Cluster* left, const SGA_Cluster* right) {
	if (left->nodes.length != right->nodes.length) {
		return false;
	}

	if (!SGA_ClusterNodeArrayList_equals(&left->nodes, &right->nodes)) {
		return false;
	}

	return true;
}

SGA_Cluster SGA_Cluster_empty() {
	return (SGA_Cluster){
	    .nodes = SGA_ClusterNodeArrayList_new(),
	};
}

void SGA_Cluster_add_node(SGA_Cluster* cluster, SGA_ClusterNode node) {
	ASSERT(!SGA_Cluster_contains_node(cluster, node.node_id));
	SGA_ClusterNodeArrayList_push(&cluster->nodes, node);
}

bool SGA_Cluster_contains_node(const SGA_Cluster* cluster, SGA_NodeId node_id) {
	for (size_t i = 0; i < cluster->nodes.length; i++) {
		if (cluster->nodes.array[i].node_id == node_id) {
			return true;
		}
	}
	return false;
}

// OPTIMISE: Very naive implementation, could be improved
void SGA_Cluster_simplify(SGA_Cluster* cluster) {
	for (size_t i = 0; i < cluster->nodes.length; i++) {
		SGA_IntervalsSet intervals = SGA_IntervalsSet_from_arraylist(cluster->nodes.array[i].presence);
		SGA_IntervalsSet_sort(&intervals);
		SGA_IntervalsSet_merge(&intervals);
		cluster->nodes.array[i].presence = SGA_IntervalArrayList_from_intervals_set(intervals);
	}
}