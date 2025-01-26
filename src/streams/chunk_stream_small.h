/**
 * @file chunk_stream_small.h
 * @brief Like ChunkStream's, but with a memory representation more efficient for small chunks of the StreamGraph.
 */

#ifndef CHUNK_STREAM_SMALL_H
#define CHUNK_STREAM_SMALL_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

#include <stddef.h>

#ifdef SGA_INTERNAL

/**
 * @brief A ChunkStreamSmall is a Stream that represents a small chunk of a StreamGraph.
 */
typedef struct {
	size_t nb_nodes;			  ///< Number of nodes in the chunk.
	SGA_NodeId* nodes_present;		  ///< Array of nodes present in the chunk.
	size_t nb_links;			  ///< Number of links in the chunk.
	SGA_LinkId* links_present;		  ///< Array of links present in the chunk.
	SGA_StreamGraph* underlying_stream_graph; ///< The StreamGraph from which the chunk was extracted.
	SGA_Interval snapshot;			  ///< The time interval of the chunk.
} ChunkStreamSmall;

extern const StreamFunctions ChunkStreamSmall_stream_functions;	  ///< Functions to access the data of a ChunkStreamSmall.
extern const MetricsFunctions ChunkStreamSmall_metrics_functions; ///< Functions to compute metrics on a ChunkStreamSmall.

#endif // SGA_INTERNAL

/**
 * @brief Create a ChunkStreamSmall from a StreamGraph.
 * @param[in] stream_graph The StreamGraph to create the ChunkStreamSmall from.
 * @param[in] nodes The nodes present in the chunk.
 * @param[in] links The links present in the chunk.
 * @param[in] snapshot The time interval of the chunk.
 * @return The created ChunkStreamSmall.
 */
SGA_Stream SGA_ChunkStreamSmall_from(SGA_StreamGraph* stream_graph, SGA_NodeIdArrayList nodes, SGA_LinkIdArrayList links,
				     SGA_Interval snapshot);
void SGA_ChunkStreamSmall_destroy(SGA_Stream stream);

#endif // CHUNK_STREAM_SMALL_H