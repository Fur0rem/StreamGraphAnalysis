#include "nodes_set.h"
#include "../utils.h"
#include <stddef.h>

TemporalNodesSet SGA_TemporalNodesSet_alloc(size_t nb_nodes) {
	TemporalNodesSet set;
	set.nb_nodes = nb_nodes;
	set.nodes = MALLOC(nb_nodes * sizeof(TemporalNode));
	return set;
}