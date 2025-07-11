/**
 * @file src/analysis/cliques.h
 * @brief Find maximal cliques in a Stream.
 *
 * A clique is a set of nodes where each node is connected to all the other nodes in the set.
 * A maximal clique is a clique that is not included in a larger clique.
 * <br>
 * You can check Section 7 of the paper for more information.
 */

#ifndef CLIQUES_H
#define CLIQUES_H

#include "../generic_data_structures/arraylist.h"
#include "../stream.h"
#include "../stream_graph/links_set.h"
#include "../units.h"
#include "../utils.h"
#include <stdbool.h>

/**
 * @ingroup EXTERNAL_API
 * @defgroup CLIQUES Cliques
 * @brief Compute the maximal cliques of a stream.
 *
 * A clique in a stream is a set of nodes over a time interval where each node is connected to all the other nodes in the set during that
 * time. A maximal clique is a clique that is not included in a larger clique.
 * @see Section 7 of the paper for more information.
 *
 * Here's an example on how to use SGA to do so : @ref examples/cliques.c
 * @include examples/cliques.c
 *
 * @{
 */

/**
 * @brief Structure representing a clique.
 */
typedef struct {
	SGA_Time time_start; ///< When the clique starts.
	SGA_Time time_end;   ///< When the clique ends.
	SGA_NodeId* nodes;   ///< The nodes in the clique.
	size_t nb_nodes;     ///< The number of nodes in the clique.
} SGA_Clique;

DeclareToString(SGA_Clique);
DeclareEquals(SGA_Clique);

DeclareArrayList(SGA_Clique);
DeclareArrayListDeriveEquals(SGA_Clique);
DeclareArrayListDeriveToString(SGA_Clique);
DeclareArrayListDeriveRemove(SGA_Clique);

/**
 * @brief Find the maximal cliques in a Stream.
 * @param[in] stream The Stream to find the maximal cliques in.
 * @return The maximal cliques as an ArrayList.
 */
SGA_CliqueArrayList SGA_Stream_maximal_cliques(SGA_Stream* st);

/** @} */

#endif // CLIQUES_H