#include "nodes_set.h"
#include "../utils.h"
#include <stddef.h>

TemporalNodesSet TemporalNodesSet_alloc(size_t nb_nodes) {
	TemporalNodesSet set = {
		.nb_nodes = nb_nodes,
		.nodes	  = MALLOC(nb_nodes * sizeof(TemporalNode)),
	};
	return set;
}

// Assumes that there is at least one interval
size_t TemporalNode_first_appearance(TemporalNode* node) {
	return node->presence.intervals[0].start;
}

// Assumes that there is at least one interval
size_t TemporalNode_last_disappearance(TemporalNode* node) {
	return node->presence.intervals[node->presence.nb_intervals - 1].end;
}