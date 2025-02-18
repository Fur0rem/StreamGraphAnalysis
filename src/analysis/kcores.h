/**
 * @file src/analysis/kcores.h
 * @brief Functions to compute the k-cores of a Stream.
 *
 * The k-core of a stream graph is the largest sub-stream of nodes where each node is connected to at least k other nodes during a maximal
 * time interval.
 * <br>
 * You can check Section 13 of the paper for more information.
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
 * @brief Compute the k-core of a Stream.
 * @param[in] stream The Stream.
 * @param[in] k The k to compute the k-core for. Should be greater between 1 and the number of nodes in the Stream.
 * @return The k-core of the Stream.
 *
 * The algorithm used won't work for the 0-core, but it is the same as the initial graph so you can just use the initial graph.
 * It will work for k greaters than the number of nodes in the Stream, but it will be empty, so it's not very useful.
 */
SGA_Cluster SGA_Stream_k_core(const SGA_Stream* stream, size_t k);

#endif // K_CORES_H