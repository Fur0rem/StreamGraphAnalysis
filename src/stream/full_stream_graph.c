#include "full_stream_graph.h"
#include "../induced_graph.h"
#include "../metrics.h"
#include "../stream_graph.h"
#include "../stream_graph/nodes_set.h"
#include "../units.h"
#include "../utils.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

FullStreamGraph FullStreamGraph_from(StreamGraph* stream_graph) {
	FullStreamGraph full_stream_graph = (FullStreamGraph){
		.underlying_stream_graph = stream_graph,
	};
	return full_stream_graph;
}

Stream FSG_from(StreamGraph* stream_graph) {
	FullStreamGraph* full_stream_graph = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = stream_graph;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = full_stream_graph};
	init_cache(&stream);
	return stream;
}

void FSG_destroy(Stream stream) {
	free(stream.stream);
}

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

size_t NodesSet_next(NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream;
	if (nodes_iter_data->current_node >= full_stream_graph->underlying_stream_graph->nodes.nb_nodes) {
		return SIZE_MAX;
	}
	size_t return_val = nodes_iter_data->current_node;
	nodes_iter_data->current_node++;
	return return_val;
}

void NodesSetIterator_destroy(NodesIterator* iterator) {
	free(iterator->iterator_data);
}

NodesIterator FullStreamGraph_nodes_set(FullStreamGraph* full_stream_graph) {
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node = 0;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = full_stream_graph};
	NodesIterator nodes_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (size_t(*)(void*))NodesSet_next,
		.destroy = (void (*)(void*))NodesSetIterator_destroy,
	};
	nodes_iterator.stream_graph.stream = full_stream_graph;
	nodes_iterator.stream_graph.type = FULL_STREAM_GRAPH;
	return nodes_iterator;
}

typedef struct {
	LinkId current_link;
} LinksSetIteratorData;

size_t LinksSet_next(LinksIterator* iter) {
	LinksSetIteratorData* links_iter_data = (LinksSetIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream;
	if (links_iter_data->current_link >= full_stream_graph->underlying_stream_graph->links.nb_links) {
		return SIZE_MAX;
	}
	size_t return_val = links_iter_data->current_link;
	links_iter_data->current_link++;
	return return_val;
}

void LinksSetIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator FullStreamGraph_links_set(FullStreamGraph* full_stream_graph) {
	LinksSetIteratorData* iterator_data = MALLOC(sizeof(LinksSetIteratorData));
	iterator_data->current_link = 0;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = full_stream_graph};
	LinksIterator links_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (size_t(*)(void*))LinksSet_next,
		.destroy = (void (*)(void*))LinksSetIterator_destroy,
	};
	return links_iterator;
}

Interval FullStreamGraph_lifespan(FullStreamGraph* full_stream_graph) {
	return (Interval){
		.start = StreamGraph_lifespan_begin(full_stream_graph->underlying_stream_graph),
		.end = StreamGraph_lifespan_end(full_stream_graph->underlying_stream_graph),
	};
}

size_t FullStreamGraph_scaling(FullStreamGraph* full_stream_graph) {
	return full_stream_graph->underlying_stream_graph->scaling;
}

NodesIterator FullStreamGraph_nodes_present_at_t(FullStreamGraph* full_stream_graph, TimeId t) {
	return get_nodes_present_at_t(full_stream_graph->underlying_stream_graph, t);
}

LinksIterator FullStreamGraph_links_present_at_t(FullStreamGraph* full_stream_graph, TimeId t) {
	return get_links_present_at_t(full_stream_graph->underlying_stream_graph, t);
}

typedef struct {
	TimeId current_interval;
	TemporalNode* node;
} TimesNodePresentIteratorData;

Interval FSG_TimesNodePresent_next(TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = (TimesNodePresentIteratorData*)iter->iterator_data;
	TemporalNode* node = times_iter_data->node;
	if (times_iter_data->current_interval >= node->presence.nb_intervals) {
		return (Interval){.start = SIZE_MAX, .end = SIZE_MAX};
	}
	Interval return_val = node->presence.intervals[times_iter_data->current_interval];
	times_iter_data->current_interval++;
	return return_val;
}

void FSG_TimesNodePresentIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator FullStreamGraph_times_node_present(FullStreamGraph* full_stream_graph, NodeId node_id) {
	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	iterator_data->current_interval = 0;
	iterator_data->node = &full_stream_graph->underlying_stream_graph->nodes.nodes[node_id];
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = full_stream_graph};
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (Interval(*)(void*))FSG_TimesNodePresent_next,
		.destroy = (void (*)(void*))FSG_TimesNodePresentIterator_destroy,
	};
	return times_iterator;
}

