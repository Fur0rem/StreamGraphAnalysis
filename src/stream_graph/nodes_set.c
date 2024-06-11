#include "nodes_set.h"
#include <stddef.h>

size_t SGA_TemporalNodesSet_size(TemporalNodesSet set) {
	size_t size = 0;
	for (size_t i = 0; i < set.nb_nodes; i++) {
		for (size_t j = 0; j < set.nodes[i].nb_intervals; j++) {
			size += Interval_size(set.nodes[i].present_at[j]);
		}
	}
	return size;
}