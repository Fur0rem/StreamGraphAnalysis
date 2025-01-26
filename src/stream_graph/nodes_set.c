#define SGA_INTERNAL

#include "nodes_set.h"
#include "../utils.h"
#include <stddef.h>

NodesSet NodesSet_alloc(size_t nb_nodes) {
	NodesSet set = {
	    .nb_nodes = nb_nodes,
	    .nodes    = MALLOC(nb_nodes * sizeof(SGA_Node)),
	};
	return set;
}

// Assumes that there is at least one interval
size_t SGA_Node_first_appearance(SGA_Node* node) {
	return node->presence.intervals[0].start;
}

// Assumes that there is at least one interval
size_t SGA_Node_last_disappearance(SGA_Node* node) {
	return node->presence.intervals[node->presence.nb_intervals - 1].end;
}