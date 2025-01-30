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
    .distinct_cardinal_of_node_set = FullStreamGraph_distinct_cardinal_of_node_set,
    .distinct_cardinal_of_link_set = FullStreamGraph_distinct_cardinal_of_link_set,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};