#include "measures.h"
#include "stream_graph.h"
#include <stddef.h>

size_t SGA_size_of_lifespan(StreamGraph* sg) {
	return Interval_size(
		Interval_from(SGA_StreamGraph_lifespan_begin(sg), SGA_StreamGraph_lifespan_end(sg)));
}

size_t SGA_NodesSet_size(StreamGraph* sg) {
	size_t size = 0;
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		for (size_t j = 0; j < sg->nodes.nodes[i].nb_intervals; j++) {
			size += Interval_size(sg->nodes.nodes[i].present_at[j]);
		}
	}
	return size;
}

size_t SGA_LinksSet_size(StreamGraph* sg) {
	size_t size = 0;
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		for (size_t j = 0; j < sg->links.links[i].nb_intervals; j++) {
			size += Interval_size(sg->links.links[i].present_at[j]);
		}
	}
	return size;
}

double SGA_number_of_temporal_nodes(StreamGraph* sg) {
	return (double)SGA_NodesSet_size(sg) / (double)SGA_size_of_lifespan(sg);
}

size_t SGA_number_of_nodes(StreamGraph* sg) {
	return sg->nodes.nb_nodes;
}

double SGA_coverage(StreamGraph* sg) {
	size_t size_of_nodes = SGA_NodesSet_size(sg);
	size_t max_possible = sg->nodes.nb_nodes * SGA_size_of_lifespan(sg);
	return (double)size_of_nodes / (double)max_possible;
}

double SGA_number_of_links(StreamGraph* sg) {
	return (double)SGA_LinksSet_size(sg) / (double)SGA_size_of_lifespan(sg);
}

double SGA_node_duration(StreamGraph* sg) {
	return (double)SGA_NodesSet_size(sg) / (double)SGA_number_of_nodes(sg);
}

size_t SGA_size_unord_pairs_set_itself(size_t n) {
	return n * (n - 1) / 2;
}

double SGA_link_duration(StreamGraph* sg) {
	return (double)SGA_LinksSet_size(sg) /
		   (double)SGA_size_unord_pairs_set_itself(SGA_number_of_nodes(sg));
}