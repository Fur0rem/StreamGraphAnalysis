#ifndef FULL_STREAM_GRAPH_H
#define FULL_STREAM_GRAPH_H

/**
 * @file full_stream_graph.h
 * @brief A wrapper around a StreamGraph to use it as a Stream. It does not do any transformation on the StreamGraph.
 */

#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"

/**
 * @brief The FullStreamGraph structure.
 */
typedef struct {
	StreamGraph* underlying_stream_graph; /**< A reference to the underlying StreamGraph. */
} FullStreamGraph;

/**
 * @brief The necessary functions to use a FullStreamGraph as a Stream.
 */
extern const StreamFunctions FullStreamGraph_stream_functions;

/**
 * @brief Some specialisation of the metrics functions for FullStreamGraph.
 */
extern const MetricsFunctions FullStreamGraph_metrics_functions;

/**
 * @brief Creates a Stream of a FullStreamGraph from a StreamGraph.
 * @param[in] stream_graph The StreamGraph.
 * @return The FullStreamGraph.
 */
Stream FullStreamGraph_from(StreamGraph* stream_graph);

// TODO
Stream* FSG_From(StreamGraph* stream_graph);

/**
 * @brief Destroys a Stream of a FullStreamGraph.
 * @param[in] stream The Stream.
 */

void FullStreamGraph_destroy(Stream stream);

/**
 * @brief Returns the link corresponding to the given link id in the FullStreamGraph.
 * @param[in] full_stream_graph The FullStreamGraph.
 * @param[in] link_id The link id.
 * @return The link corresponding to the given link id in the FullStreamGraph.
 */
Link FullStreamGraph_link_by_id(StreamData* stream_data, LinkId link_id);

#endif // FULL_STREAM_GRAPH_H