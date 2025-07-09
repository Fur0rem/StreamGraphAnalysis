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

NodesSet NodesSet_from(size_t nb_nodes, SGA_IntervalArrayList* node_presences, SGA_LinkIdArrayList* neighbours_of_nodes) {
	NodesSet nodes_set = NodesSet_alloc(nb_nodes);

	// Fill the nodes set with the provided arrays
	for (size_t i = 0; i < nb_nodes; i++) {
		// Shrink the array to the actual size for better memory usage
		neighbours_of_nodes[i].array = realloc(neighbours_of_nodes[i].array, neighbours_of_nodes[i].length * sizeof(SGA_LinkId));

		// Create the node from the info in the arrays
		nodes_set.nodes[i] = (SGA_Node){
		    .presence	   = SGA_IntervalsSet_from_arraylist(node_presences[i]),
		    .neighbours	   = neighbours_of_nodes[i].array,
		    .nb_neighbours = neighbours_of_nodes[i].length,
		};
	}

	return nodes_set;
}

// Assumes that there is at least one interval
size_t SGA_Node_first_appearance(SGA_Node* node) {
	return node->presence.intervals[0].start;
}

// Assumes that there is at least one interval
size_t SGA_Node_last_disappearance(SGA_Node* node) {
	return node->presence.intervals[node->presence.nb_intervals - 1].end;
}
