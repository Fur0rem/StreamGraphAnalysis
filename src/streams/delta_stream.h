/**
 * @file src/streams/delta_stream.h
 * @brief A Stream to do delta-analysis on a StreamGraph.
 * <br>
 * A DeltaStream is a Stream where we define a span of delta units of time, and where presence is extended to half of the delta unit before
 * and after the instant.
 */

#ifndef DELTA_STREAM_H
#define DELTA_STREAM_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

#ifdef SGA_INTERNAL

/**
 * @brief A DeltaStream is a Stream that extends the presence of nodes and links to half of the delta unit before and after the instant.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; ///< A reference to the underlying StreamGraph.
	SGA_Time half_delta;			  ///< Half of the delta unit of time for the DeltaStream.
} DeltaStream;

/**
 * @brief The necessary functions to use a DeltaStream as a Stream.
 */
extern const StreamFunctions DeltaStream_stream_functions;

/**
 * @brief Some specialisation of the metrics functions for DeltaStream.
 */
extern const MetricsFunctions DeltaStream_metrics_functions;

#endif // SGA_INTERNAL

/**
 * @brief Creates the delta-equivalent Stream of a StreamGraph
 * @param[in] stream_graph The StreamGraph to create the DeltaStream from.
 * @param[in] half_delta Half of the delta unit of time for the DeltaStream. i.e. how far to extend the presence before and after each
 * instant.
 * @note In the paper, delta streams are defined with a delta parameter, and extend presence by delta/2 before and after each instant. Here,
 * we directly take half_delta as parameter to simplify the implementation.
 * @note The delta is will not be scaled according to the time scale of the StreamGraph. Meaning a delta of 2 in a StreamGraph with a time
 * scale of 10 will not be turned into a delta of 20 when querying information from the DeltaStream.
 * @return The DeltaStream.
 */
SGA_Stream SGA_DeltaStream_from(SGA_StreamGraph* stream_graph, SGA_Time half_delta);

/**
 * @brief Destroys a Stream of a DeltaStream.
 * @param[in] stream The DeltaStream to destroy.
 */
void SGA_DeltaStream_destroy(SGA_Stream stream);

//////////////////////////
//// Weighted version ////
//////////////////////////

#include "../weighted_stream_functions.h"

#ifdef SGA_INTERNAL

/**
 * @brief The weighted version of the DeltaStream
 */
typedef struct W_DeltaStream {
	SGA_W_StreamGraph* underlying_stream_graph; ///< The weighted StreamGraph from which the DeltaStream was extracted.
} W_DeltaStream;

/**
 * @brief The necessary functions to use a W_DeltaStream as a weighted Stream.
 */
extern const WeightedStreamFunctions DeltaStream_weighted_stream_functions;

#endif // SGA_INTERNAL

/**
 * @brief Create a weighted DeltaStream from a StreamGraph. The weight on the new intervals are the ones that are the closest in the
 * underlying StreamGraph.
 * @param[in] stream_graph The weighted StreamGraph to create the DeltaStream from.
 * @param[in] half_delta Half of the delta unit of time for the DeltaStream.
 * @return The created DeltaStream.
 */
SGA_W_Stream SGA_W_DeltaStream_from(SGA_W_StreamGraph* stream_graph, SGA_Time half_delta);

#ifdef SGA_INTERNAL
/**
 * @brief Destroys a weighted Stream of a DeltaStream.
 * @param[in] self The DeltaStream to destroy.
 */
void W_DeltaStream_destroy(SGA_W_Stream self);
#endif // SGA_INTERNAL

#endif // DELTA_STREAM_H