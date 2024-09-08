#ifndef METRICS_H
#define METRICS_H

/**
 * @file metrics.h
 * @brief Functions to compute metrics on a Stream.
 *
 * The metrics are defined in the paper. I encourage you to read it to understand the metrics as explaining them here
 * would be redundant. Most of the functions have been named the same way, but some have been renamed to be more
 * coherent with the implementation, if I changed the name, I will give the name of the metric in the paper.
 * <br>
 * The metrics documentation have been split per section of the paper and ordered in the same order as in the paper.
 */

#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "vector.h"
#include <stddef.h>

/**
 * @brief Table of functions to compute metrics on a Stream.
 *
 * Different types of Stream's can define a specialisation of some of these functions to compute metrics in a more
 * efficient way.
 * If set to NULL, the default implementation will be used.
 */
// TODO : rename these
typedef struct {
	size_t (*cardinalOfW)(Stream*);
	size_t (*cardinalOfT)(Stream*);
	size_t (*cardinalOfV)(Stream*);
	double (*coverage)(Stream*);
	double (*node_duration)(Stream*);
	double (*density)(Stream*);
} MetricsFunctions;

/**
 *@name Section 3 : Stream graphs and link streams
 *@{
 */
/**
 * @param[in] stream The Stream.
 */
double Stream_coverage(Stream* stream);
/** @} */

/**
 *@name Section 4 : Size, duration, uniformity and compactness
 *@{
 */

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the contribution of.
 */
double Stream_contribution_of_node(Stream* stream, NodeId node_id);

/**
 * @param[in] stream The Stream.
 */
double Stream_number_of_nodes(Stream* stream);

/**
 * Named contribution of a pair of nodes in the paper.
 * @param[in] stream The Stream.
 * @param[in] link_id The id of the link to get the contribution of.
 */
double Stream_contribution_of_link(Stream* stream, LinkId link_id);

/**
 * @param[in] stream The Stream.
 */
double Stream_number_of_links(Stream* stream);

/**
 * @param[in] stream The Stream.
 * @param[in] time_id The instant to get the contribution at.
 */
double Stream_node_contribution_at_instant(Stream* stream, TimeId time_id);

/**
 * @param[in] stream The Stream.
 * @param[in] time_id The instant to get the contribution at.
 */
double Stream_link_contribution_at_instant(Stream* stream, TimeId time_id);

/**
 * @param[in] stream The Stream.
 */
double Stream_node_duration(Stream* stream);

/**
 * @param[in] stream The Stream.
 */
double Stream_link_duration(Stream* stream);

/**
 * @param[in] stream The Stream.
 */
double Stream_uniformity(Stream* stream);

// TODO : compactness
/** @} */

/**
 *@name Section 5 : Density
 *@{
 */

/**
 * @param[in] stream The Stream.
 */
double Stream_density(Stream* stream);

/**
 * Named density of a pair of nodes in the paper.
 * @param[in] stream The Stream.
 * @param[in] link_id The id of the link to get the density of.
 */
double Stream_density_of_link(Stream* stream, LinkId link_id);

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the density of.
 */
double Stream_density_of_node(Stream* stream, NodeId node_id);

/**
 * @param[in] stream The Stream.
 * @param[in] time_id The instant to get the density at.
 */
double Stream_density_at_instant(Stream* stream, TimeId time_id);
/** @} */

/**
 *@name Section 8 : Neighbourhood and degree
 *@{
 */

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the degree of.
 */
double Stream_degree_of_node(Stream* stream, NodeId node_id);

/**
 * @param[in] stream The Stream.
 */
double Stream_average_node_degree(Stream* stream);
/** @} */

/**
 *@name Section 9 : Clustering coefficient and transitivity ratio
 *@{
 */

/**
 * @param[in] stream The Stream.
 * @param[in] node_id The id of the node to get the clustering coefficient of.
 */
double Stream_clustering_coeff_of_node(Stream* stream, NodeId node_id);

/**
 * @param[in] stream The Stream.
 */
double Stream_node_clustering_coeff(Stream* stream);

double Stream_transitivity_ratio(Stream* stream);

size_t cardinalOfW(Stream* stream);
size_t cardinalOfT(Stream* stream);
size_t cardinalOfV(Stream* stream);
size_t cardinalOfE(Stream* stream);
/** @} */

// TODO: move out of metrics, and do Stream_mathematically_equivalent
bool Stream_equals(const Stream* stream1, const Stream* stream2);

String Stream_to_string(const Stream* stream);

#endif // METRICS_H