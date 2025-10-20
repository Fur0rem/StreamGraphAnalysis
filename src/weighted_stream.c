/**
 * @file src/weighted_stream.c
 */

#define SGA_INTERNAL

#include "weighted_stream.h"
#include "interval.h"
#include "parsing/parse_stream_graph.h"
#include "stream.h"
#include "weighted/weight_function.h"

SGA_W_StreamGraph SGA_W_StreamGraph_from_parsed(SGA_ParsedStreamGraph parsed) {
	ASSERT(parsed.is_weighted);
	SGA_W_StreamGraph wsg = {
	    .base = SGA_StreamGraph_from_parsed(&parsed),
	    .node_weights =
		SGA_WeightFunc_from_parsed(parsed.node_weights, parsed.node_presences.length, parsed.general_header.lifespan, true, NULL),
	    .link_weights = SGA_WeightFunc_from_parsed(
		parsed.link_weights, parsed.link_presences.length, parsed.general_header.lifespan, false, &parsed.link_id_map),
	};

	return wsg;
}

SGA_W_StreamGraph SGA_StreamGraph_equip_weight(SGA_StreamGraph stream_graph, SGA_WeightFunc node_weights, SGA_WeightFunc link_weights) {
	SGA_W_StreamGraph w_stream_graph = {
	    .base	  = stream_graph,
	    .node_weights = node_weights,
	    .link_weights = link_weights,
	};

	return w_stream_graph;
}

SGA_Weight SGA_W_StreamGraph_node_weight_at_t(const SGA_W_StreamGraph* stream_graph, SGA_NodeId node, SGA_Time time) {
	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, time));
	return SGA_WeightFunc_weight_at_t(&stream_graph->node_weights, node, time);
}
SGA_Weight SGA_W_StreamGraph_link_weight_at_t(const SGA_W_StreamGraph* stream_graph, SGA_LinkId link, SGA_Time time) {
	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, time));

	return SGA_WeightFunc_weight_at_t(&stream_graph->link_weights, link, time);
}

SGA_Weight SGA_W_StreamGraph_weight_integral_of_node_between(const SGA_W_StreamGraph* stream_graph, SGA_NodeId node,
							     SGA_Interval interval) {
	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains_interval(stream_graph->base.lifespan, interval));

	return SGA_WeightFunc_weight_integral_between(&stream_graph->node_weights, node, interval);
}
SGA_Weight SGA_W_StreamGraph_weight_integral_of_link_between(const SGA_W_StreamGraph* stream_graph, SGA_LinkId link,
							     SGA_Interval interval) {
	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains_interval(stream_graph->base.lifespan, interval));

	return SGA_WeightFunc_weight_integral_between(&stream_graph->link_weights, link, interval);
}

SGA_Weight SGA_W_StreamGraph_max_node_weight(const SGA_W_StreamGraph* stream_graph) {
	return SGA_WeightFunc_max(&stream_graph->node_weights);
}

SGA_Weight SGA_W_StreamGraph_min_node_weight(const SGA_W_StreamGraph* stream_graph) {
	return SGA_WeightFunc_min(&stream_graph->node_weights);
}

SGA_Weight SGA_W_StreamGraph_min_link_weight(const SGA_W_StreamGraph* stream_graph) {
	return SGA_WeightFunc_min(&stream_graph->link_weights);
}

SGA_Weight SGA_W_StreamGraph_max_link_weight(const SGA_W_StreamGraph* stream_graph) {
	return SGA_WeightFunc_max(&stream_graph->link_weights);
}

void SGA_W_StreamGraph_normalise_node_weights(SGA_W_StreamGraph* stream_graph) {
	SGA_Weight min = SGA_W_StreamGraph_min_node_weight(stream_graph);
	SGA_Weight max = SGA_W_StreamGraph_max_node_weight(stream_graph);
	SGA_WeightFunc_normalise(&stream_graph->node_weights, min, max);
}

void SGA_W_StreamGraph_normalise_link_weights(SGA_W_StreamGraph* stream_graph) {
	SGA_Weight min = SGA_W_StreamGraph_min_link_weight(stream_graph);
	SGA_Weight max = SGA_W_StreamGraph_max_link_weight(stream_graph);
	SGA_WeightFunc_normalise(&stream_graph->link_weights, min, max);
}

SGA_W_StreamGraph SGA_W_StreamGraph_from_file(const char* filename) {
	SGA_ParsedStreamGraph parsed = SGA_parse_stream_graph(filename);
	SGA_W_StreamGraph sg	     = SGA_W_StreamGraph_from_parsed(parsed);
	SGA_ParsedStreamGraph_destroy(parsed);
	return sg;
}

void SGA_W_StreamGraph_destroy(SGA_W_StreamGraph self) {
	SGA_StreamGraph_destroy(self.base);
	SGA_WeightFunc_destroy(self.node_weights);
	SGA_WeightFunc_destroy(self.link_weights);
}