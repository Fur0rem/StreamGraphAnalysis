#ifndef CHUNK_STREAM_H
#define CHUNK_STREAM_H

#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/links_set.h"
#include "../stream_graph/nodes_set.h"
#include <stddef.h>

typedef struct {
	StreamGraph* underlying_stream_graph;
	Interval snapshot;
	BitArray nodes_present;
	BitArray links_present;
} ChunkStream;

ChunkStream ChunkStream_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
							 size_t time_end);
Stream CS_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start, size_t time_end);
void CS_destroy(Stream stream);

void filter_interval(Interval* interval, Interval snapshot);

extern const StreamFunctions ChunkStream_stream_functions;
extern const MetricsFunctions ChunkStream_metrics_functions;

#endif // CHUNK_STREAM_H