#include "full_stream_graph.h"
#include "../metrics.h"
#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"
#include "../units.h"
#include "../utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Stream FullStreamGraph_from(StreamGraph* stream_graph) {
	FullStreamGraph* full_stream_graph		   = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = stream_graph;
	Stream stream							   = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	init_cache(&stream);
	return stream;
}

void FullStreamGraph_destroy(Stream stream) {
	free(stream.stream_data);
}

NodesIterator FullStreamGraph_nodes_set(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	StreamGraph* stream_graph		   = full_stream_graph->underlying_stream_graph;
	return StreamGraph_nodes_set(stream_graph);
}

Interval FullStreamGraph_lifespan(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->lifespan;
}

size_t FullStreamGraph_scaling(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->scaling;
}

NodesIterator FullStreamGraph_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(Interval_contains(full_stream_graph->underlying_stream_graph->lifespan, instant));

	return StreamGraph_nodes_present_at(full_stream_graph->underlying_stream_graph, instant);
}

LinksIterator FullStreamGraph_links_present_at_t(StreamData* stream_data, TimeId instant) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(Interval_contains(full_stream_graph->underlying_stream_graph->lifespan, instant));

	return StreamGraph_links_present_at(full_stream_graph->underlying_stream_graph, instant);
}

TimesIterator FullStreamGraph_times_node_present(StreamData* stream_data, NodeId node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	StreamGraph* stream_graph		   = full_stream_graph->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return StreamGraph_times_node_present(stream_graph, node_id);
}

Link FullStreamGraph_link_by_id(StreamData* stream_data, size_t link_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(link_id < full_stream_graph->underlying_stream_graph->links.nb_links);

	return full_stream_graph->underlying_stream_graph->links.links[link_id];
}

LinksIterator FullStreamGraph_neighbours_of_node(StreamData* stream_data, NodeId node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	StreamGraph* stream_graph		   = full_stream_graph->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return StreamGraph_neighbours_of_node(stream_graph, node_id);
}

TemporalNode FullStreamGraph_node_by_id(StreamData* stream_data, size_t node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(node_id < full_stream_graph->underlying_stream_graph->nodes.nb_nodes);

	return full_stream_graph->underlying_stream_graph->nodes.nodes[node_id];
}

LinksIterator FullStreamGraph_links_set(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return StreamGraph_links_set(full_stream_graph->underlying_stream_graph);
}

TimesIterator FullStreamGraph_times_link_present(StreamData* stream_data, LinkId link_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;

	ASSERT(link_id < full_stream_graph->underlying_stream_graph->links.nb_links);

	return StreamGraph_times_link_present(full_stream_graph->underlying_stream_graph, link_id);
}

LinkId FullStreamGraph_link_between_nodes(StreamData* stream_data, NodeId node_id, NodeId other_node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	StreamGraph* stream_graph		   = full_stream_graph->underlying_stream_graph;

	ASSERT(node_id != other_node_id);
	ASSERT(node_id < stream_graph->nodes.nb_nodes);
	ASSERT(other_node_id < stream_graph->nodes.nb_nodes);

	return StreamGraph_link_between_nodes(stream_graph, node_id, other_node_id);
}

const StreamFunctions FullStreamGraph_stream_functions = {
	.nodes_set			= FullStreamGraph_nodes_set,
	.links_set			= FullStreamGraph_links_set,
	.lifespan			= FullStreamGraph_lifespan,
	.scaling			= FullStreamGraph_scaling,
	.nodes_present_at_t = FullStreamGraph_nodes_present_at_t,
	.links_present_at_t = FullStreamGraph_links_present_at_t,
	.times_node_present = FullStreamGraph_times_node_present,
	.times_link_present = FullStreamGraph_times_link_present,
	.link_by_id			= FullStreamGraph_link_by_id,
	.node_by_id			= FullStreamGraph_node_by_id,
	.neighbours_of_node = FullStreamGraph_neighbours_of_node,
	.link_between_nodes = FullStreamGraph_link_between_nodes,
};

size_t FullStreamGraph_cardinalOfV(Stream* stream) {
	StreamData* stream_data			   = stream->stream_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->nodes.nb_nodes;
}

const MetricsFunctions FullStreamGraph_metrics_functions = {
	.cardinalOfW   = NULL,
	.cardinalOfT   = NULL,
	.cardinalOfV   = FullStreamGraph_cardinalOfV,
	.coverage	   = NULL,
	.node_duration = NULL,
};