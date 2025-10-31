/**
 * @file src/weighted_stream_functions.h
 * @brief Functions to access the data of a weighted stream.
 */

#ifndef WEIGHTED_STREAM_FUNCTIONS_H
#define WEIGHTED_STREAM_FUNCTIONS_H

#include "iterators.h"
#include "stream.h"
#include "stream_functions.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include "utils.h"
#include "weighted_stream.h"

#ifdef SGA_INTERNAL

/**
 * @brief Table of functions to access the data of a weighted StreamGraph.
 */
typedef struct WeightedStreamFunctions {
	////////////////////////
	//// Node functions ////
	////////////////////////

	/**
	 * @brief Get the weight of a node at a given time instant.
	 * @param stream The Stream to get the weight from.
	 * @param node The id of the node to get the weight of.
	 * @param time The time instant to get the weight at.
	 * @return The weight of the node at the given time instant.
	 */
	SGA_Weight (*node_weight_at_t)(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Time time);

	/**
	 * @brief Get the integral of the weight of a node over a time interval.
	 * @param stream The Stream to get the integral from.
	 * @param node The id of the node to get the integral of.
	 * @param interval The time interval to get the integral over.
	 * @return The integral of the weight of the node over the given time interval.
	 */
	SGA_Weight (*weight_integral_of_node_between)(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Interval interval);

	/**
	 * @brief Get the maximal weight of a node in the Stream.
	 * @param stream The Stream to get the maximal weight from.
	 * @return The maximal weight of a node in the Stream.
	 */
	SGA_Weight (*max_node_weight)(const SGA_W_Stream* stream);

	/**
	 * @brief Get the minimal weight of a node in the Stream.
	 * @param stream The Stream to get the minimal weight from.
	 * @return The minimal weight of a node in the Stream.
	 */
	SGA_Weight (*min_node_weight)(const SGA_W_Stream* stream);

	/**
	 * @brief Normalise the weights of the nodes in the Stream to the range [0, 1].
	 * @param stream The Stream to normalise the weights of.
	 */
	void (*normalise_node_weights)(SGA_W_Stream* stream);

	////////////////////////
	//// Link functions ////
	////////////////////////

	/**
	 * @brief Get the weight of a link at a given time instant.
	 * @param stream The Stream to get the weight from.
	 * @param link The id of the link to get the weight of.
	 * @param time The time instant to get the weight at.
	 * @return The weight of the link at the given time instant.
	 */
	SGA_Weight (*link_weight_at_t)(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Time time);

	/**
	 * @brief Get the integral of the weight of a link over a time interval.
	 * @param stream The Stream to get the integral from.
	 * @param link The id of the link to get the integral of.
	 * @param interval The time interval to get the integral over.
	 * @return The integral of the weight of the link over the given time interval.
	 */
	SGA_Weight (*weight_integral_of_link_between)(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Interval interval);

	/**
	 * @brief Get the maximal weight of a link in the Stream.
	 * @param stream The Stream to get the maximal weight from.
	 * @return The maximal weight of a link in the Stream.
	 */
	SGA_Weight (*max_link_weight)(const SGA_W_Stream* stream);

	/**
	 * @brief Get the minimal weight of a link in the Stream.
	 * @param stream The Stream to get the minimal weight from.
	 * @return The minimal weight of a link in the Stream.
	 */
	SGA_Weight (*min_link_weight)(const SGA_W_Stream* stream);

	/**
	 * @brief Normalise the weights of the links in the Stream to the range [0, 1].
	 * @param stream The Stream to normalise the weights of.
	 */
	void (*normalise_link_weights)(SGA_W_Stream* stream);
} WeightedStreamFunctions;

/**
 * @brief Get the table of functions to access the data of a weighted StreamGraph.
 * @param stream The Stream to get the functions from.
 * @return The table of functions to access the data of a weighted StreamGraph.
 */
WeightedStreamFunctions SGA_Weighted_StreamFunctions(const SGA_W_Stream* stream);

#endif // SGA_INTERNAL

/**
 * @brief Casts a weighted stream into a regular Stream to use the regular Stream functions.
 * @param self The weighted stream to cast.
 * @return A pointer to a regular SGA_Stream
 */
SGA_Stream* SGA_W_Stream_as_regular_stream(SGA_W_Stream* self);

void SGA_W_Stream_destroy(SGA_W_Stream stream);

#ifdef SGA_INTERNAL

/**
 * @brief Get the functions to access the data of a weighted StreamGraph.
 * @param[out] variable The variable to store the functions in.
 * @param[in] stream_var The Stream to get the functions for.
 */
#	define WEIGHTED_STREAM_FUNCS(variable, stream_var)                                                                                \
		({                                                                                                                         \
			switch ((stream_var)->base.type) {                                                                                 \
				case FULL_STREAM_GRAPH: {                                                                                  \
					(variable) = FullStreamGraph_weighted_stream_functions;                                            \
					break;                                                                                             \
				}                                                                                                          \
				case LINK_STREAM: {                                                                                        \
					(variable) = LinkStream_weighted_stream_functions;                                                 \
					break;                                                                                             \
				}                                                                                                          \
				case CHUNK_STREAM: {                                                                                       \
					(variable) = ChunkStream_weighted_stream_functions;                                                \
					break;                                                                                             \
				}                                                                                                          \
				case CHUNK_STREAM_SMALL: {                                                                                 \
					(variable) = ChunkStreamSmall_weighted_stream_functions;                                           \
					break;                                                                                             \
				}                                                                                                          \
				case TIMEFRAME_STREAM: {                                                                                   \
					(variable) = TimeFrameStream_weighted_stream_functions;                                            \
					break;                                                                                             \
				}                                                                                                          \
				case DELTA_STREAM: {                                                                                       \
					(variable) = DeltaStream_weighted_stream_functions;                                                \
					break;                                                                                             \
				}                                                                                                          \
				default: {                                                                                                 \
					UNREACHABLE_CODE;                                                                                  \
				}                                                                                                          \
			}                                                                                                                  \
			(variable);                                                                                                        \
		})

#endif // SGA_INTERNAL

#endif // STREAM_FUNCTIONS_H