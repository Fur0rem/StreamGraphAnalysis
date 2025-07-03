/**
 * @file src/analysis/kcores.h
 */

#ifndef K_CORES_H
#define K_CORES_H

#include "../generic_data_structures/arraylist.h"
#include "../interval.h"
#include "../stream.h"
#include "../utils.h"
#include "cluster.h"
#include "stddef.h"

/**
 * @ingroup EXTERNAL_API
 * @defgroup K_CORES k-cores
 * @brief Compute the k-cores of a stream.
 *
 * The k-core of a stream graph is the largest sub-stream of nodes where each node is connected to at least k other nodes during a maximal
 * time interval.
 * @see Section 13 of the paper for more information.
 *
 * Here's an example on how to use SGA to do so : @ref examples/kcores.c
 * @include examples/kcores.c
 *
 * @{
 */

/**
 * @brief Compute the k-core of a @ref SGA_Stream "stream".
 * @param[in] stream The stream.
 * @param[in] k The k to compute the k-core for.
 * Should be between 1 and the @ref SGA_Stream_distinct_cardinal_of_node_set() "distinct number of nodes" in the stream,
 * both bounds included.
 * @return The k-core of the Stream.
 *
 * @note It won't work for the 0-core, but it is the same as the initial stream, so it's not useful to compute it.
 * It will work for k greaters than the @ref SGA_Stream_distinct_cardinal_of_node_set() "distinct number of nodes" in the stream, but it
 * will be empty, so it's not very useful.
 */
SGA_Cluster SGA_Stream_k_core(const SGA_Stream* stream, size_t k);

#endif // K_CORES_H