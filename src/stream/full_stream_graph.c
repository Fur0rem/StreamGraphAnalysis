#include "full_stream_graph.h"
#include "../induced_graph.h"
#include "../metrics.h"
#include "../stream_graph/nodes_set.h"
#include "../units.h"
#include "../utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Stream FullStreamGraph_from(StreamGraph* stream_graph) {
	FullStreamGraph* full_stream_graph = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = stream_graph;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	init_cache(&stream);
	return stream;
}

Stream* FSG_From(StreamGraph* stream_graph) {
	FullStreamGraph* full_stream_graph = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = stream_graph;
	Stream* stream = MALLOC(sizeof(Stream));
	stream->type = FULL_STREAM_GRAPH;
	stream->stream_data = full_stream_graph;
	init_cache(stream);
	return stream;
}

void FullStreamGraph_destroy(Stream stream) {
	free(stream.stream_data);
}

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

size_t NodesSet_next(NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream_data;
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

NodesIterator FullStreamGraph_nodes_set(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node = 0;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = full_stream_graph;
	NodesIterator nodes_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = NodesSet_next,
		.destroy = NodesSetIterator_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	LinkId current_link;
} LinksSetIteratorData;

size_t LinksSet_next(LinksIterator* iter) {
	LinksSetIteratorData* links_iter_data = (LinksSetIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream_data;
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

LinksIterator FullStreamGraph_links_set(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	LinksSetIteratorData* iterator_data = MALLOC(sizeof(LinksSetIteratorData));
	iterator_data->current_link = 0;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = full_stream_graph;
	LinksIterator links_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = LinksSet_next,
		.destroy = LinksSetIterator_destroy,
	};
	return links_iterator;
}

Interval FullStreamGraph_lifespan(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return (Interval){
		.start = StreamGraph_lifespan_begin(full_stream_graph->underlying_stream_graph),
		.end = StreamGraph_lifespan_end(full_stream_graph->underlying_stream_graph),
	};
}

size_t FullStreamGraph_scaling(StreamData* stream_data) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->scaling;
}

NodesIterator FullStreamGraph_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return get_nodes_present_at_t(full_stream_graph->underlying_stream_graph, instant);
}

LinksIterator FullStreamGraph_links_present_at_t(StreamData* stream_data, TimeId instant) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return get_links_present_at_t(full_stream_graph->underlying_stream_graph, instant);
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

TimesIterator FullStreamGraph_times_node_present(StreamData* stream_data, NodeId node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	iterator_data->current_interval = 0;
	iterator_data->node = &full_stream_graph->underlying_stream_graph->nodes.nodes[node_id];
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = full_stream_graph;
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = FSG_TimesNodePresent_next,
		.destroy = FSG_TimesNodePresentIterator_destroy,
	};
	return times_iterator;
}

typedef struct {
	TimeId current_interval;
	LinkId link_id;
} TimesLinkPresentIteratorData;

Interval TimesLinkPresent_next(TimesIterator* iter) {
	TimesLinkPresentIteratorData* times_iter_data = (TimesLinkPresentIteratorData*)iter->iterator_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream_data;
	// printf("full_stream_graph : %p\n", full_stream_graph);
	// printf("full_stream_graph->underlying_stream_graph : %p\n", full_stream_graph->underlying_stream_graph);
	// printf("full_stream_graph->underlying_stream_graph->links : %p\n",
	// 	   &full_stream_graph->underlying_stream_graph->links);
	assert((size_t)full_stream_graph->underlying_stream_graph > 0x1000UL);
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

TimesIterator FullStreamGraph_times_link_present(StreamData* stream_data, LinkId link_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	TimesLinkPresentIteratorData* iterator_data = malloc(sizeof(TimesLinkPresentIteratorData));
	iterator_data->current_interval = 0;
	iterator_data->link_id = link_id;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = full_stream_graph;
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = TimesLinkPresent_next,
		.destroy = TimesLinkPresentIterator_destroy,
	};
	return times_iterator;
}

