#include "metrics_generic.h"
#include "full_stream_graph.h"
#include "link_stream.h"
#include "stream_graph.h"
#include <stddef.h>

double coverage_stream(stream_t* stream) {
	size_t w, t, v;
	switch (stream->type) {
		case FULL_STREAM_GRAPH: {
			if (full_stream_graph_hijacked_functions.coverage != NULL) {
				return full_stream_graph_hijacked_functions.coverage(stream->stream);
			}
			FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
			w = full_stream_graph_base_functions.cardinalOfW(fsg);
			t = full_stream_graph_base_functions.cardinalOfT(fsg);
			v = full_stream_graph_base_functions.cardinalOfV(fsg);
			break;
		}
		case LINK_STREAM: {
			if (link_stream_hijacked_functions.coverage != NULL) {
				return link_stream_hijacked_functions.coverage(stream->stream);
			}
			LinkStream* ls = (LinkStream*)stream->stream;
			w = link_stream_base_functions.cardinalOfW(ls);
			t = link_stream_base_functions.cardinalOfT(ls);
			v = link_stream_base_functions.cardinalOfV(ls);
			break;
		}
	}
	return (double)w / (double)(t * v);
}