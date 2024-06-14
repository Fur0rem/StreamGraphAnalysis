#ifndef LINK_STREAM_H
#define LINK_STREAM_H

#include "metrics_generic.h"
#include "stream_graph.h"
typedef struct {
	StreamGraph* underlying_stream_graph;
} LinkStream;

LinkStream LinkStream_from(StreamGraph* stream_graph);

extern BaseGenericFunctions link_stream_base_functions;
extern HijackedGenericFunctions link_stream_hijacked_functions;

#endif // LINK_STREAM_H