Link FullStreamGraph_link_by_id(StreamData* stream_data, size_t link_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->links.links[link_id];
}

typedef struct {
	NodeId node_to_get_neighbours;
	NodeId current_neighbour;
} NeighboursOfNodeIteratorData;

size_t NeighboursOfNode_next(LinksIterator* iter) {
	NeighboursOfNodeIteratorData* neighbours_iter_data = (NeighboursOfNodeIteratorData*)iter->iterator_data;
	// printf("neighbours_iter_data->node_to_get_neighbours : %zu\n", neighbours_iter_data->node_to_get_neighbours);
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)iter->stream_graph.stream_data;
	// printf("full_stream_graph : %p\n", full_stream_graph);
	NodeId node_id = neighbours_iter_data->node_to_get_neighbours;
	// printf("full_stream_graph->underlying_stream_graph : %p\n", full_stream_graph->underlying_stream_graph);
	// printf("full_stream_graph->underlying_stream_graph->nodes : %p\n",
	//    &full_stream_graph->underlying_stream_graph->nodes);
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

LinksIterator FullStreamGraph_neighbours_of_node(StreamData* stream_data, NodeId node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(NeighboursOfNodeIteratorData));
	iterator_data->node_to_get_neighbours = node_id;
	iterator_data->current_neighbour = 0;
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = full_stream_graph};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = full_stream_graph;
	LinksIterator neighbours_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = NeighboursOfNode_next,
		.destroy = NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

TemporalNode FullStreamGraph_node_by_id(StreamData* stream_data, size_t node_id) {
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->nodes.nodes[node_id];
}

LinkId FullStreamGraph_links_between_nodes(StreamData* stream_data, NodeId node_id, NodeId other_node_id) {
	TemporalNode n1 = FullStreamGraph_node_by_id(stream_data, node_id);
	TemporalNode n2 = FullStreamGraph_node_by_id(stream_data, other_node_id);
	NodeId to_look_for = other_node_id;
	// Optimisation: we iterate over the smallest neighbour list
	if (n1.nb_neighbours > n2.nb_neighbours) {
		TemporalNode tmp = n1;
		n1 = n2;
		n2 = tmp;
		to_look_for = node_id;
	}

	for (size_t i = 0; i < n1.nb_neighbours; i++) {
		Link link = FullStreamGraph_link_by_id(stream_data, n1.neighbours[i]);
		if (link.nodes[0] == to_look_for || link.nodes[1] == to_look_for) {
			return n1.neighbours[i];
		}
	}

	// No link found
	return SIZE_MAX;
}

const StreamFunctions FullStreamGraph_stream_functions = {
	.nodes_set = FullStreamGraph_nodes_set,
	.links_set = FullStreamGraph_links_set,
	.lifespan = FullStreamGraph_lifespan,
	.scaling = FullStreamGraph_scaling,
	.nodes_present_at_t = FullStreamGraph_nodes_present_at_t,
	.links_present_at_t = FullStreamGraph_links_present_at_t,
	.times_node_present = FullStreamGraph_times_node_present,
	.times_link_present = FullStreamGraph_times_link_present,
	.link_by_id = FullStreamGraph_link_by_id,
	.node_by_id = FullStreamGraph_node_by_id,
	.neighbours_of_node = FullStreamGraph_neighbours_of_node,
	.links_between_nodes = FullStreamGraph_links_between_nodes,
};

size_t FullStreamGraph_cardinalOfV(Stream* stream) {
	StreamData* stream_data = stream->stream_data;
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)stream_data;
	return full_stream_graph->underlying_stream_graph->nodes.nb_nodes;
}

const MetricsFunctions FullStreamGraph_metrics_functions = {
	.cardinalOfW = NULL,
	.cardinalOfT = NULL,
	.cardinalOfV = FullStreamGraph_cardinalOfV,
	.coverage = NULL,
	.node_duration = NULL,
};