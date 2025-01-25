/**
 * @file src/streams/snapshot_stream.h
 * @brief A Stream that represents a snapshot of a StreamGraph.
 * A SnapshotStream is wrapper around a StreamGraph to only consider a specific time interval.
 */

#ifndef SGA_SNAPSHOT_STREAM_H
#define SGA_SNAPSHOT_STREAM_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/links_set.h"
#include "../stream_graph/nodes_set.h"
#include <stddef.h>

#ifdef SGA_INTERNAL

/**
 * @brief The structure of a SnapshotStream.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; ///< The StreamGraph from which the snapshot was extracted.
	Interval snapshot;			  ///< The time interval of the snapshot.
} SnapshotStream;

extern const StreamFunctions SnapshotStream_stream_functions;	///< Functions to access the data of a SnapshotStream.
extern const MetricsFunctions SnapshotStream_metrics_functions; ///< Functions to compute metrics on a SnapshotStream.

#endif // SGA_INTERNAL

/**
 * @brief Create a SnapshotStream from a StreamGraph.
 * @param[in] stream_graph The StreamGraph to create the SnapshotStream from.
 * @param[in] snapshot The time interval of the snapshot.
 * @return The created SnapshotStream.
 */
SGA_Stream SGA_SnapshotStream_from(SGA_StreamGraph* stream_graph, Interval snapshot);

/**
 * @brief Destroy a SnapshotStream.
 * @param[in] stream The SnapshotStream to destroy.
 */
void SGA_SnapshotStream_destroy(SGA_Stream stream);

#endif // SGA_SNAPSHOT_STREAM_H