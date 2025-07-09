/**
 * @file src/stream_functions.h
 * @brief Functions to access the data of a StreamGraph.
 */

#ifndef STREAM_FUNCTIONS_H
#define STREAM_FUNCTIONS_H

#include "iterators.h"
#include "stream.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include "utils.h"

#ifdef SGA_INTERNAL

/**
 * @brief Table of functions to access the data of a StreamGraph.
 */
typedef struct {
	SGA_Interval (*lifespan)(SGA_StreamData*); ///< Lifespan of the Stream.
	size_t (*time_scale)(SGA_StreamData*);	   ///< By how much the time is scaled. Used to normalise many time-related metrics.
						   ///< It is useful if you scaled up the time of your data to have only integers.
						   ///< And you want to compute the metrics on the original time scale.

	SGA_NodesIterator (*nodes_set)(SGA_StreamData*); ///< Set of nodes in the Stream.
	SGA_LinksIterator (*links_set)(SGA_StreamData*); ///< Set of links in the Stream.

	SGA_NodesIterator (*nodes_present_at_t)(SGA_StreamData*, SGA_Time); ///< Nodes present at a given instant.
	SGA_LinksIterator (*links_present_at_t)(SGA_StreamData*, SGA_Time); ///< Links present at a given instant.

	SGA_TimesIterator (*times_node_present)(SGA_StreamData*, SGA_NodeId); ///< Times a node is present.
	SGA_TimesIterator (*times_link_present)(SGA_StreamData*, SGA_LinkId); ///< Times a link is present.

	SGA_Link (*link_by_id)(SGA_StreamData*, SGA_LinkId); ///< Access a link by its id.
	SGA_Node (*node_by_id)(SGA_StreamData*, SGA_NodeId); ///< Access a node by its id.

	SGA_LinksIterator (*neighbours_of_node)(SGA_StreamData*, SGA_NodeId);	   ///< Neighbours of a node.
	SGA_LinkId (*link_between_nodes)(SGA_StreamData*, SGA_NodeId, SGA_NodeId); ///< Link between two nodes.

	SGA_TimesIterator (*key_instants)(SGA_StreamData*); ///< Key instants of the Stream.
} StreamFunctions;

/**
 * @brief Get the functions to access the data of a StreamGraph.
 * @param[out] variable The variable to store the functions in.
 * @param[in] stream_var The Stream to get the functions for.
 */
#	define STREAM_FUNCS(variable, stream_var)                                                                                         \
		({                                                                                                                         \
			switch ((stream_var)->type) {                                                                                      \
				case FULL_STREAM_GRAPH: {                                                                                  \
					(variable) = FullStreamGraph_stream_functions;                                                     \
					break;                                                                                             \
				}                                                                                                          \
				case LINK_STREAM: {                                                                                        \
					(variable) = LinkStream_stream_functions;                                                          \
					break;                                                                                             \
				}                                                                                                          \
				case CHUNK_STREAM: {                                                                                       \
					(variable) = ChunkStream_stream_functions;                                                         \
					break;                                                                                             \
				}                                                                                                          \
				case CHUNK_STREAM_SMALL: {                                                                                 \
					(variable) = ChunkStreamSmall_stream_functions;                                                    \
					break;                                                                                             \
				}                                                                                                          \
				case TIMEFRAME_STREAM: {                                                                                   \
					(variable) = TimeFrameStream_stream_functions;                                                     \
					break;                                                                                             \
				}                                                                                                          \
				case DELTA_STREAM: {                                                                                       \
					(variable) = DeltaStream_stream_functions;                                                         \
					break;                                                                                             \
				}                                                                                                          \
				default: {                                                                                                 \
					UNREACHABLE_CODE;                                                                                  \
				}                                                                                                          \
			}                                                                                                                  \
			(variable);                                                                                                        \
		})

#endif // SGA_INTERNAL

/**
 * @brief Get the lifespan of a Stream.
 * @param[in] stream The Stream to get the lifespan of.
 * @return The lifespan of the Stream.
 */
SGA_Interval SGA_Stream_lifespan(const SGA_Stream* stream);

