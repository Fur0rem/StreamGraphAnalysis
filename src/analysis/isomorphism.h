/**
 * @file src/analysis/isomorphism.h
 * @brief Functions to check if two streamgraphs are isomorphic

 * Iso-morphism in regular graphs means that two graphs are the same, but with different labels on the nodes.
 * That is, the structure of the graph is the same, but the nodes are different.
 * In mathematical terms, it means there exists a bijection between the nodes of the two graphs that preserves the edges.

 * In the context of streamgraphs, we add the time dimension to the definition.
 * Now, two streamgraphs are isomorphic if their structure is the same, and if their evolution over time is the same.
 * That means that the bijection between the nodes also preserves the times at which the nodes are present, and the times at which the links
 are present. Albeit with a possible offset of the time axis between the two streamgraphs.
 */

#ifndef SGA_ISO_MORPHISM_H
#define SGA_ISO_MORPHISM_H

#include "../stream.h"

/**
 * @brief Check if two streamgraphs are isomorphic.
 * @param s1 The first streamgraph
 * @param s2 The second streamgraph
 * @return true if the two streamgraphs are isomorphic, false otherwise

 * This function can be quite costly in terms of computation, as the algorithm used is in O(n!) worst case where n is the number of nodes in
 the streamgraphs.
 * This is why there is also an alternative function are_probably_isomorphic which is way faster, but may return false positives.
 */
// TODO: return mapping instead of boolean
// TODO: prefix guard it with SGA_
bool are_isomorphic(const SGA_Stream* s1, const SGA_Stream* s2);

/**
 * @brief Check if two streamgraphs are probably isomorphic.
 * @param s1 The first streamgraph
 * @param s2 The second streamgraph
 * @return true if the two streamgraphs are probably isomorphic, false otherwise

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
// TODO: prefix guard it with SGA_
bool are_probably_isomorphic(SGA_Stream* s1, SGA_Stream* s2);

#endif