/**
 * @file src/streams/chunk_stream.h
 * @brief A Stream that represents a chunk of a StreamGraph.
 * It contains a subset of the nodes and links of the StreamGraph.
 * It also contains a subset of the time present in the StreamGraph.
 * <br>
 * It is useful to work on a subset of the StreamGraph. For example, to work on a specific time interval or on a specific set of nodes.
 * <br>
 * It is optimised to work on a big chunk of the StreamGraph.
 * If you want to work on a small chunk of the StreamGraph, maybe ChunkStreamSmall will be more efficient.
 */

#ifndef CHUNK_STREAM_H
#define CHUNK_STREAM_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/links_set.h"
#include "../stream_graph/nodes_set.h"
#include <stddef.h>

#ifdef SGA_INTERNAL

/**
 * @brief The structure of a ChunkStream.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; ///< The StreamGraph from which the chunk was extracted.
	SGA_Interval snapshot;			  ///< The time interval of the chunk.
	BitArray nodes_present;			  ///< Nodes present in the chunk.
						  ///< It works like an array of booleans optimised as a bit array.
						  ///< If nodes_present[i] is true, then the node i is present in the chunk.
	BitArray links_present;			  ///< Links present in the chunk.
				///< Same implementation as nodes_present but for links.
} ChunkStream;

extern const StreamFunctions ChunkStream_stream_functions;   ///< Functions to access the data of a ChunkStream.
extern const MetricsFunctions ChunkStream_metrics_functions; ///< Functions to compute metrics on a ChunkStream.

#endif // SGA_INTERNAL

/**
 * @brief Create a ChunkStream from a StreamGraph, with a subset of nodes and links.
 * @param[in] stream_graph The StreamGraph to create the ChunkStream from.
 * @param[in] nodes The nodes present in the chunk.
 * @param[in] links The links present in the chunk.
 * @param[in] snapshot The time interval at which the chunk exists.
 * @return The created ChunkStream.
 */
SGA_Stream SGA_ChunkStream_with(SGA_StreamGraph* stream_graph, SGA_NodeIdArrayList* nodes_present, SGA_LinkIdArrayList* links_present,
				SGA_Interval snapshot);

/**
 * @brief Create a ChunkStream from a StreamGraph, without a subset of nodes and links.
 * @param[in] stream_graph The StreamGraph to create the ChunkStream from.
 * @param[in] nodes The nodes to remove from the chunk.
 * @param[in] links The links to remove from the chunk.
 * @param[in] snapshot The time interval at which the chunk exists.
 * @return The created ChunkStream.
 */
SGA_Stream SGA_ChunkStream_without(SGA_StreamGraph* stream_graph, SGA_NodeIdArrayList* nodes_absent, SGA_LinkIdArrayList* links_absent,
				   SGA_Interval snapshot);

/**
 * @brief Destroy a ChunkStream.
 * @param[in] stream The ChunkStream to destroy.
 */
void SGA_ChunkStream_destroy(SGA_Stream stream);

#endif // CHUNK_STREAM_H