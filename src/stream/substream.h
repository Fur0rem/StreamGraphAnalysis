#ifndef SUB_STREAM_H
#define SUB_STREAM_H

#include "../metrics.h"
#include "../stream_functions.h"
#include "../stream_graph.h"
#include "chunk_stream.h"
#include <stddef.h>

typedef struct {
	StreamGraph* underlying_stream_graph;
	size_t nb_chunks;
	ChunkStream* chunks;
} SubStream;

typedef struct {
	size_t id;
	IntervalsSet presence;
} ClusterNode;

typedef struct {
	size_t nb_nodes;
	ClusterNode* nodes;
} Cluster;

#endif // SUB_STREAM_H