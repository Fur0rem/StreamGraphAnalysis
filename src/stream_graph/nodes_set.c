#include "nodes_set.h"
#include "../utils.h"
#include <stddef.h>

TemporalNodesSet SGA_TemporalNodesSet_alloc(size_t nb_nodes) {
	TemporalNodesSet set;
	set.nb_nodes = nb_nodes;
	set.nodes = MALLOC(nb_nodes * sizeof(TemporalNode));
	return set;
}

// Assumes that there is at least one interval
size_t SGA_TemporalNode_first_appearance(TemporalNode* node) {
	return node->presence.intervals[0].start;
}

// Assumes that there is at least one interval
size_t SGA_TemporalNode_last_disappearance(TemporalNode* node) {
	return node->presence.intervals[node->presence.nb_intervals - 1].end;
}