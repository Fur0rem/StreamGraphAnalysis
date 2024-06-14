#include "full_stream_graph.h"
#include "metrics_generic.h"
#include "stream_graph.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

FullStreamGraph FullStreamGraph_from(StreamGraph* stream_graph) {
	FullStreamGraph full_stream_graph = (FullStreamGraph){
		.underlying_stream_graph = stream_graph,
	};
	return full_stream_graph;
}

size_t FSG_cardinal_of_W(FullStreamGraph* fsg) {
	size_t size = 0;
	StreamGraph* sg = fsg->underlying_stream_graph;
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		size += IntervalsSet_size(sg->nodes.nodes[i].presence);
	}
	return size;
}

size_t FSG_cardinal_of_V(FullStreamGraph* fsg) {
	return fsg->underlying_stream_graph->nodes.nb_nodes;
}

size_t FSG_cardinal_of_T(FullStreamGraph* fsg) {
	return Interval_size(Interval_from(StreamGraph_lifespan_begin(fsg->underlying_stream_graph),
									   StreamGraph_lifespan_end(fsg->underlying_stream_graph)));
}

BaseGenericFunctions full_stream_graph_base_functions = {
	.cardinalOfW = (size_t(*)(void*))FSG_cardinal_of_W,
	.cardinalOfV = (size_t(*)(void*))FSG_cardinal_of_V,
	.cardinalOfT = (size_t(*)(void*))FSG_cardinal_of_T,
};

HijackedGenericFunctions full_stream_graph_hijacked_functions = {
	.coverage = NULL,
};