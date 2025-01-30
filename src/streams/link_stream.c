#define SGA_INTERNAL

#include "link_stream.h"
#include "../analysis/metrics.h"
#include "../interval.h"
#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/key_instants.h" // TODO: maybe rename to time_access
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

SGA_Stream SGA_LinkStream_from(SGA_StreamGraph* stream_graph) {
	LinkStream* link_stream		     = MALLOC(sizeof(LinkStream));
	link_stream->underlying_stream_graph = stream_graph;
	SGA_Stream stream		     = {.type = LINK_STREAM, .stream_data = link_stream};
	init_cache(&stream);
	return stream;
}

void SGA_LinkStream_destroy(SGA_Stream stream) {
	free(stream.stream_data);
}

SGA_NodesIterator LinkStream_nodes_set(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_nodes_set(stream_graph);
}

SGA_LinksIterator LinkStream_links_set(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_links_set(stream_graph);
}

SGA_Interval LinkStream_lifespan(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_lifespan(stream_graph);
}

size_t LinkStream_time_scale(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_time_scale(stream_graph);
}

SGA_NodesIterator LinkStream_nodes_present_at_t(SGA_StreamData* stream_data, SGA_TimeId instant) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(SGA_Interval_contains(SGA_StreamGraph_lifespan(link_stream->underlying_stream_graph), instant));

	// In a linkstream, all nodes are present at all times
	return SGA_StreamGraph_nodes_set(link_stream->underlying_stream_graph);
}

SGA_LinksIterator LinkStream_links_present_at_t(SGA_StreamData* stream_data, SGA_TimeId instant) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(SGA_Interval_contains(SGA_StreamGraph_lifespan(link_stream->underlying_stream_graph), instant));

	return SGA_StreamGraph_links_present_at(link_stream->underlying_stream_graph, instant);
}

typedef struct {
	bool has_been_called;
} TimesNodePresentIteratorData;

SGA_Interval LinkStream_TimesNodePresent_next(SGA_TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = (TimesNodePresentIteratorData*)iter->iterator_data;
	LinkStream* ls				      = iter->stream_graph.stream_data;
	if (times_iter_data->has_been_called) {
		return SGA_TIMES_ITERATOR_END;
	}
	times_iter_data->has_been_called = true;
	return SGA_StreamGraph_lifespan(ls->underlying_stream_graph);
}

void LinkStream_TimesNodePresentIterator_destroy(SGA_TimesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_TimesIterator LinkStream_times_node_present(SGA_StreamData* link_stream, SGA_NodeId node_id) {
	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	SGA_Stream stream			    = {.stream_data = link_stream};
	iterator_data->has_been_called		    = false;
	SGA_TimesIterator times_iterator	    = {
		       .stream_graph  = stream,
		       .iterator_data = iterator_data,
		       .next	      = LinkStream_TimesNodePresent_next,
		       .destroy	      = LinkStream_TimesNodePresentIterator_destroy,
	   };
	return times_iterator;
}

SGA_TimesIterator LinkStream_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(link_id < link_stream->underlying_stream_graph->links.nb_links);

	return SGA_StreamGraph_times_link_present(link_stream->underlying_stream_graph, link_id);
}

SGA_Link LinkStream_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(link_id < link_stream->underlying_stream_graph->links.nb_links);

	return link_stream->underlying_stream_graph->links.links[link_id];
}

SGA_LinksIterator LinkStream_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node_id) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_neighbours_of_node(stream_graph, node_id);
}

SGA_TimesIterator LinkStream_key_instants(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	return SGA_StreamGraph_key_instants(stream_graph);
}

const StreamFunctions LinkStream_stream_functions = {
    .nodes_set		= LinkStream_nodes_set,
    .links_set		= LinkStream_links_set,
    .lifespan		= LinkStream_lifespan,
    .time_scale		= LinkStream_time_scale,
    .nodes_present_at_t = LinkStream_nodes_present_at_t,
    .links_present_at_t = LinkStream_links_present_at_t,
    .times_node_present = LinkStream_times_node_present,
    .times_link_present = LinkStream_times_link_present,
    .link_by_id		= LinkStream_link_by_id,
    .neighbours_of_node = LinkStream_neighbours_of_node,
    .key_instants	= LinkStream_key_instants,
};

double LS_coverage(SGA_Stream* stream_data) {
	return 1.0;
}

size_t LS_cardinal_of_T(SGA_Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t t       = SGA_Interval_duration(SGA_StreamGraph_lifespan(ls->underlying_stream_graph));
	UPDATE_CACHE(stream, duration, t);
	return t;
}

size_t LS_cardinal_of_V(SGA_Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t v       = ls->underlying_stream_graph->nodes.nb_nodes;
	UPDATE_CACHE(stream, distinct_cardinal_of_node_set, v);
	return v;
}

size_t LS_cardinal_of_W(SGA_Stream* stream) {
	size_t v = LS_cardinal_of_V(stream);
	size_t t = LS_cardinal_of_T(stream);
	size_t w = v * t;
	UPDATE_CACHE(stream, temporal_cardinal_of_node_set, w);
	return w;
}

size_t LS_cardinal_of_E(SGA_Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t e       = 0;
	for (size_t i = 0; i < ls->underlying_stream_graph->links.nb_links; i++) {
		SGA_Link link = ls->underlying_stream_graph->links.links[i];
		for (size_t j = 0; j < link.presence.nb_intervals; j++) {
			e += SGA_Interval_duration(link.presence.intervals[j]);
		}
	}
	UPDATE_CACHE(stream, temporal_cardinal_of_link_set, e);
	return e;
}

double LinkStream_density(SGA_Stream* stream) {
	size_t n = LS_cardinal_of_V(stream);
	// we multiply by 2 here because it's faster to double an integer than to double a float
	double m = (double)(2 * LS_cardinal_of_E(stream)) / (double)LS_cardinal_of_T(stream);
	return m / (double)(n * (n - 1));
}

size_t LinkStream_distinct_cardinal_of_link_set(SGA_Stream* stream) {
	LinkStream* link_stream	      = (LinkStream*)stream->stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return stream_graph->links.nb_links;
}

size_t LinkStream_distinct_cardinal_of_node_set(SGA_Stream* stream) {
	LinkStream* link_stream	      = (LinkStream*)stream->stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return stream_graph->nodes.nb_nodes;
}

const MetricsFunctions LinkStream_metrics_functions = {
    .coverage			   = LS_coverage,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = LS_cardinal_of_V,
    .temporal_cardinal_of_node_set = LS_cardinal_of_W,
    .node_duration		   = NULL,
    .density			   = LinkStream_density,
};