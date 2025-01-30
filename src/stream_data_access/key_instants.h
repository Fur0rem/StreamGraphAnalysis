/**
 * @file src/stream_data_access/key_instants.h
 * @brief Functions to access the key instants of a StreamGraph.
 */

#ifndef SGA_KEY_INSTANTS_H
#define SGA_KEY_INSTANTS_H

#include "../iterators.h"
#include "../stream.h"
#include "../stream_graph/key_instants_table.h"
#include <stddef.h>

/**
 * @brief Get all the key instants of a StreamGraph.
 * @param[in] stream_graph The StreamGraph to get the key instants from.
 * @return An iterator over all the key instants of the StreamGraph.
 */
SGA_TimesIterator SGA_StreamGraph_key_instants(SGA_StreamGraph* stream_graph);

#ifdef SGA_INTERNAL

/**
 * @brief Get all the key instants of a StreamGraph in between two instants
 * @param[in] stream_graph The StreamGraph to get the key instants from.
 * @param[in] interval The interval to get the key instants in.
 * @return An iterator over all the key instants of the StreamGraph in the interval.
 */
SGA_TimesIterator SGA_StreamGraph_key_instants_between(SGA_StreamGraph* stream_graph, SGA_Interval interval);

#endif // SGA_INTERNAL

#endif // SGA_KEY_INSTANTS_H