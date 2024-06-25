#ifndef FULL_STREAM_GRAPH_H
#define FULL_STREAM_GRAPH_H

#include "../metrics.h"
#include "../stream_functions.h"
#include "../stream_graph.h"

typedef struct {
	StreamGraph* underlying_stream_graph;
} FullStreamGraph;

extern const StreamFunctions FullStreamGraph_stream_functions;
extern const MetricsFunctions FullStreamGraph_metrics_functions;

FullStreamGraph FullStreamGraph_from(StreamGraph* stream_graph);
Stream FSG_from(StreamGraph* stream_graph);
void FSG_destroy(Stream* stream);
Link FullStreamGraph_nth_link(FullStreamGraph* full_stream_graph, LinkId link_id);

#endif // FULL_STREAM_GRAPH_H