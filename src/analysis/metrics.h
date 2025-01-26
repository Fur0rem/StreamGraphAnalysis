#ifndef METRICS_H
#define METRICS_H

/**
 * @file src/analysis/metrics.h
 * @brief Functions to compute metrics on a Stream.
 *
 * The metrics are defined in the paper. I encourage you to read it to understand the metrics as explaining them here
 * would be redundant. Most of the functions have been named the same way, but some have been renamed to be more
 * coherent with the implementation, if I changed the name, I will give the name of the metric in the paper.
 * <br>
 * The metrics documentation have been split per section of the paper and ordered in the same order as in the paper.
 */

#include "../generic_data_structures/arraylist.h"
#include "../interval.h"
#include "../iterators.h"
#include "../stream.h"
#include <stddef.h>

#ifdef SGA_INTERNAL
/**
 * @brief Table of functions to compute metrics on a Stream.
 *
 * Different types of Stream's can define a specialisation of some of these functions to compute metrics in a more
 * efficient way.
 * If set to NULL, the default implementation will be used.
 */
typedef struct {
	size_t (*temporal_cardinal_of_node_set)(SGA_Stream*); // |W|
	size_t (*temporal_cardinal_of_link_set)(SGA_Stream*); // |E|
	size_t (*distinct_cardinal_of_node_set)(SGA_Stream*); // |V|
	size_t (*distinct_cardinal_of_link_set)(SGA_Stream*); // Not defined in the paper
	size_t (*duration)(SGA_Stream*);		      // |T|
	double (*coverage)(SGA_Stream*);
	double (*node_duration)(SGA_Stream*);
	double (*density)(SGA_Stream*);
} MetricsFunctions;

#endif // SGA_INTERNAL

/**
 *@name Section 3 : Stream graphs and link streams
 *@{
 */

/**
 * @brief Compute the temporal cardinal of node set of a Stream. It is noted |W| in the paper.
 * It is defined as the sum of the duration of the presence of all nodes.
 * For example, if the stream contains 2 nodes A present at [0, 2[ and B present at [1, 4[, the temporal cardinal
 * of the node set is (2 - 0) + (4 - 1) = 2 + 3 = 5.
 * @param[in] stream The Stream.
 * @return The temporal cardinal of the node set.
 */
size_t SGA_Stream_temporal_cardinal_of_node_set(SGA_Stream* stream);

/**
 * @brief Compute the temporal cardinal of link set of a Stream. It is noted |E| in the paper.
 * It is defined as the sum of the duration of the presence of all links.
 * @param[in] stream The Stream.
 * @return The temporal cardinal of the link set.
 */
size_t SGA_Stream_temporal_cardinal_of_link_set(SGA_Stream* stream);

/**
 * @brief Compute the duration of the Stream. It is noted |T| in the paper.
 * It is defined as the length of its lifespan.
 * @param[in] stream The Stream.
 * @return The duration of the Stream.
 *
 * @note In the paper, T is defined as a set of time instants, meaning it could be split in multiple intervals.
 * Here, we consider it as a single interval.
 */
size_t SGA_Stream_duration(SGA_Stream* stream);

/**
 * @brief Compute the distinct cardinal of node set of a Stream. It is noted |V| in the paper.
 * It is defined as the number of distinct nodes present in the Stream.
 * For example, if the stream contains 2 nodes A and B, the distinct cardinal is 2.
 * @param[in] stream The Stream.
 * @return The distinct cardinal of the node set.
 */
size_t SGA_Stream_distinct_cardinal_of_node_set(SGA_Stream* stream);

/**
 * @brief Compute the distinct cardinal of link set of a Stream. It isn't defined in the paper.
 * It is defined as the number of distinct links present in the Stream.
 * @param[in] stream The Stream.
 * @return The distinct cardinal of the link set.
 */
size_t SGA_Stream_distinct_cardinal_of_link_set(SGA_Stream* stream);

/**
 * @brief Compute the coverage of the Stream. It is noted $cov$ in the paper.
 * @param[in] stream The Stream.
 * @return The coverage of the Stream.
 */
double SGA_Stream_coverage(SGA_Stream* stream);
/** @} */

/**
 *@name Section 4 : Size, duration, uniformity and compactness
 *@{
 */

