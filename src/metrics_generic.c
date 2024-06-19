#include "metrics_generic.h"
#include "full_stream_graph.h"
#include "induced_graph.h"
#include "interval.h"
#include "link_stream.h"
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
#define TRY_HIJACK(function, stream)                                                                                   \
	switch (stream.type) {                                                                                             \
		case FULL_STREAM_GRAPH: {                                                                                      \
			if (full_stream_graph_hijacked_functions.function != NULL) {                                               \
				return full_stream_graph_hijacked_functions.function(stream.stream);                                   \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case LINK_STREAM: {                                                                                            \
			if (link_stream_hijacked_functions.function != NULL) {                                                     \
				return link_stream_hijacked_functions.function(stream.stream);                                         \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
	}

#define GET_BASE_FUNCS(variable, stream_var)                                                                           \
	({                                                                                                                 \
		switch (stream_var.type) {                                                                                     \
			case FULL_STREAM_GRAPH: {                                                                                  \
				variable = full_stream_graph_base_functions;                                                           \
				break;                                                                                                 \
			}                                                                                                          \
			case LINK_STREAM: {                                                                                        \
				variable = link_stream_base_functions;                                                                 \
				break;                                                                                                 \
			}                                                                                                          \
		}                                                                                                              \
		variable;                                                                                                      \
	})

size_t cardinalOfTimes(TimesIterator times) {
	size_t count = 0;
	FOR_EACH(Interval, interval, times, interval.start != SIZE_MAX) {
		count += Interval_size(interval);
	}
	return count;
}

size_t cardinalOfT(stream_t stream) {
	TRY_HIJACK(cardinalOfT, stream);
	BaseGenericFunctions base_functions = GET_BASE_FUNCS(base_functions, stream);
	Interval lifespan = base_functions.lifespan(stream.stream);
	return Interval_size(lifespan);
	/*size_t count = 0;
	FOR_EACH(Interval, interval, times, interval.start != SIZE_MAX) {
		count += Interval_size(interval);
	}
	return count;*/
}

size_t cardinalOfV(NodesIterator nodes) {
	size_t count = 0;
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		count++;
	}
	return count;
}

size_t cardinalOfW(NodesIterator nodes) {
	printf("cardinalOfW\n");
	size_t count = 0;
	BaseGenericFunctions base_functions = GET_BASE_FUNCS(base_functions, nodes.stream_graph);
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		TimesIterator times = base_functions.times_node_present(nodes.stream_graph.stream, node_id);
		count += cardinalOfTimes(times);
	}
	return count;
}

/*size_t cardinalOfW(NodesIterator nodes) {
	size_t count = 0;
	FOR_EACH_NODE(nodes, node_id) {
		count += cardinalOfT(full_stream_graph_base_functions.times_node_present(nodes.stream_graph.stream, node_id));
	}
	return count;
}*/

double coverage_stream(stream_t stream) {
	TRY_HIJACK(coverage, stream);
	BaseGenericFunctions base_functions = GET_BASE_FUNCS(base_functions, stream);
	NodesIterator nodes = base_functions.nodes_set(stream.stream);
	size_t w = cardinalOfW(nodes);
	nodes = base_functions.nodes_set(stream.stream);
	size_t t = cardinalOfT(stream);
	size_t v = cardinalOfV(nodes);

	return (double)w / (double)(t * v);
}

double node_duration_stream(stream_t stream) {
	// TRY_HIJACK(node_duration, stream);
	BaseGenericFunctions base_functions = GET_BASE_FUNCS(base_functions, stream);
	NodesIterator nodes = base_functions.nodes_set(stream.stream);
	size_t w = cardinalOfW(nodes);
	size_t v = cardinalOfV(nodes);
	size_t scaling = base_functions.scaling(stream.stream);
	return (double)w / (double)(v * scaling);
}