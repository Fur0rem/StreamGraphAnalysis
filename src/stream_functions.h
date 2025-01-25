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

#ifdef SGA_INTERNAL

/**
 * @brief Table of functions to access the data of a StreamGraph.
 */
typedef struct {
	Interval (*lifespan)(SGA_StreamData*); ///< Lifespan of the Stream.
	size_t (*time_scale)(SGA_StreamData*); ///< By how much the time is scaled. Used to normalise many time-related metrics.
					       ///< It is useful if you scaled up the time of your data to have only integers.
					       ///< And you want to compute the metrics on the original time scale.

	NodesIterator (*nodes_set)(SGA_StreamData*); ///< Set of nodes in the Stream.
	LinksIterator (*links_set)(SGA_StreamData*); ///< Set of links in the Stream.

	NodesIterator (*nodes_present_at_t)(SGA_StreamData*, TimeId); ///< Nodes present at a given instant.
	LinksIterator (*links_present_at_t)(SGA_StreamData*, TimeId); ///< Links present at a given instant.

	TimesIterator (*times_node_present)(SGA_StreamData*, NodeId); ///< Times a node is present.
	TimesIterator (*times_link_present)(SGA_StreamData*, LinkId); ///< Times a link is present.

	Link (*link_by_id)(SGA_StreamData*, size_t); ///< Access a link by its id.
	Node (*node_by_id)(SGA_StreamData*, size_t); ///< Access a node by its id.

	LinksIterator (*neighbours_of_node)(SGA_StreamData*, NodeId);  ///< Neighbours of a node.
	LinkId (*link_between_nodes)(SGA_StreamData*, NodeId, NodeId); ///< Link between two nodes.

	TimesIterator (*key_moments)(SGA_StreamData*); ///< Key moments of the Stream.
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
				case SNAPSHOT_STREAM: {                                                                                    \
					(variable) = SnapshotStream_stream_functions;                                                      \
					break;                                                                                             \
				}                                                                                                          \
				default: {                                                                                                 \
					UNREACHABLE_CODE;                                                                                  \
				}                                                                                                          \
			}                                                                                                                  \
			(variable);                                                                                                        \
		})

#endif // SGA_INTERNAL

Interval SGA_Stream_lifespan(SGA_Stream* stream);
NodesIterator SGA_Stream_nodes_set(SGA_Stream* stream);
LinksIterator SGA_Stream_links_set(SGA_Stream* stream);
NodesIterator SGA_Stream_nodes_present_at_t(SGA_Stream* stream, TimeId time_id);
LinksIterator SGA_Stream_links_present_at_t(SGA_Stream* stream, TimeId time_id);
TimesIterator SGA_Stream_times_node_present(SGA_Stream* stream, NodeId node_id);
TimesIterator SGA_Stream_times_link_present(SGA_Stream* stream, LinkId link_id);
Link SGA_Stream_link_by_id(SGA_Stream* stream, size_t link_id);
Node SGA_Stream_node_by_id(SGA_Stream* stream, size_t node_id);
LinksIterator SGA_Stream_neighbours_of_node(SGA_Stream* stream, NodeId node_id);
LinkId SGA_Stream_link_between_nodes(SGA_Stream* stream, NodeId node_id, NodeId other_node_id);
TimesIterator SGA_Stream_key_moments(SGA_Stream* stream);

#endif // STREAM_FUNCTIONS_H