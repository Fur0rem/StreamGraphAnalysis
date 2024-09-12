#ifndef LINK_STREAM_H
#define LINK_STREAM_H

#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

typedef struct {
	StreamGraph* underlying_stream_graph;
} LinkStream;

Stream LinkStream_from(StreamGraph* stream_graph);
void LinkStream_destroy(Stream stream);

extern const StreamFunctions LinkStream_stream_functions;
extern const MetricsFunctions LinkStream_metrics_functions;

#endif // LINK_STREAM_H