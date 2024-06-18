#ifndef STREAM_GRAPH_NODES_SET_H
#define STREAM_GRAPH_NODES_SET_H

#include "../interval.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	IntervalsSet presence;
	size_t nb_neighbours;
	LinkId* neighbours;
} TemporalNode;

typedef struct {
	size_t nb_nodes;
	TemporalNode* nodes;
} TemporalNodesSet;

size_t TemporalNode_first_appearance(TemporalNode* node);
size_t TemporalNode_last_disappearance(TemporalNode* node);
TemporalNodesSet TemporalNodesSet_alloc(size_t nb_nodes);

#endif // STREAM_GRAPH_NODES_SET_H