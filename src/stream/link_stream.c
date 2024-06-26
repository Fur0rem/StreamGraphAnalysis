#include "link_stream.h"
#include "../interval.h"
#include "../metrics.h"
#include "../stream_graph.h"
#include "../utils.h"
#include "full_stream_graph.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

LinkStream LinkStream_from(StreamGraph* stream_graph) {
	LinkStream link_stream = (LinkStream){
		.underlying_stream_graph = stream_graph,
	};
	return link_stream;
}

/*size_t LS_cardinal_of_V(LinkStream* ls) {
	printf("called LS_cardinal_of_V\n");
	return ls->underlying_stream_graph->nodes.nb_nodes;
}

size_t LS_cardinal_of_T(LinkStream* ls) {
	return Interval_size(Interval_from(StreamGraph_lifespan_begin(ls->underlying_stream_graph),
									   StreamGraph_lifespan_end(ls->underlying_stream_graph)));
}

size_t LS_cardinal_of_W(LinkStream* ls) {
	return LS_cardinal_of_V(ls) * LS_cardinal_of_T(ls);
}

double LS_coverage(LinkStream* ls) {
	return 1.0;
}

size_t LS_scaling(LinkStream* ls) {
	return ls->underlying_stream_graph->scaling;
}

BaseGenericFunctions link_stream_base_functions = {
	.cardinalOfT = (size_t(*)(void*))LS_cardinal_of_T,
	.cardinalOfV = (size_t(*)(void*))LS_cardinal_of_V,
	.cardinalOfW = (size_t(*)(void*))LS_cardinal_of_W,
	.scaling = (size_t(*)(void*))LS_scaling,
};

HijackedGenericFunctions link_stream_hijacked_functions = {
	.coverage = (double (*)(void*))LS_coverage,
};*/

/*NodesIterator (*nodes_set)(void*);
LinksIterator (*links_set)(void*);
Interval (*lifespan)(void*);
size_t (*scaling)(void*);

NodesIterator (*nodes_present_at_t)(void*, TimeId);
LinksIterator (*links_present_at_t)(void*, TimeId);

TimesIterator (*times_node_present)(void*, NodeId);
TimesIterator (*times_link_present)(void*, LinkId);*/

typedef struct {
	NodeId current_node;
	FullStreamGraph* fsg;
} LinkStreamNodesSetIteratorData;

void LinkStream_iter_destroy(NodesIterator* iterator) {
	LinkStreamNodesSetIteratorData* iterator_data = (LinkStreamNodesSetIteratorData*)iterator->iterator_data;
	free(iterator_data->fsg);
	free(iterator->iterator_data);
}

// TRICK : kind of weird hack but it works ig?
NodesIterator LinkStream_nodes_set(LinkStream* link_stream) {
	FullStreamGraph* full_stream_graph = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = link_stream->underlying_stream_graph;
	NodesIterator n = FullStreamGraph_stream_functions.nodes_set(full_stream_graph);
	free(n.iterator_data);
	n.iterator_data = MALLOC(sizeof(LinkStreamNodesSetIteratorData));
	LinkStreamNodesSetIteratorData* iterator_data = (LinkStreamNodesSetIteratorData*)n.iterator_data;
	iterator_data->fsg = full_stream_graph;
	iterator_data->current_node = 0;
	n.stream_graph.type = LINK_STREAM;
	n.destroy = (void (*)(void*))LinkStream_iter_destroy;
	return n;
}

LinksIterator LinkStream_links_set(LinkStream* link_stream) {
	FullStreamGraph full_stream_graph = FullStreamGraph_from(link_stream->underlying_stream_graph);
	return FullStreamGraph_stream_functions.links_set(&full_stream_graph);
}

Interval LinkStream_lifespan(LinkStream* link_stream) {
	return Interval_from(StreamGraph_lifespan_begin(link_stream->underlying_stream_graph),
						 StreamGraph_lifespan_end(link_stream->underlying_stream_graph));
}

size_t LinkStream_scaling(LinkStream* link_stream) {
	return link_stream->underlying_stream_graph->scaling;
}

