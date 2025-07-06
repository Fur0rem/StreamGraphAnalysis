/**
 * @file src/analysis/isomorphism.h
 */

#ifndef SGA_ISO_MORPHISM_H
#define SGA_ISO_MORPHISM_H

#include "../stream.h"

/**
 * @ingroup EXTERNAL_API
 * @defgroup ISOMORPHISM Isomorphism
 * @brief Check if two streams are isomorphic

 * Isomorphism in regular graphs means that two graphs are the same, but with different labels on the nodes.
 * That is, the structure of the graph is the same, but the nodes are different.
 * In mathematical terms, it means there exists a bijection between the nodes of the two graphs that preserves the edges.

 * In the context of streamgraphs, we add the time dimension to the definition.
 * Now, two streamgraphs are isomorphic if their structure is the same, and if their evolution over time is the same.
 * That means that the bijection between the nodes also preserves the times at which the nodes are present, and the times at which the links
 are present. Albeit with a possible offset of the time axis between the two streamgraphs.
 *
 * @{
 */

/**
 * @brief Check if two streams are isomorphic, and get the node mapping if they are.
 * @param s1 The first stream
 * @param s2 The second stream
 * @return NULL if they are not isomorphic.
 * If they are isomorphic, an array of Node IDs corresponding where the i-th element is what Node ID in s2 the i-th Node in s1 would be. The
 * array has as many elements as the number of distinct nodes of s1 (s2 has to have the same number of nodes if they are isomorphic).
 *
 * This function can be quite costly in terms of computation, as the algorithm used is in O(n!) worst case where n is the number of nodes in
 the streams.
 * This is why there is also an alternative function are_probably_isomorphic which is way faster, but may return false positives.
 */
SGA_NodeId* SGA_Stream_isomorphing_mapping(const SGA_Stream* s1, const SGA_Stream* s2);

/**
 * @brief Check if two streams are isomorphic.
 * @param s1 The first stream
 * @param s2 The second stream
 * @return true if they are isomorphic, false otherwise.
 *
 * This function is a wrapper around SGA_Stream_isomorphing_mapping, and returns true if the mapping is not NULL.
 * It is a convenience function if you just need to know if they are isomorphic but not interested in the mapping
 */
bool SGA_Stream_are_isomorphic(const SGA_Stream* s1, const SGA_Stream* s2);

/**
 * @brief Check if two streamgraphs are probably isomorphic.
 * @param s1 The first streamgraph
 * @param s2 The second streamgraph
 * @return false means they are 100% not isomorphic, true means they are probably isomorphic.

 * This function only checks for fast-to-compute properties and metrics, and will catch most cases where the two streamgraphs are not
 isomorphic, especially as the streamgraphs get larger.
 * However, it may return false positives in some cases, where the two streamgraphs are not isomorphic, but the function returns true.
 * This function checks for the following properties:
 * - The number of nodes in the two streamgraphs (distinct_cardinal_of_node_set)
 * - The number of links in the two streamgraphs (temporal_cardinal_of_link_set)
 * - The total duration of the two streamgraphs (duration)
 * - The total node duration of the two streamgraphs (temporal_cardinal_of_node_set)
 * - The density of the two streamgraphs
 * - The average node degree of the two streamgraphs
 * - The uniformity of the two streamgraphs
 * All these properties can be computed in <= O(n) time, where n is the number of nodes in the streamgraphs.
 */
bool SGA_Streams_are_probably_isomorphic(SGA_Stream* s1, SGA_Stream* s2);

/** @} */

#endif