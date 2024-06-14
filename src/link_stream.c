#include "link_stream.h"
#include "metrics_generic.h"
#include <stddef.h>
#include <stdio.h>

LinkStream LinkStream_from(StreamGraph* stream_graph) {
	LinkStream link_stream = (LinkStream){
		.underlying_stream_graph = stream_graph,
	};
	return link_stream;
}

size_t LS_cardinal_of_V(LinkStream* ls) {
	printf("called LS_cardinal_of_V\n");
	return ls->underlying_stream_graph->nodes.nb_nodes;
}

size_t LS_cardinal_of_T(LinkStream* ls) {
	return Interval_size(Interval_from(StreamGraph_lifespan_begin(ls->underlying_stream_graph),
									   StreamGraph_lifespan_end(ls->underlying_stream_graph)));
}

size_t LS_cardinal_of_W(LinkStream* ls) {
	return LS_cardinal_of_V(ls) * LS_cardinal_of_T(ls);
}

double LS_coverage(LinkStream* ls) {
	return 1.0;
}

size_t LS_scaling(LinkStream* ls) {
	return ls->underlying_stream_graph->scaling;
}

BaseGenericFunctions link_stream_base_functions = {
	.cardinalOfT = (size_t(*)(void*))LS_cardinal_of_T,
	.cardinalOfV = (size_t(*)(void*))LS_cardinal_of_V,
	.cardinalOfW = (size_t(*)(void*))LS_cardinal_of_W,
	.scaling = (size_t(*)(void*))LS_scaling,
};

HijackedGenericFunctions link_stream_hijacked_functions = {
	.coverage = (double (*)(void*))LS_coverage,
};