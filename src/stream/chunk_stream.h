#ifndef CHUNK_STREAM_H
#define CHUNK_STREAM_H

#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include <stddef.h>

typedef struct {
	StreamGraph* underlying_stream_graph;
	Interval snapshot;
	BitArray nodes_present;
	BitArray links_present;
} ChunkStream;

/*DEFAULT_TO_STRING(NodeId, "%zu");
DEFAULT_COMPARE(NodeId);
DefVector(NodeId, NO_FREE(NodeId));

DEFAULT_TO_STRING(LinkId, "%zu");
DEFAULT_COMPARE(LinkId);
DefVector(LinkId, NO_FREE(LinkId));*/

char* NodeId_to_string(const NodeId* node_id);
bool NodeId_equals(NodeId a, NodeId b);
DefVector(NodeId, NO_FREE(NodeId));

char* LinkId_to_string(const LinkId* link_id);
bool LinkId_equals(LinkId a, LinkId b);
DefVector(LinkId, NO_FREE(LinkId));

ChunkStream ChunkStream_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
							 size_t time_end);
Stream CS_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start, size_t time_end);
void CS_destroy(Stream stream);

void filter_interval(Interval* interval, Interval snapshot);

extern const StreamFunctions ChunkStream_stream_functions;
extern const MetricsFunctions ChunkStream_metrics_functions;

#endif // CHUNK_STREAM_H