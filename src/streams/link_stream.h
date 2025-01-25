/**
 * @file src/streams/link_stream.h
 * @brief A Stream that represents a StreamGraph as a LinkStream.
 * A LinkStream is a Stream where all nodes are present during the entire lifespan of the Stream.
 * You can check Section 7 of the article for more information.
 */

#ifndef LINK_STREAM_H
#define LINK_STREAM_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

#ifdef SGA_INTERNAL

/**
 * @brief The structure of a LinkStream.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; ///< The StreamGraph from which the LinkStream was extracted.
} LinkStream;

extern const StreamFunctions LinkStream_stream_functions;   ///< Functions to access the data of a LinkStream.
extern const MetricsFunctions LinkStream_metrics_functions; ///< Functions to compute metrics on a LinkStream.

#endif // SGA_INTERNAL

/**
 * @brief Create a LinkStream from a StreamGraph.
 * @param[in] stream_graph The StreamGraph to create the LinkStream from.
 * @return The created LinkStream.
 */
SGA_Stream SGA_LinkStream_from(SGA_StreamGraph* stream_graph);

/**
 * @brief Destroy a LinkStream.
 * @param[in] stream The LinkStream to destroy.
 */
void SGA_LinkStream_destroy(SGA_Stream stream);

#endif // LINK_STREAM_H