/**
 * @brief Get the time scale of a Stream.
 * @param[in] stream The Stream to get the time scale of.
 * @return The time scale of the Stream.
 */
size_t SGA_Stream_time_scale(const SGA_Stream* stream);

/**
 * @brief Get the set of nodes in a Stream.
 * @param[in] stream The Stream to get the set of nodes of.
 * @return An iterator over the set of nodes in the Stream.
 */
SGA_NodesIterator SGA_Stream_nodes_set(const SGA_Stream* stream);

/**
 * @brief Get the set of links in a Stream.
 * @param[in] stream The Stream to get the set of links of.
 * @return An iterator over the set of links in the Stream.
 */
SGA_LinksIterator SGA_Stream_links_set(const SGA_Stream* stream);

/**
 * @brief Get the nodes present at a given instant in a Stream.
 * @param[in] stream The Stream to get the nodes from.
 * @param[in] time_id The instant to get the nodes at.
 * @return An iterator over the nodes present at the given instant.
 */
SGA_NodesIterator SGA_Stream_nodes_present_at_t(const SGA_Stream* stream, SGA_Time time);

/**
 * @brief Get the links present at a given instant in a Stream.
 * @param[in] stream The Stream to get the links from.
 * @param[in] time_id The instant to get the links at.
 * @return An iterator over the links present at the given instant.
 */
SGA_LinksIterator SGA_Stream_links_present_at_t(const SGA_Stream* stream, SGA_Time time);

/**
 * @brief Get the times a node is present in a Stream.
 * @param[in] stream The Stream to get the times from.
 * @param[in] node_id The id of the node to get the times of.
 * @return An iterator over the times the node is present in the Stream.
 */
SGA_TimesIterator SGA_Stream_times_node_present(const SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @brief Get the times a link is present in a Stream.
 * @param[in] stream The Stream to get the times from.
 * @param[in] link_id The id of the link to get the times of.
 * @return An iterator over the times the link is present in the Stream.
 */
SGA_TimesIterator SGA_Stream_times_link_present(const SGA_Stream* stream, SGA_LinkId link_id);

/**
 * @brief Get a link by its id in a Stream.
 * @param[in] stream The Stream to get the link from.
 * @param[in] link_id The id of the link to get.
 * @return The link with the given id.
 */
SGA_Link SGA_Stream_link_by_id(const SGA_Stream* stream, SGA_LinkId link_id);

/**
 * @brief Get a node by its id in a Stream.
 * @param[in] stream The Stream to get the node from.
 * @param[in] node_id The id of the node to get.
 * @return The node with the given id.
 */
SGA_Node SGA_Stream_node_by_id(const SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @brief Get the neighbours of a node in a Stream.
 * @param[in] stream The Stream to get the neighbours from.
 * @param[in] node_id The id of the node to get the neighbours of.
 * @return An iterator over the neighbours of the node.
 */
SGA_LinksIterator SGA_Stream_neighbours_of_node(const SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @brief Get the id of the link between two nodes in a Stream.
 * @param[in] stream The Stream to get the link from.
 * @param[in] node_id The id of the first node.
 * @param[in] other_node_id The id of the second node.
 * @return The id of the link between the two nodes.
 */
SGA_LinkId SGA_Stream_link_between_nodes(const SGA_Stream* stream, SGA_NodeId node_id, SGA_NodeId other_node_id);

/**
 * @brief Get the key instants of a Stream. A key instant is a time instant where the graph changes.
 * For each time interval [t_start, t_end[ received from this iterator, all induced graphs {G(t_start), ..., G(t_end-1)} are the same.
 * @param[in] stream The Stream to get the key instants of.
 * @return An iterator over the key instants of the Stream.
 */
SGA_TimesIterator SGA_Stream_key_instants(const SGA_Stream* stream);

/**
 * @brief Destroy a Stream and free its resources. Doesn't destroy the underlying StreamGraph, just the Stream structure.
 * @param stream The Stream to destroy.
 */
void SGA_Stream_destroy(SGA_Stream stream);

#endif // STREAM_FUNCTIONS_H