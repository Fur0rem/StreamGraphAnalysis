#define SGA_INTERNAL

#include "snapshot_stream.h"

#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/key_moments.h"
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"

#include <stddef.h>
#include <stdlib.h>

SGA_Stream SGA_SnapshotStream_from(SGA_StreamGraph* stream_graph, Interval snapshot) {
	SnapshotStream* snapshot_stream = MALLOC(sizeof(SnapshotStream));
	// *snapshot_stream = SnapshotStream_with(stream_graph, nodes, links, time_start, time_end);
	*snapshot_stream = (SnapshotStream){
	    .underlying_stream_graph = stream_graph,
	    .snapshot		     = snapshot,
	};
	SGA_Stream stream = {.type = SNAPSHOT_STREAM, .stream_data = snapshot_stream};
	init_cache(&stream);
	return stream;
}

void SGA_SnapshotStream_destroy(SGA_Stream stream) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream.stream_data;
	free(snapshot_stream);
}

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

NodesIterator SnapshotStream_nodes_set(SGA_StreamData* stream_data) {
	SnapshotStream* snap	      = (SnapshotStream*)stream_data;
	SGA_StreamGraph* stream_graph = snap->underlying_stream_graph;
	return SGA_StreamGraph_nodes_set(stream_graph);
}

LinksIterator SnapshotStream_links_set(SGA_StreamData* stream_data) {
	SnapshotStream* snap	      = (SnapshotStream*)stream_data;
	SGA_StreamGraph* stream_graph = snap->underlying_stream_graph;
	return SGA_StreamGraph_links_set(stream_graph);
}

Interval SnapshotStream_lifespan(SGA_StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	return snapshot_stream->snapshot;
}

size_t SnapshotStream_time_scale(SGA_StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	return SGA_StreamGraph_time_scale(snapshot_stream->underlying_stream_graph);
}

NodesIterator SnapshotStream_nodes_present_at_t(SGA_StreamData* stream_data, TimeId instant) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	SGA_StreamGraph* stream_graph	= snapshot_stream->underlying_stream_graph;

	ASSERT(Interval_contains(snapshot_stream->snapshot, instant));

	return SGA_StreamGraph_nodes_present_at(stream_graph, instant);
}

LinksIterator SnapshotStream_links_present_at_t(SGA_StreamData* stream_data, TimeId instant) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	SGA_StreamGraph* stream_graph	= snapshot_stream->underlying_stream_graph;

	ASSERT(Interval_contains(snapshot_stream->snapshot, instant));

	return SGA_StreamGraph_links_present_at(stream_graph, instant);
}

LinksIterator SnapshotStream_neighbours_of_node(SGA_StreamData* stream_data, NodeId node) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	SGA_StreamGraph* stream_graph	= snapshot_stream->underlying_stream_graph;

	ASSERT(node < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_neighbours_of_node(stream_graph, node);
}

// TODO: maybe switch to map and filter when those are done?
typedef struct {
	size_t current_time;
	size_t current_id;
} SSS_TimesIdPresentAtIteratorData; // TODO : rename ?

Interval SSS_TimesNodePresentAt_next(TimesIterator* iter) {
	SSS_TimesIdPresentAtIteratorData* times_iter_data = (SSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	SnapshotStream* snapshot_stream			  = (SnapshotStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			  = snapshot_stream->underlying_stream_graph;
	NodeId node					  = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	nth_time	  = Interval_intersection(nth_time, snapshot_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

void SSS_TimesNodePresentAtIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator SnapshotStream_times_node_present(SGA_StreamData* stream_data, NodeId node) {
	SnapshotStream* snapshot_stream			= (SnapshotStream*)stream_data;
	SSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(SSS_TimesIdPresentAtIteratorData));
	size_t nb_skips					= 0;
	while (nb_skips < snapshot_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
	       snapshot_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end <
		   snapshot_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (SSS_TimesIdPresentAtIteratorData){
	    .current_time = nb_skips,
	    .current_id	  = node,
	};
	SGA_Stream stream = {.type = SNAPSHOT_STREAM, .stream_data = snapshot_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = snapshot_stream;
	// stream->stream = snapshot_stream;
	TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = SSS_TimesNodePresentAt_next,
	    .destroy	   = SSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
Interval SSS_TimesLinkPresentAt_next(TimesIterator* iter) {
	SSS_TimesIdPresentAtIteratorData* times_iter_data = (SSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	SnapshotStream* snapshot_stream			  = (SnapshotStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			  = snapshot_stream->underlying_stream_graph;
	LinkId link					  = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	nth_time	  = Interval_intersection(nth_time, snapshot_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

TimesIterator SnapshotStream_times_link_present(SGA_StreamData* stream_data, LinkId link) {
	SnapshotStream* snapshot_stream			= (SnapshotStream*)stream_data;
	SSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(SSS_TimesIdPresentAtIteratorData));
	size_t nb_skips					= 0;
	while (nb_skips < snapshot_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
	       snapshot_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end <
		   snapshot_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (SSS_TimesIdPresentAtIteratorData){
	    .current_time = nb_skips,
	    .current_id	  = link,
	};
	SGA_Stream stream = {.type = SNAPSHOT_STREAM, .stream_data = snapshot_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = snapshot_stream;
	// stream->stream = snapshot_stream;
	TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = SSS_TimesLinkPresentAt_next,
	    .destroy	   = SSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

Link SnapshotStream_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	return SGA_StreamGraph_link_by_id(snapshot_stream->underlying_stream_graph, link_id);
}

TimesIterator SnapshotStream_key_moments(SGA_StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	SGA_StreamGraph* stream_graph	= snapshot_stream->underlying_stream_graph;
	return SGA_StreamGraph_key_moments_between(stream_graph, snapshot_stream->snapshot);
}

const StreamFunctions SnapshotStream_stream_functions = {
    .nodes_set		= SnapshotStream_nodes_set,
    .links_set		= SnapshotStream_links_set,
    .lifespan		= SnapshotStream_lifespan,
    .time_scale		= SnapshotStream_time_scale,
    .nodes_present_at_t = SnapshotStream_nodes_present_at_t,
    .links_present_at_t = SnapshotStream_links_present_at_t,
    .times_node_present = SnapshotStream_times_node_present,
    .times_link_present = SnapshotStream_times_link_present,
    .link_by_id		= SnapshotStream_link_by_id,
    .neighbours_of_node = SnapshotStream_neighbours_of_node,
};

size_t SnapshotStream_distinct_cardinal_of_link_set(SGA_Stream* stream) {
	SGA_StreamData* stream_data   = stream->stream_data;
	SGA_StreamGraph* stream_graph = ((SnapshotStream*)stream_data)->underlying_stream_graph;
	return stream_graph->links.nb_links;
}

size_t SnapshotStream_distinct_cardinal_of_node_set(SGA_Stream* stream) {
	SGA_StreamData* stream_data   = stream->stream_data;
	SGA_StreamGraph* stream_graph = ((SnapshotStream*)stream_data)->underlying_stream_graph;
	return stream_graph->nodes.nb_nodes;
}

const MetricsFunctions SnapshotStream_metrics_functions = {
    .temporal_cardinal_of_node_set = NULL,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = SnapshotStream_distinct_cardinal_of_node_set,
    .distinct_cardinal_of_link_set = SnapshotStream_distinct_cardinal_of_link_set,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};
