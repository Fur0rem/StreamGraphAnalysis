#define SGA_INTERNAL

#include "full_stream_graph.h"
#include "../analysis/metrics.h"
#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/key_instants.h"
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"
#include "../units.h"
#include "../utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

SGA_Stream SGA_FullStreamGraph_from(SGA_StreamGraph* stream_graph) {
	FullStreamGraph* full_stream_graph	   = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = stream_graph;
	SGA_Stream stream			   = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	init_cache(&stream);
	return stream;
}

void SGA_FullStreamGraph_destroy(SGA_Stream stream) {
	free(stream.stream_data);
}

SGA_NodesIterator FullStreamGraph_nodes_set(SGA_StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	SGA_StreamGraph* stream_graph	   = full_stream_graph->underlying_stream_graph;
	return SGA_StreamGraph_nodes_set(stream_graph);
}

SGA_Interval FullStreamGraph_lifespan(SGA_StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->lifespan;
}

size_t FullStreamGraph_time_scale(SGA_StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->time_scale;
}

SGA_NodesIterator FullStreamGraph_nodes_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(SGA_Interval_contains(full_stream_graph->underlying_stream_graph->lifespan, instant));

	return SGA_StreamGraph_nodes_present_at(full_stream_graph->underlying_stream_graph, instant);
}

SGA_LinksIterator FullStreamGraph_links_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(SGA_Interval_contains(full_stream_graph->underlying_stream_graph->lifespan, instant));

	return SGA_StreamGraph_links_present_at(full_stream_graph->underlying_stream_graph, instant);
}

SGA_TimesIterator FullStreamGraph_times_node_present(SGA_StreamData* stream_data, SGA_NodeId node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	SGA_StreamGraph* stream_graph	   = full_stream_graph->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_times_node_present(stream_graph, node_id);
}

SGA_Link FullStreamGraph_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(link_id < full_stream_graph->underlying_stream_graph->links.nb_links);

	return full_stream_graph->underlying_stream_graph->links.links[link_id];
}

SGA_LinksIterator FullStreamGraph_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	SGA_StreamGraph* stream_graph	   = full_stream_graph->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_neighbours_of_node(stream_graph, node_id);
}

SGA_Node FullStreamGraph_node_by_id(SGA_StreamData* stream_data, size_t node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(node_id < full_stream_graph->underlying_stream_graph->nodes.nb_nodes);

	return full_stream_graph->underlying_stream_graph->nodes.nodes[node_id];
}

SGA_LinksIterator FullStreamGraph_links_set(SGA_StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return SGA_StreamGraph_links_set(full_stream_graph->underlying_stream_graph);
}

SGA_TimesIterator FullStreamGraph_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(link_id < full_stream_graph->underlying_stream_graph->links.nb_links);

	return SGA_StreamGraph_times_link_present(full_stream_graph->underlying_stream_graph, link_id);
}

SGA_LinkId FullStreamGraph_link_between_nodes(SGA_StreamData* stream_data, SGA_NodeId node_id, SGA_NodeId other_node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	SGA_StreamGraph* stream_graph	   = full_stream_graph->underlying_stream_graph;

	ASSERT(node_id != other_node_id);
	ASSERT(node_id < stream_graph->nodes.nb_nodes);
	ASSERT(other_node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_link_between_nodes(stream_graph, node_id, other_node_id);
}

SGA_TimesIterator FullStreamGraph_key_instants(SGA_StreamData* stream_data) {
	FullStreamGraph* link_stream  = (FullStreamGraph*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	return SGA_StreamGraph_key_instants(stream_graph);
}

const StreamFunctions FullStreamGraph_stream_functions = {
    .nodes_set		= FullStreamGraph_nodes_set,
    .links_set		= FullStreamGraph_links_set,
    .lifespan		= FullStreamGraph_lifespan,
    .time_scale		= FullStreamGraph_time_scale,
    .nodes_present_at_t = FullStreamGraph_nodes_present_at_t,
    .links_present_at_t = FullStreamGraph_links_present_at_t,
    .times_node_present = FullStreamGraph_times_node_present,
    .times_link_present = FullStreamGraph_times_link_present,
    .link_by_id		= FullStreamGraph_link_by_id,
    .node_by_id		= FullStreamGraph_node_by_id,
    .neighbours_of_node = FullStreamGraph_neighbours_of_node,
    .link_between_nodes = FullStreamGraph_link_between_nodes,
    .key_instants	= FullStreamGraph_key_instants,
};

size_t FullStreamGraph_distinct_cardinal_of_node_set(SGA_Stream* stream) {
	SGA_StreamData* stream_data	   = stream->stream_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->nodes.nb_nodes;
}

size_t FullStreamGraph_distinct_cardinal_of_link_set(SGA_Stream* stream) {
	SGA_StreamData* stream_data	   = stream->stream_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->links.nb_links;
}

const MetricsFunctions FullStreamGraph_metrics_functions = {
    .temporal_cardinal_of_node_set = NULL,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = (size_t (*)(const SGA_Stream*))FullStreamGraph_distinct_cardinal_of_node_set,
    .distinct_cardinal_of_link_set = (size_t (*)(const SGA_Stream*))FullStreamGraph_distinct_cardinal_of_link_set,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};

//////////////////////////
//// Weighted version ////
//////////////////////////

SGA_W_Stream SGA_W_FullStreamGraph_from(SGA_W_StreamGraph* stream_graph) {
	SGA_Stream base = {
	    .type	 = FULL_STREAM_GRAPH,
	    .stream_data = &stream_graph->base,
	};
	init_cache(&base);

	W_FullStreamGraph* full_stream_graph	   = MALLOC(sizeof(W_FullStreamGraph));
	full_stream_graph->underlying_stream_graph = stream_graph;

	SGA_W_Stream stream = {
	    .base	 = base,
	    .stream_data = full_stream_graph,
	};

	return stream;
}

void SGA_W_FullStreamGraph_destroy(SGA_W_Stream self) {
	free(self.stream_data);
}

#include "../weighted_stream_functions.h"

SGA_Weight SGA_W_FullStreamGraph_node_weight_at_t(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Time time) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;
	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, time));

	return SGA_W_StreamGraph_node_weight_at_t(stream_graph, node, time);
}

