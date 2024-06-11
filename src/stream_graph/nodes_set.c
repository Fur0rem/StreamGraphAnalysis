#include "nodes_set.h"
#include "../utils.h"
#include <stddef.h>

TemporalNodesSet SGA_TemporalNodesSet_alloc(size_t nb_nodes) {
	TemporalNodesSet set;
	set.nb_nodes = nb_nodes;
	set.nodes = MALLOC(nb_nodes * sizeof(TemporalNode));
	return set;
}

size_t SGA_TemporalNodesSet_size(TemporalNodesSet set) {
	size_t size = 0;
	for (size_t i = 0; i < set.nb_nodes; i++) {
		for (size_t j = 0; j < set.nodes[i].nb_intervals; j++) {
			size += Interval_size(set.nodes[i].present_at[j]);
		}
	}
	return size;
}