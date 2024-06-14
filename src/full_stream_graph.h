#ifndef FULL_STREAM_GRAPH_H
#define FULL_STREAM_GRAPH_H

#include "metrics_generic.h"
#include "stream_graph.h"

typedef struct {
	StreamGraph* underlying_stream_graph;
} FullStreamGraph;

extern BaseGenericFunctions full_stream_graph_base_functions;
extern HijackedGenericFunctions full_stream_graph_hijacked_functions;

FullStreamGraph FullStreamGraph_from(StreamGraph* stream_graph);

#endif // FULL_STREAM_GRAPH_H