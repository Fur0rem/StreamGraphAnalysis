#include "metrics_generic.h"
#include "full_stream_graph.h"
#include "link_stream.h"
#include "stream_graph.h"
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
	switch (stream->type) {                                                                                            \
		case FULL_STREAM_GRAPH: {                                                                                      \
			if (full_stream_graph_hijacked_functions.function != NULL) {                                               \
				return full_stream_graph_hijacked_functions.function(stream->stream);                                  \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case LINK_STREAM: {                                                                                            \
			if (link_stream_hijacked_functions.function != NULL) {                                                     \
				return link_stream_hijacked_functions.function(stream->stream);                                        \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
	}

#define GET_BASE_FUNC(variable, function, stream)                                                                      \
	({                                                                                                                 \
		switch (stream->type) {                                                                                        \
			case FULL_STREAM_GRAPH: {                                                                                  \
				variable = full_stream_graph_base_functions.function(stream->stream);                                  \
				break;                                                                                                 \
			}                                                                                                          \
			case LINK_STREAM: {                                                                                        \
				variable = link_stream_base_functions.function(stream->stream);                                        \
				break;                                                                                                 \
			}                                                                                                          \
		}                                                                                                              \
		variable;                                                                                                      \
	})

double coverage_stream(stream_t* stream) {
	TRY_HIJACK(coverage, stream);
	size_t w = GET_BASE_FUNC(w, cardinalOfW, stream);
	size_t t = GET_BASE_FUNC(t, cardinalOfT, stream);
	size_t v = GET_BASE_FUNC(v, cardinalOfV, stream);
	return (double)w / (double)(t * v);
}

double node_duration_stream(stream_t* stream) {
	TRY_HIJACK(node_duration, stream);
	size_t w = GET_BASE_FUNC(w, cardinalOfW, stream);
	size_t v = GET_BASE_FUNC(v, cardinalOfV, stream);
	size_t scaling = GET_BASE_FUNC(scaling, scaling, stream);
	return (double)w / (double)(v * scaling);
}