/**
 * @brief Compute the contribution of a node in the Stream. It is noted $n_v$ in the paper.
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the contribution of.
 * @return The contribution of the node.
 */
double SGA_Stream_contribution_of_node(SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @brief Compute the number of nodes in the Stream. It is noted $n$ in the paper.
 * @param[in] stream The Stream.
 * @return The number of nodes in the Stream.
 */
double SGA_Stream_number_of_nodes(SGA_Stream* stream);

/**
 * @brief Compute the contribution of a link in the Stream. It is noted $m_{uv}$ in the paper.
 * @param[in] stream The Stream.
 * @param[in] link_id The id of the link to get the contribution of.
 * @return The contribution of the link.
 */
double SGA_Stream_contribution_of_link(SGA_Stream* stream, SGA_LinkId link_id);

/**
 * @brief Compute the number of links in the Stream. It is noted $m$ in the paper.
 * @param[in] stream The Stream.
 * @return The number of links in the Stream.
 */
double SGA_Stream_number_of_links(SGA_Stream* stream);

/**
 * @brief Compute the node contribution at a given instant in the Stream. It is noted $k_t$ in the paper.
 * @param[in] stream The Stream.
 * @param[in] time_id The instant to get the contribution at.
 * @return The node contribution at the instant.
 */
double SGA_Stream_node_contribution_at_instant(SGA_Stream* stream, SGA_Time time);

/**
 * @brief Compute the link contribution at a given instant in the Stream. It is noted $l_t$ in the paper.
 * @param[in] stream The Stream.
 * @param[in] time_id The instant to get the contribution at.
 * @return The link contribution at the instant.
 */
double SGA_Stream_link_contribution_at_instant(SGA_Stream* stream, SGA_Time time);

/**
 * @brief Compute the node duration of the Stream. It is noted $k$ in the paper.
 * @param[in] stream The Stream.
 * @return The node duration of the Stream.
 */
double SGA_Stream_node_duration(SGA_Stream* stream);

/**
 * @brief Compute the link duration of the Stream. It is noted $l$ in the paper.
 * @param[in] stream The Stream.
 * @return The link duration of the Stream.
 */
double SGA_Stream_link_duration(SGA_Stream* stream);

/**
 * @brief Compute the uniformity of the Stream. It is noted $⋓$ in the paper.
 * @param[in] stream The Stream.
 * @return The uniformity of the Stream.
 */
double SGA_Stream_uniformity(SGA_Stream* stream);

/**
 * @brief Compute the compactness of the Stream. It is noted $c$ in the paper.
 * @param[in] stream The Stream.
 */
double SGA_Stream_compactness(SGA_Stream* stream);
/** @} */

/**
 *@name Section 5 : Density
 *@{
 */

/**
 * @brief Compute the density of the Stream. It is noted $δ$ in the paper.
 * @param[in] stream The Stream.
 */
double SGA_Stream_density(SGA_Stream* stream);

/**
 * Named density of a pair of nodes in the paper.
 * @param[in] stream The Stream.
 * @param[in] link_id The id of the link to get the density of.
 */
double SGA_Stream_density_of_link(SGA_Stream* stream, SGA_LinkId link_id);

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the density of.
 */
double SGA_Stream_density_of_node(SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @param[in] stream The Stream.
 * @param[in] time_id The instant to get the density at.
 */
double SGA_Stream_density_at_instant(SGA_Stream* stream, SGA_Time time);
/** @} */

/**
 *@name Section 8 : Neighbourhood and degree
 *@{
 */

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the degree of.
 */
double SGA_Stream_degree_of_node(SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @param[in] stream The Stream.
 */
double SGA_Stream_average_node_degree(SGA_Stream* stream);
/** @} */

/**
 *@name Section 9 : Clustering coefficient and transitivity ratio
 *@{
 */

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the clustering coefficient of.
 */
double SGA_Stream_clustering_coeff_of_node(SGA_Stream* stream, SGA_NodeId node_id);

/**
 * @param[in] stream The Stream.
 */
double SGA_Stream_node_clustering_coeff(SGA_Stream* stream);

double SGA_Stream_transitivity_ratio(SGA_Stream* stream);

/** @} */

String SGA_Stream_to_string(const SGA_Stream* stream);

#endif // METRICS_H