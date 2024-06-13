#include "../interval.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	SGA_IntervalsSet presence;
	size_t nb_neighbours;
	EdgeId* neighbours;
} TemporalNode;

typedef struct {
	size_t nb_nodes;
	TemporalNode* nodes;
} TemporalNodesSet;

size_t SGA_TemporalNode_first_appearance(TemporalNode* node);
size_t SGA_TemporalNode_last_disappearance(TemporalNode* node);
TemporalNodesSet SGA_TemporalNodesSet_alloc(size_t nb_nodes);