SGA_Weight SGA_W_FullStreamGraph_link_weight_at_t(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Time time) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;
	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, time));

	return SGA_W_StreamGraph_link_weight_at_t(stream_graph, link, time);
}

SGA_Weight SGA_W_FullStreamGraph_weight_integral_of_node_between(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Interval interval) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;
	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, interval.start));
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, interval.end));

	return SGA_W_StreamGraph_weight_integral_of_node_between(stream_graph, node, interval);
}

SGA_Weight SGA_W_FullStreamGraph_weight_integral_of_link_between(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Interval interval) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;
	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, interval.start));
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, interval.end));

	return SGA_W_StreamGraph_weight_integral_of_link_between(stream_graph, link, interval);
}

SGA_Weight SGA_W_FullStreamGraph_max_node_weight(const SGA_W_Stream* stream) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;

	return SGA_WeightFunc_max(&stream_graph->node_weights);
}

SGA_Weight SGA_W_FullStreamGraph_min_node_weight(const SGA_W_Stream* stream) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;

	return SGA_WeightFunc_min(&stream_graph->node_weights);
}

void SGA_W_FullStreamGraph_normalise_node_weights(SGA_W_Stream* stream) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;

	SGA_WeightFunc_normalise(&stream_graph->node_weights);
}

SGA_Weight SGA_W_FullStreamGraph_max_link_weight(const SGA_W_Stream* stream) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;

	return SGA_WeightFunc_max(&stream_graph->link_weights);
}

SGA_Weight SGA_W_FullStreamGraph_min_link_weight(const SGA_W_Stream* stream) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;

	return SGA_WeightFunc_min(&stream_graph->link_weights);
}

void SGA_W_FullStreamGraph_normalise_link_weights(SGA_W_Stream* stream) {
	W_FullStreamGraph* full_stream_graph = (W_FullStreamGraph*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	     = full_stream_graph->underlying_stream_graph;

	SGA_WeightFunc_normalise(&stream_graph->link_weights);
}

const WeightedStreamFunctions FullStreamGraph_weighted_stream_functions = {
    .base			     = FullStreamGraph_stream_functions,
    .node_weight_at_t		     = SGA_W_FullStreamGraph_node_weight_at_t,
    .weight_integral_of_node_between = SGA_W_FullStreamGraph_weight_integral_of_node_between,
    .link_weight_at_t		     = SGA_W_FullStreamGraph_link_weight_at_t,
    .weight_integral_of_link_between = SGA_W_FullStreamGraph_weight_integral_of_link_between,
    .max_node_weight		     = SGA_W_FullStreamGraph_max_node_weight,
    .min_node_weight		     = SGA_W_FullStreamGraph_min_node_weight,
    .normalise_node_weights	     = SGA_W_FullStreamGraph_normalise_node_weights,
    .max_link_weight		     = SGA_W_FullStreamGraph_max_link_weight,
    .min_link_weight		     = SGA_W_FullStreamGraph_min_link_weight,
    .normalise_link_weights	     = SGA_W_FullStreamGraph_normalise_link_weights,
};