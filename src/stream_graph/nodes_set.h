#include "../interval.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	IntervalsSet presence;
	size_t nb_neighbours;
	EdgeId* neighbours;
} TemporalNode;

typedef struct {
	size_t nb_nodes;
	TemporalNode* nodes;
} TemporalNodesSet;

TemporalNodesSet SGA_TemporalNodesSet_alloc(size_t nb_nodes);