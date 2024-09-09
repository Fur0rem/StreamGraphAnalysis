#ifndef SGA_SNAPSHOT_STREAM_H
#define SGA_SNAPSHOT_STREAM_H

#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/links_set.h"
#include "../stream_graph/nodes_set.h"
#include <stddef.h>

typedef struct {
	StreamGraph* underlying_stream_graph;
	Interval snapshot;
} SnapshotStream;

Stream SnapshotStream_from(StreamGraph* stream_graph, Interval snapshot);
void SnapshotStream_destroy(Stream stream);

extern const StreamFunctions SnapshotStream_stream_functions;
extern const MetricsFunctions SnapshotStream_metrics_functions;

#endif // SGA_SNAPSHOT_STREAM_H