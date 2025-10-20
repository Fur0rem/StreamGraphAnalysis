/**
 * @file src/weighted_stream.h
 */

#ifndef WEIGHTED_STREAM_H
#define WEIGHTED_STREAM_H

#include "bit_array.h"
#include "interval.h"
#include "stream.h"
#include "stream_graph/events_table.h"
#include "stream_graph/key_instants_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include "weighted/weight_function.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The internal implementation of a weighted StreamGraph.
 */
typedef struct {
	SGA_StreamGraph base;	     ///< The base StreamGraph, which contains the nodes, links, lifespan, time scale, and events.
	SGA_WeightFunc node_weights; ///< The weight function for the nodes in the StreamGraph. It is a union of all the different types of
				     ///< weight functions.
	SGA_WeightFunc link_weights; ///< The weight function for the links in the StreamGraph. It is a union of all the different types of
				     ///< weight functions.
} SGA_W_StreamGraph;

#include "weighted_stream.h"

SGA_W_StreamGraph SGA_W_StreamGraph_from_file(const char* filename);

/**
 * @brief Equips weights to a stream graph.
 * @param stream_graph The stream graph to equip the weights with. Not useable after the function call.
 * @param node_weights The weight function applied to the nodes.
 * @param link_weights The weight function applied to the links.
 */
SGA_W_StreamGraph SGA_StreamGraph_equip_weight(SGA_StreamGraph stream_graph, SGA_WeightFunc node_weights, SGA_WeightFunc link_weights);

SGA_Weight SGA_W_StreamGraph_node_weight_at_t(const SGA_W_StreamGraph* stream_graph, SGA_NodeId node, SGA_Time time);
SGA_Weight SGA_W_StreamGraph_link_weight_at_t(const SGA_W_StreamGraph* stream_graph, SGA_LinkId link, SGA_Time time);
SGA_Weight SGA_W_StreamGraph_weight_integral_of_node_between(const SGA_W_StreamGraph* stream_graph, SGA_NodeId node, SGA_Interval interval);
SGA_Weight SGA_W_StreamGraph_weight_integral_of_link_between(const SGA_W_StreamGraph* stream_graph, SGA_LinkId link, SGA_Interval interval);
SGA_Weight SGA_W_StreamGraph_max_node_weight(const SGA_W_StreamGraph* stream_graph);
SGA_Weight SGA_W_StreamGraph_min_node_weight(const SGA_W_StreamGraph* stream_graph);
SGA_Weight SGA_W_StreamGraph_min_link_weight(const SGA_W_StreamGraph* stream_graph);
SGA_Weight SGA_W_StreamGraph_max_link_weight(const SGA_W_StreamGraph* stream_graph);
void SGA_W_StreamGraph_normalise_node_weights(SGA_W_StreamGraph* stream_graph);
void SGA_W_StreamGraph_normalise_link_weights(SGA_W_StreamGraph* stream_graph);

/**
 * @brief Blanket struct that serves as a placeholder for the union of all stream types (FullStreamGraph, LinkStream, etc...).
 * Otherwise a circular dependency would be created since these files need weighted_stream.h.
 * It is safe to cast this to the actual type of the Stream (which is known in the W_Stream struct).
 */
typedef void SGA_W_StreamData;

/**
 * @brief The structure of a Stream.
 *
 * Depends on an underlying StreamGraph, so its lifetime/scope should be less than the StreamGraph.
 */
typedef struct SGA_W_Stream {
	SGA_Stream base;	     ///< The base Stream, which contains the StreamGraph, type, and cache.
	SGA_StreamData* stream_data; ///< The data of the Stream. It is a union of all the different types of StreamData.
} SGA_W_Stream;

void SGA_W_StreamGraph_destroy(SGA_W_StreamGraph self);

#endif // WEIGHTED_STREAM_H