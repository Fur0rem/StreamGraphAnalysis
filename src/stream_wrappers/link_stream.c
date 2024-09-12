#include "link_stream.h"
#include "../interval.h"
#include "../metrics.h"
#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Stream LinkStream_from(StreamGraph* stream_graph) {
	LinkStream* link_stream				 = MALLOC(sizeof(LinkStream));
	link_stream->underlying_stream_graph = stream_graph;
	Stream stream						 = {.type = LINK_STREAM, .stream_data = link_stream};
	init_cache(&stream);
	return stream;
}

void LinkStream_destroy(Stream stream) {
	free(stream.stream_data);
}

NodesIterator LinkStream_nodes_set(StreamData* stream_data) {
	LinkStream* link_stream	  = (LinkStream*)stream_data;
	StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return StreamGraph_nodes_set(stream_graph);
}

LinksIterator LinkStream_links_set(StreamData* stream_data) {
	LinkStream* link_stream	  = (LinkStream*)stream_data;
	StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return StreamGraph_links_set(stream_graph);
}

Interval LinkStream_lifespan(StreamData* stream_data) {
	LinkStream* link_stream	  = (LinkStream*)stream_data;
	StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return StreamGraph_lifespan(stream_graph);
}

size_t LinkStream_scaling(StreamData* stream_data) {
	LinkStream* link_stream	  = (LinkStream*)stream_data;
	StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return StreamGraph_scaling(stream_graph);
}

NodesIterator LinkStream_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(Interval_contains(StreamGraph_lifespan(link_stream->underlying_stream_graph), instant));

	// In a linkstream, all nodes are present at all times
	return StreamGraph_nodes_set(link_stream->underlying_stream_graph);
}

LinksIterator LinkStream_links_present_at_t(StreamData* stream_data, TimeId instant) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(Interval_contains(StreamGraph_lifespan(link_stream->underlying_stream_graph), instant));

	return StreamGraph_links_present_at(link_stream->underlying_stream_graph, instant);
}

typedef struct {
	bool has_been_called;
} TimesNodePresentIteratorData;

Interval LinkStream_TimesNodePresent_next(TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = (TimesNodePresentIteratorData*)iter->iterator_data;
	LinkStream* ls								  = iter->stream_graph.stream_data;
	if (times_iter_data->has_been_called) {
		return TIMES_ITERATOR_END;
	}
	times_iter_data->has_been_called = true;
	return StreamGraph_lifespan(ls->underlying_stream_graph);
}

void LinkStream_TimesNodePresentIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator LinkStream_times_node_present(StreamData* link_stream, NodeId node_id) {
	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	Stream stream								= {.stream_data = link_stream};
	iterator_data->has_been_called				= false;
	TimesIterator times_iterator				= {
					   .stream_graph  = stream,
					   .iterator_data = iterator_data,
					   .next		  = LinkStream_TimesNodePresent_next,
					   .destroy		  = LinkStream_TimesNodePresentIterator_destroy,
	   };
	return times_iterator;
}

TimesIterator LinkStream_times_link_present(StreamData* stream_data, LinkId link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(link_id < link_stream->underlying_stream_graph->links.nb_links);

	return StreamGraph_times_link_present(link_stream->underlying_stream_graph, link_id);
}

Link LinkStream_link_by_id(StreamData* stream_data, size_t link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(link_id < link_stream->underlying_stream_graph->links.nb_links);

	return link_stream->underlying_stream_graph->links.links[link_id];
}

LinksIterator LinkStream_neighbours_of_node(StreamData* stream_data, NodeId node_id) {
	LinkStream* link_stream	  = (LinkStream*)stream_data;
	StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return StreamGraph_neighbours_of_node(stream_graph, node_id);
}

const StreamFunctions LinkStream_stream_functions = {
	.nodes_set			= LinkStream_nodes_set,
	.links_set			= LinkStream_links_set,
	.lifespan			= LinkStream_lifespan,
	.scaling			= LinkStream_scaling,
	.nodes_present_at_t = LinkStream_nodes_present_at_t,
	.links_present_at_t = LinkStream_links_present_at_t,
	.times_node_present = LinkStream_times_node_present,
	.times_link_present = LinkStream_times_link_present,
	.link_by_id			= LinkStream_link_by_id,
	.neighbours_of_node = LinkStream_neighbours_of_node,
};

double LS_coverage(Stream* stream_data) {
	return 1.0;
}

size_t LS_cardinal_of_T(Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t t	   = Interval_duration(StreamGraph_lifespan(ls->underlying_stream_graph));
	UPDATE_CACHE(stream, cardinalOfT, t);
	return t;
}

size_t LS_cardinal_of_V(Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t v	   = ls->underlying_stream_graph->nodes.nb_nodes;
	UPDATE_CACHE(stream, cardinalOfV, v);
	return v;
}

size_t LS_cardinal_of_W(Stream* stream) {
	size_t v = LS_cardinal_of_V(stream);
	size_t t = LS_cardinal_of_T(stream);
	size_t w = v * t;
	UPDATE_CACHE(stream, cardinalOfW, w);
	return w;
}

size_t LS_cardinal_of_E(Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t e	   = 0;
	for (size_t i = 0; i < ls->underlying_stream_graph->links.nb_links; i++) {
		Link link = ls->underlying_stream_graph->links.links[i];
		for (size_t j = 0; j < link.presence.nb_intervals; j++) {
			e += Interval_duration(link.presence.intervals[j]);
		}
	}
	UPDATE_CACHE(stream, cardinalOfE, e);
	return e;
}

double density(Stream* stream) {
	size_t n = LS_cardinal_of_V(stream);
	// we multiply by 2 here because it's faster to double an integer than to double a float
	double m = (double)(2 * LS_cardinal_of_E(stream)) / (double)LS_cardinal_of_T(stream);
	return m / (double)(n * (n - 1));
}

const MetricsFunctions LinkStream_metrics_functions = {
	.coverage	   = LS_coverage,
	.cardinalOfT   = NULL,
	.cardinalOfV   = LS_cardinal_of_V,
	.cardinalOfW   = LS_cardinal_of_W,
	.node_duration = NULL,
	.density	   = density,
};