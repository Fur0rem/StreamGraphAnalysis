#include "metrics.h"
#include "induced_graph.h"
#include "interval.h"
#include "iterators.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream_functions.h"
#include "stream_graph.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

// TODO : Confirm this
// I hope gcc or clang does this optimisation :
// ```
// int a = 0;
// int b = 0;
// if (condition) {
//     a = 5;
// }
// if (condition) {
//     b = 5;
// }
// ```
// into
// ```
// int a = 0;
// int b = 0;
// if (condition) {
//     a = 5;
//     b = 5;
// }
// ```
// Otherwise the performance is going to be baaaaaaaaaaaaaaaaaaaaad
#define CATCH_METRICS_IMPLEM(function, stream)                                                                         \
	switch (stream.type) {                                                                                             \
		case FULL_STREAM_GRAPH: {                                                                                      \
			if (FullStreamGraph_metrics_functions.function != NULL) {                                                  \
				return FullStreamGraph_metrics_functions.function(stream.stream);                                      \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case LINK_STREAM: {                                                                                            \
			if (LinkStream_metrics_functions.function != NULL) {                                                       \
				return LinkStream_metrics_functions.function(stream.stream);                                           \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
	}                                                                                                                  \
	printf("COULD NOT CATCH " #function "\n");

size_t cardinalOfT(Stream stream) {
	CATCH_METRICS_IMPLEM(cardinalOfT, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	Interval lifespan = stream_functions.lifespan(stream.stream);
	return Interval_size(lifespan);
}

size_t cardinalOfV(NodesIterator nodes) {
	size_t count = 0;
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		count++;
	}
	return count;
}

size_t cardinalOfW(NodesIterator nodes) {
	size_t count = 0;
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, nodes.stream_graph);
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		TimesIterator times = stream_functions.times_node_present(nodes.stream_graph.stream, node_id);
		count += total_time_of(times);
	}
	return count;
}

double coverage_stream(Stream stream) {
	CATCH_METRICS_IMPLEM(coverage, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	size_t w = cardinalOfW(nodes);
	nodes = stream_functions.nodes_set(stream.stream);
	size_t t = cardinalOfT(stream);
	size_t v = cardinalOfV(nodes);

	return (double)w / (double)(t * v);
}

double node_duration_stream(Stream stream) {
	CATCH_METRICS_IMPLEM(node_duration, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	size_t w = cardinalOfW(nodes);
	size_t v = cardinalOfV(nodes);
	size_t scaling = stream_functions.scaling(stream.stream);
	return (double)w / (double)(v * scaling);
}