NodesIterator LinkStream_nodes_present_at_t(LinkStream* link_stream, TimeId instant) {
	FullStreamGraph full_stream_graph = FullStreamGraph_from(link_stream->underlying_stream_graph);
	return FullStreamGraph_stream_functions.nodes_set(&full_stream_graph);
}

LinksIterator LinkStream_links_present_at_t(LinkStream* link_stream, TimeId instant) {
	FullStreamGraph full_stream_graph = FullStreamGraph_from(link_stream->underlying_stream_graph);
	return FullStreamGraph_stream_functions.links_present_at_t(&full_stream_graph, instant);
}

// time of nodes iterator
typedef struct {
	bool has_been_called;
} TimesNodePresentIteratorData;

Interval LinkStream_TimesNodePresent_next(TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = (TimesNodePresentIteratorData*)iter->iterator_data;
	StreamGraph* stream_graph = iter->stream_graph.stream;
	if (times_iter_data->has_been_called) {
		return (Interval){.start = SIZE_MAX};
	}
	times_iter_data->has_been_called = true;
	return Interval_from(StreamGraph_lifespan_begin(stream_graph), StreamGraph_lifespan_end(stream_graph));
}

void LinkStream_TimesNodePresentIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator LinkStream_times_node_present(LinkStream* link_stream, NodeId node_id) {
	TimesNodePresentIteratorData* iterator_data = malloc(sizeof(TimesNodePresentIteratorData));
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream = link_stream->underlying_stream_graph};
	iterator_data->has_been_called = false;
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (Interval(*)(void*))LinkStream_TimesNodePresent_next,
		.destroy = (void (*)(void*))LinkStream_TimesNodePresentIterator_destroy,
	};
	return times_iterator;
}

// time of links iterator
TimesIterator LinkStream_times_link_present(LinkStream* link_stream, LinkId link_id) {
	return FullStreamGraph_stream_functions.times_link_present(link_stream->underlying_stream_graph, link_id);
}

Stream LS_from(StreamGraph* stream_graph) {
	LinkStream* link_stream = MALLOC(sizeof(LinkStream));
	link_stream->underlying_stream_graph = stream_graph;
	Stream stream = {.type = LINK_STREAM, .stream = link_stream};
	init_cache(&stream);
	return stream;
}

void LS_destroy(Stream stream) {
	free(stream.stream);
}

// TRICK
Link LinkStream_nth_link(LinkStream* link_stream, LinkId link_id) {
	FullStreamGraph fsg = FullStreamGraph_from(link_stream->underlying_stream_graph);
	Link link = FullStreamGraph_nth_link(&fsg, link_id);
	return link;
}

const StreamFunctions LinkStream_stream_functions = {
	.nodes_set = (NodesIterator(*)(void*))LinkStream_nodes_set,
	.links_set = (LinksIterator(*)(void*))LinkStream_links_set,
	.lifespan = (Interval(*)(void*))LinkStream_lifespan,
	.scaling = (size_t(*)(void*))LinkStream_scaling,
	.nodes_present_at_t = (NodesIterator(*)(void*, TimeId))LinkStream_nodes_present_at_t,
	.links_present_at_t = (LinksIterator(*)(void*, TimeId))LinkStream_links_present_at_t,
	.times_node_present = (TimesIterator(*)(void*, NodeId))LinkStream_times_node_present,
	.times_link_present = (TimesIterator(*)(void*, LinkId))LinkStream_times_link_present,
	.nth_link = (Link(*)(void*, LinkId))LinkStream_nth_link,
	.neighbours_of_node = NULL,
};

double LS_coverage(LinkStream* link_stream) {
	printf("called LS_coverage\n");
	return 1.0;
}

double density(Stream* st) {
	LinkStream* ls = (LinkStream*)st->stream;
	StreamGraph* sg = ls->underlying_stream_graph;
	size_t n = sg->nodes.nb_nodes;
	double m = Stream_number_of_links(st);
	return m / (double)(n * (n - 1));
}

const MetricsFunctions LinkStream_metrics_functions = {
	.coverage = (double (*)(void*))LS_coverage,
	.cardinalOfT = NULL,
	.cardinalOfV = NULL,
	.cardinalOfW = NULL,
	.node_duration = NULL,
	.density = (double (*)(void*))density,
};