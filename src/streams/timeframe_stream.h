/**
 * @file src/streams/timeframe_stream.h
 * @brief A Stream that represents a timeframe of a StreamGraph.
 * A TimeFrameStream is wrapper around a StreamGraph to only consider a specific time interval.
 */

#ifndef SGA_TIMEFRAME_STREAM_H
#define SGA_TIMEFRAME_STREAM_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/links_set.h"
#include "../stream_graph/nodes_set.h"
#include <stddef.h>

#ifdef SGA_INTERNAL

/**
 * @brief The structure of a TimeFrameStream.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; ///< The StreamGraph from which the timeframe was extracted.
	SGA_Interval timeframe;			  ///< The time interval of the timeframe.
} TimeFrameStream;

extern const StreamFunctions TimeFrameStream_stream_functions;	 ///< Functions to access the data of a TimeFrameStream.
extern const MetricsFunctions TimeFrameStream_metrics_functions; ///< Functions to compute metrics on a TimeFrameStream.

#endif // SGA_INTERNAL

/**
 * @brief Create a TimeFrameStream from a StreamGraph.
 * @param[in] stream_graph The StreamGraph to create the TimeFrameStream from.
 * @param[in] timeframe The time interval of the timeframe.
 * @return The created TimeFrameStream.
 */
SGA_Stream SGA_TimeFrameStream_from(SGA_StreamGraph* stream_graph, SGA_Interval timeframe);

/**
 * @brief Destroy a TimeFrameStream.
 * @param[in] stream The TimeFrameStream to destroy.
 */
void SGA_TimeFrameStream_destroy(SGA_Stream stream);

#endif // SGA_TIMEFRAME_STREAM_H