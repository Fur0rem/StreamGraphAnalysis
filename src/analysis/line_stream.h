/**
 * @file src/analysis/line_stream.h
 * @brief Header file for transforming a stream into its line stream.
 */

#ifndef SGA_LINE_STREAM_H
#define SGA_LINE_STREAM_H

#include "../stream.h"

/**
 * @ingroup EXTERNAL_API
 * @defgroup LINE_STREAM Line stream
 * @brief Compute the line stream of a stream.
 *
 * The line stream is a transformation where the nodes represent the links of the original stream, and the links represent the times when
 * two links have a node in common.
 *
 * @see Section 12 of the paper for more information.
 * @{
 */

/**
 * @brief Compute the line stream of a given stream. The line stream is a transformation where the nodes represent the links of the original
 * stream, and the links represent the times when two links have a node in common.
 * @param stream The stream to compute the line stream of.
 * @return Its line stream, as a StreamGraph.
 * @note If two links have a node in common but are never present at the same time, the corresponding link in the line stream will not be
 * included. \n
 * The nodes are in the same order as the links of the original stream, links however are only ordered by the first node in the original
 * pair, but the second node is not guaranteed to be in any order. \n
 * Although all line streams representation of the same original stream are isomorphic to each other so it doesn't really matter. \n
 * You can still query a particular link between two nodes using SGA_Stream_link_between_nodes on the original stream and check which node
 * does a link correspond to by looking which node is in common between the two original pairs.
 */
SGA_StreamGraph SGA_line_stream_of(const SGA_Stream* stream);

/**
 * @}
 */

#endif // SGA_LINE_STREAM_H