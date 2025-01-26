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

	SGA_TimesIterator (*key_moments)(SGA_StreamData*); ///< Key moments of the Stream.
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

SGA_Interval SGA_Stream_lifespan(SGA_Stream* stream);
SGA_NodesIterator SGA_Stream_nodes_set(SGA_Stream* stream);
SGA_LinksIterator SGA_Stream_links_set(SGA_Stream* stream);
SGA_NodesIterator SGA_Stream_nodes_present_at_t(SGA_Stream* stream, SGA_TimeId time_id);
SGA_LinksIterator SGA_Stream_links_present_at_t(SGA_Stream* stream, SGA_TimeId time_id);
SGA_TimesIterator SGA_Stream_times_node_present(SGA_Stream* stream, SGA_NodeId node_id);
SGA_TimesIterator SGA_Stream_times_link_present(SGA_Stream* stream, SGA_LinkId link_id);
SGA_Link SGA_Stream_link_by_id(SGA_Stream* stream, SGA_LinkId link_id);
SGA_Node SGA_Stream_node_by_id(SGA_Stream* stream, SGA_NodeId node_id);
SGA_LinksIterator SGA_Stream_neighbours_of_node(SGA_Stream* stream, SGA_NodeId node_id);
SGA_LinkId SGA_Stream_link_between_nodes(SGA_Stream* stream, SGA_NodeId node_id, SGA_NodeId other_node_id);
SGA_TimesIterator SGA_Stream_key_moments(SGA_Stream* stream);

#endif // STREAM_FUNCTIONS_H