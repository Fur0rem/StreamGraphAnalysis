#ifndef CHUNK_STREAM_SMALL_H
#define CHUNK_STREAM_SMALL_H

/**
 * @file chunk_stream_small.h
 * @brief Like ChunkStream's, but with a memory representation more efficient for small chunks of the StreamGraph.
 */

#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

#include <stddef.h>

typedef struct {
	size_t nb_nodes;					  /**< Number of nodes in the chunk. */
	NodeId* nodes_present;				  /**< Array of nodes present in the chunk. */
	size_t nb_links;					  /**< Number of links in the chunk. */
	LinkId* links_present;				  /**< Array of links present in the chunk. */
	StreamGraph* underlying_stream_graph; /**< The StreamGraph from which the chunk was extracted. */
	Interval snapshot;					  /**< The time interval of the chunk. */
} ChunkStreamSmall;

extern const StreamFunctions ChunkStreamSmall_stream_functions;
extern const MetricsFunctions ChunkStreamSmall_metrics_functions;

// TODO : change the signature of this function to not have to take 6 arguments
Stream CSS_from(StreamGraph* stream_graph, NodeId* nodes, LinkId* links, Interval snapshot, size_t nb_nodes,
				size_t nb_links);
void ChunkStreamSmall_destroy(Stream stream);

#endif // CHUNK_STREAM_SMALL_H