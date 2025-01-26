/**
 * @file src/stream_data_access/key_moments.h
 * @brief Functions to access the key moments of a StreamGraph.
 */

#ifndef SGA_KEY_MOMENTS_H
#define SGA_KEY_MOMENTS_H

#include "../iterators.h"
#include "../stream.h"
#include "../stream_graph/key_moments_table.h"
#include <stddef.h>

/**
 * @brief Get all the key moments of a StreamGraph.
 * @param[in] stream_graph The StreamGraph to get the key moments from.
 * @return An iterator over all the key moments of the StreamGraph.
 */
SGA_TimesIterator SGA_StreamGraph_key_moments(SGA_StreamGraph* stream_graph);

#ifdef SGA_INTERNAL

/**
 * @brief Get all the key moments of a StreamGraph in between two instants
 * @param[in] stream_graph The StreamGraph to get the key moments from.
 * @param[in] interval The interval to get the key moments in.
 * @return An iterator over all the key moments of the StreamGraph in the interval.
 */
SGA_TimesIterator SGA_StreamGraph_key_moments_between(SGA_StreamGraph* stream_graph, SGA_Interval interval);

#endif // SGA_INTERNAL

#endif // SGA_KEY_MOMENTS_H