typedef struct {
	TimeId current_interval;
	LinkId link_id;
} TimesLinkPresentIteratorData;

Interval TimesLinkPresent_next(TimesIterator* iter) {
	TimesLinkPresentIteratorData* times_iter_data = (TimesLinkPresentIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream;
	Link* link = &full_stream_graph->underlying_stream_graph->links.links[times_iter_data->link_id];
	if (times_iter_data->current_interval >= link->presence.nb_intervals) {
		return (Interval){.start = SIZE_MAX, .end = SIZE_MAX};
	}
	Interval return_val = link->presence.intervals[times_iter_data->current_interval];
	times_iter_data->current_interval++;
	return return_val;
}

void TimesLinkPresentIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator FullStreamGraph_times_link_present(FullStreamGraph* full_stream_graph, LinkId link_id) {
	TimesLinkPresentIteratorData* iterator_data = malloc(sizeof(TimesLinkPresentIteratorData));
	iterator_data->current_interval = 0;
	iterator_data->link_id = link_id;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = full_stream_graph};
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (Interval(*)(void*))TimesLinkPresent_next,
		.destroy = (void (*)(void*))TimesLinkPresentIterator_destroy,
	};
	return times_iterator;
}

Link FullStreamGraph_nth_link(FullStreamGraph* full_stream_graph, size_t link_id) {
	return full_stream_graph->underlying_stream_graph->links.links[link_id];
}

typedef struct {
	NodeId node_to_get_neighbours;
	NodeId current_neighbour;
} NeighboursOfNodeIteratorData;

size_t NeighboursOfNode_next(LinksIterator* iter) {
	NeighboursOfNodeIteratorData* neighbours_iter_data = (NeighboursOfNodeIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream;
	NodeId node_id = neighbours_iter_data->node_to_get_neighbours;
	if (neighbours_iter_data->current_neighbour >=
		full_stream_graph->underlying_stream_graph->nodes.nodes[node_id].nb_neighbours) {
		return SIZE_MAX;
	}
	size_t return_val = full_stream_graph->underlying_stream_graph->nodes.nodes[node_id]
							.neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	return return_val;
}

void NeighboursOfNodeIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator FullStreamGraph_neighbours_of_node(FullStreamGraph* full_stream_graph, NodeId node_id) {
	NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(NeighboursOfNodeIteratorData));
	iterator_data->node_to_get_neighbours = node_id;
	iterator_data->current_neighbour = 0;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = full_stream_graph};
	LinksIterator neighbours_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (size_t(*)(void*))NeighboursOfNode_next,
		.destroy = (void (*)(void*))NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

const StreamFunctions FullStreamGraph_stream_functions = {
	.nodes_set = (NodesIterator(*)(void*))FullStreamGraph_nodes_set,
	.links_set = (LinksIterator(*)(void*))FullStreamGraph_links_set,
	.lifespan = (Interval(*)(void*))FullStreamGraph_lifespan,
	.scaling = (size_t(*)(void*))FullStreamGraph_scaling,
	.nodes_present_at_t = (NodesIterator(*)(void*, TimeId))FullStreamGraph_nodes_present_at_t,
	.links_present_at_t = (LinksIterator(*)(void*, TimeId))FullStreamGraph_links_present_at_t,
	.times_node_present = (TimesIterator(*)(void*, NodeId))FullStreamGraph_times_node_present,
	.times_link_present = (TimesIterator(*)(void*, LinkId))FullStreamGraph_times_link_present,
	.nth_link = (Link(*)(void*, size_t))FullStreamGraph_nth_link,
	.neighbours_of_node = (LinksIterator(*)(void*, NodeId))FullStreamGraph_neighbours_of_node,
};

size_t FullStreamGraph_cardinalOfV(FullStreamGraph* full_stream_graph) {
	return full_stream_graph->underlying_stream_graph->nodes.nb_nodes;
}

const MetricsFunctions FullStreamGraph_metrics_functions = {
	.cardinalOfW = NULL,
	.cardinalOfT = NULL,
	.cardinalOfV = (size_t(*)(void*))FullStreamGraph_cardinalOfV,
	.coverage = NULL,
	.node_duration = NULL,
};