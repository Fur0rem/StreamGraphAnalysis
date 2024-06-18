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

#define GET_BASE_FUNC(variable, function, stream_var, args)                                                            \
	({                                                                                                                 \
		printf("GET_BASE_FUNC\n");                                                                                     \
		switch (stream_var.type) {                                                                                     \
			case FULL_STREAM_GRAPH: {                                                                                  \
				printf("FULL_STREAM_GRAPH\n");                                                                         \
				assert(full_stream_graph_base_functions.function != NULL);                                             \
				printf("assert passed : %p\n", full_stream_graph_base_functions.function);                             \
				variable = full_stream_graph_base_functions.function args;                                             \
				printf("got variable\n");                                                                              \
				break;                                                                                                 \
			}                                                                                                          \
			case LINK_STREAM: {                                                                                        \
				printf("LINK_STREAM\n");                                                                               \
				variable = link_stream_base_functions.function args;                                                   \
				break;                                                                                                 \
			}                                                                                                          \
			default: {                                                                                                 \
				printf("EIZHUEEZIIEZHEHE");                                                                            \
				assert(0);                                                                                             \
			}                                                                                                          \
		}                                                                                                              \
		variable;                                                                                                      \
	})

/*size_t cardinalOfT(TimesIterator times) {
	size_t count = 0;
	FOR_EACH(Interval, interval, times, interval.start != SIZE_MAX) {
		count += Interval_size(interval);
	}
	return count;
}*/

size_t cardinalOfT(TimesIterator times_but_long_for_clang) {
	size_t count = 0;
	printf("cardinalOfT\n");
	printf("times_but_long_for_clang %p\n", &times_but_long_for_clang);
	printf("times_but_long_for_clang next %p\n", times_but_long_for_clang.next);
	for (Interval interval = (times_but_long_for_clang).next(&(times_but_long_for_clang));
		 (interval.start != (18446744073709551615UL)) || ({
			 (times_but_long_for_clang).destroy(&(times_but_long_for_clang));
			 0;
		 });
		 (interval) = (times_but_long_for_clang).next(&(times_but_long_for_clang))) {
		printf("Interval : [%lu, %lu]\n", interval.start, interval.end);
		count += Interval_size(interval);
	}
	return count;
}

size_t cardinalOfV(NodesIterator nodes) {
	size_t count = 0;
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		count++;
	}
	return count;
}

// TODO : why doesnt get base func work here
size_t cardinalOfW(NodesIterator nodes) {
	printf("cardinalOfW\n");
	size_t count = 0;
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		printf("Node %lu\n", node_id);
		/*printf("func %p\n", full_stream_graph_base_functions.times_node_present);
		TimesIterator times =
			GET_BASE_FUNC(times, times_node_present, nodes.stream_graph, (&nodes.stream_graph, node_id));*/
		TimesIterator times = full_stream_graph_base_functions.times_node_present(nodes.stream_graph.stream, node_id);
		count += cardinalOfT(times);
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
	// TRY_HIJACK(coverage, stream);
	NodesIterator nodes = GET_BASE_FUNC(nodes, nodes_set, stream, (&stream));
	size_t w = cardinalOfW(nodes);
	size_t t = cardinalOfT(full_stream_graph_base_functions.times_node_present(stream.stream, 0));
	size_t v = cardinalOfV(nodes);

	return (double)w / (double)(t * v);
}

double node_duration_stream(stream_t stream) {
	// TRY_HIJACK(node_duration, stream);
	NodesIterator nodes = GET_BASE_FUNC(nodes, nodes_set, stream, (&stream));
	size_t w = cardinalOfW(nodes);
	size_t v = cardinalOfV(nodes);
	size_t scaling = GET_BASE_FUNC(scaling, scaling, stream, (&stream));
	return (double)w / (double)(v * scaling);
}