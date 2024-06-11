#include "measures.h"
#include "stream_graph.h"

size_t SGA_size_of_lifespan(StreamGraph* sg) {
	return Interval_size(
		Interval_from(SGA_StreamGraph_lifespan_begin(sg), SGA_StreamGraph_lifespan_end(sg)));
}

double SGA_number_of_nodes(StreamGraph* sg) {
	return (double)SGA_TemporalNodesSet_size(sg->nodes) / (double)SGA_size_of_lifespan(sg);
}

double SGA_coverage(StreamGraph* sg) {
	size_t size_of_nodes = SGA_TemporalNodesSet_size(sg->nodes);
	size_t max_possible = sg->nodes.nb_nodes * SGA_size_of_lifespan(sg);
	return (double)size_of_nodes / (double)max_possible;
}