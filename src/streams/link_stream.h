/**
 * @file src/streams/link_stream.h
 * @brief A Stream that represents a StreamGraph as a LinkStream.
 * A LinkStream is a Stream where all nodes are present during the entire lifespan of the Stream.
 * You can check Section 7 of the article for more information.
 */

#ifndef LINK_STREAM_H
#define LINK_STREAM_H

#include "../analysis/metrics.h"
#include "../weighted_stream.h"
#include "../weighted_stream_functions.h"

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

//////////////////////////
//// Weighted version ////
//////////////////////////

#ifdef SGA_INTERNAL

/**
 * @brief The weighted version of the LinkStream
 */
typedef struct W_LinkStream {
	SGA_W_StreamGraph* underlying_stream_graph; ///< The weighted StreamGraph from which the LinkStream was extracted.
	SGA_WeightFunc extended_nodes_weights;	    ///< The weight function used for the new node presences,
						    ///< created by turning the stream graph into a link stream.
} W_LinkStream;

/**
 * @brief The necessary functions to use a W_LinkStream as a weighted Stream.
 */
extern const WeightedStreamFunctions LinkStream_weighted_stream_functions;

#endif // SGA_INTERNAL
/**
 * @brief Creates a weighted Stream of a LinkStream from a weighted StreamGraph (i.e. all nodes are present at all times)
 * @param[in] stream_graph The weighted StreamGraph
 * @param[in] extended_nodes_weights The weights used for the extensions of nodes. Since turning a stream graph into a link stream creates
 * node presences, it is the weight function used for what got created, used only when the nodes were not already present there.
 * @return The link stream as a weighted stream
 */
SGA_W_Stream SGA_W_LinkStream_from(SGA_W_StreamGraph* stream_graph, SGA_WeightFunc extended_nodes_weights);

#ifdef SGA_INTERNAL
/**
 * @brief Destroys a weighted Stream of a LinkStream.
 * @param[in] self The LinkStream to destroy.
 */
void W_LinkStream_destroy(SGA_W_Stream self);
#endif // SGA_INTERNAL

#endif // LINK_STREAM_H