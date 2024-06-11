#include "../interval.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	size_t nb_intervals;
	Interval* present_at;
	size_t nb_neighbours;
	EdgeId* neighbours;
} TemporalNode;

typedef struct {
	size_t nb_nodes;
	TemporalNode* nodes;
} TemporalNodesSet;