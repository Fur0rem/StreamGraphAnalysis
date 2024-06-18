#ifndef FULL_STREAM_GRAPH_H
#define FULL_STREAM_GRAPH_H

#include "induced_graph.h"
#include "metrics_generic.h"
#include "stream_graph.h"

typedef struct {
	StreamGraph* underlying_stream_graph;
} FullStreamGraph;

const extern BaseGenericFunctions full_stream_graph_base_functions;
const extern HijackedGenericFunctions full_stream_graph_hijacked_functions;

FullStreamGraph FullStreamGraph_from(StreamGraph* stream_graph);
stream_t FSG_from(StreamGraph* stream_graph);

#endif // FULL_STREAM_GRAPH_H