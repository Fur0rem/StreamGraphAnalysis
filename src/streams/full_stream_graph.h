/**
 * @file src/streams/full_stream_graph.h
 * @brief A wrapper around a StreamGraph to use it as a Stream. It does not do any transformation on the StreamGraph.
 */

#ifndef FULL_STREAM_GRAPH_H
#define FULL_STREAM_GRAPH_H

#include "../analysis/metrics.h"
#include "../weighted_stream.h"
#include "../weighted_stream_functions.h"

#ifdef SGA_INTERNAL

/**
 * @brief The FullStreamGraph structure.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; ///< A reference to the underlying StreamGraph.
} FullStreamGraph;

/**
 * @brief The necessary functions to use a FullStreamGraph as a Stream.
 */
extern const StreamFunctions FullStreamGraph_stream_functions;

/**
 * @brief Some specialisation of the metrics functions for FullStreamGraph.
 */
extern const MetricsFunctions FullStreamGraph_metrics_functions;

#endif // SGA_INTERNAL

/**
 * @brief Creates a Stream of a FullStreamGraph from a StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @return The FullStreamGraph.
 */
SGA_Stream SGA_FullStreamGraph_from(SGA_StreamGraph* stream_graph);

/**
 * @brief Destroys a Stream of a FullStreamGraph.
 * @param[in] stream The FullStreamGraph to destroy.
 */
void SGA_FullStreamGraph_destroy(SGA_Stream stream);

//////////////////////////
//// Weighted version ////
//////////////////////////

/**
 * @brief The weighted version of the FullStreamGraph structure.
 */
typedef struct W_FullStreamGraph {
	SGA_W_StreamGraph* underlying_stream_graph; ///< A reference to the underlying weighted StreamGraph.
} W_FullStreamGraph;

/**
 * @brief Creates a weighted Stream of a FullStreamGraph from a weighted StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @return The FullStreamGraph as a weighted Stream.
 */
SGA_W_Stream SGA_W_FullStreamGraph_from(SGA_W_StreamGraph* stream_graph);

#ifdef SGA_INTERNAL
/**
 * @brief Destroys a weighted Stream of a FullStreamGraph.
 * @param[in] self The FullStreamGraph to destroy.
 */
void W_FullStreamGraph_destroy(SGA_W_Stream self);

/**
 * @brief The necessary functions to use a W_FullStreamGraph as a weighted Stream.
 */
extern const WeightedStreamFunctions FullStreamGraph_weighted_stream_functions;
#endif // SGA_INTERNAL

#endif // FULL_STREAM_GRAPH_H