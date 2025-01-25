/**
 * @file src/streams/full_stream_graph.h
 * @brief A wrapper around a StreamGraph to use it as a Stream. It does not do any transformation on the StreamGraph.
 */

#ifndef FULL_STREAM_GRAPH_H
#define FULL_STREAM_GRAPH_H

#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

#ifdef SGA_INTERNAL

/**
 * @brief The FullStreamGraph structure.
 */
typedef struct {
	SGA_StreamGraph* underlying_stream_graph; /**< A reference to the underlying StreamGraph. */
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

#endif // FULL_STREAM_GRAPH_H