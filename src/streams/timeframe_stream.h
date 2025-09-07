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
#include "../weighted_stream_functions.h"
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

//////////////////////////
//// Weighted version ////
//////////////////////////

#ifdef SGA_INTERNAL

/**
 * @brief The weighted version of the TimeFrameStream
 */
typedef struct W_TimeFrameStream {
	SGA_W_StreamGraph* underlying_stream_graph; ///< The weighted StreamGraph from which the TimeFrameStream was extracted.
	SGA_Interval timeframe;			    ///< The time interval of the timeframe.
} W_TimeFrameStream;

/**
 * @brief The necessary functions to use a W_TimeFrameStream as a weighted Stream.
 */
extern const WeightedStreamFunctions TimeFrameStream_weighted_stream_functions;

#endif // SGA_INTERNAL

/**
 * @brief Creates a weighted Stream of a TimeFrameStream from a weighted StreamGraph.
 * @param[in] stream_graph The weighted StreamGraph
 * @param[in] timeframe The time interval of the timeframe.
 * @return The TimeFrameStream as a weighted stream
 */
SGA_W_Stream SGA_W_TimeFrameStream_from(SGA_W_StreamGraph* stream_graph, SGA_Interval timeframe);

/**
 * @brief Destroys a weighted Stream of a TimeFrameStream.
 * @param[in] self The TimeFrameStream to destroy.
 */
void SGA_W_TimeFrameStream_destroy(SGA_W_Stream self);

#endif // SGA_TIMEFRAME_STREAM_H