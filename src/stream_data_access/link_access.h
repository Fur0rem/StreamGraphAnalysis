/**
 * @file src/stream_data_access/link_access.h
 * @brief Functions to access the links of a StreamGraph.
 */

#ifndef SGA_LINK_ACCESS_H
#define SGA_LINK_ACCESS_H

#include "../generic_data_structures/arraylist.h"
#include "../iterators.h"

#ifdef SGA_INTERNAL

/**
 * @brief Get the whole set of links of a StreamGraph.
 * @param[in] stream_graph The StreamGraph to get the links from.
 * @return An iterator over all the links of the StreamGraph.
 */
SGA_LinksIterator SGA_StreamGraph_links_set(SGA_StreamGraph* stream_graph);

/**
 * @brief Get the links present at a given time in a StreamGraph.
 * @param[in] stream_graph The StreamGraph to get the links from.
 * @param[in] t The time at which to get the links present.
 * @return An iterator over the links present at the given time in the given StreamGraph.
 */
SGA_LinksIterator SGA_StreamGraph_links_present_at(SGA_StreamGraph* stream_graph, SGA_Time t);

/**
 * @brief Get a link by its id.
 * @param[in] stream_graph The StreamGraph to get the link from.
 * @param[in] link_id The id of the link to get.
 * @return The link with the given id.
 */
SGA_Link SGA_StreamGraph_link_by_id(SGA_StreamGraph* stream_graph, SGA_LinkId link_id);

/**
 * @brief Get the times a link is present in a StreamGraph.
 * @param[in] stream_graph The StreamGraph to get the times from.
 * @param[in] link_id The id of the link to get the times for.
 * @return An iterator over the times the link is present.
 */
SGA_TimesIterator SGA_StreamGraph_times_link_present(SGA_StreamGraph* stream_graph, SGA_LinkId link_id);

/**
 * @brief Get the id of the link between two nodes.
 * @param[in] stream_graph The StreamGraph to get the link from.
 * @param[in] node_id The id of the first node.
 * @param[in] other_node_id The id of the second node.
 * @return The id of the link between the two nodes.
 */
SGA_LinkId SGA_StreamGraph_link_between_nodes(SGA_StreamGraph* stream_graph, SGA_NodeId node_id, SGA_NodeId other_node_id);

#endif // SGA_INTERNAL

#endif