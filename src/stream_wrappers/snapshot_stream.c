#include "snapshot_stream.h"
#include "chunk_stream.h"
#include "full_stream_graph.h" // TODO : switch with interval for interval filtering

#include <stddef.h>
#include <stdlib.h>

/*SnapshotStream SnapshotStream_from(StreamGraph* stream_graph, Interval snapshot) {
	SnapshotStream snapshot_stream = (SnapshotStream){
		.underlying_stream_graph = stream_graph,
		.snapshot = snapshot,
	};
	return snapshot_stream;
}*/

Stream SnapshotStream_from(StreamGraph* stream_graph, Interval snapshot) {
	SnapshotStream* snapshot_stream = MALLOC(sizeof(SnapshotStream));
	// *snapshot_stream = SnapshotStream_with(stream_graph, nodes, links, time_start, time_end);
	*snapshot_stream = (SnapshotStream){
		.underlying_stream_graph = stream_graph,
		.snapshot				 = snapshot,
	};
	Stream stream = {.type = SNAPSHOT_STREAM, .stream_data = snapshot_stream};
	init_cache(&stream);
	return stream;
}

void SnapshotStream_destroy(Stream stream) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream.stream_data;
	free(snapshot_stream);
}

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

// TODO: this leaks memory from the allocated streamgraph
NodesIterator SnapshotStream_nodes_set(StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	Stream full_stream_graph		= FullStreamGraph_from(snapshot_stream->underlying_stream_graph);
	FullStreamGraph* fsg			= (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.nodes_set(fsg);
}

LinksIterator SnapshotStream_links_set(StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	Stream full_stream_graph		= FullStreamGraph_from(snapshot_stream->underlying_stream_graph);
	FullStreamGraph* fsg			= (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.links_set(fsg);
}

Interval SnapshotStream_lifespan(StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	return snapshot_stream->snapshot;
}

size_t SnapshotStream_scaling(StreamData* stream_data) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	return snapshot_stream->underlying_stream_graph->scaling;
}

typedef struct {
	NodeId node_to_get_neighbours;
	NodeId current_neighbour;
} SSS_NeighboursOfNodeIteratorData;

NodesIterator SnapshotStream_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	Stream full_stream_graph		= FullStreamGraph_from(snapshot_stream->underlying_stream_graph);
	FullStreamGraph* fsg			= (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.nodes_present_at_t(fsg, instant);
}

LinksIterator SnapshotStream_links_present_at_t(StreamData* stream_data, TimeId instant) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	Stream full_stream_graph		= FullStreamGraph_from(snapshot_stream->underlying_stream_graph);
	FullStreamGraph* fsg			= (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.links_present_at_t(fsg, instant);
}

LinksIterator SnapshotStream_neighbours_of_node(StreamData* stream_data, NodeId node) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	Stream full_stream_graph		= FullStreamGraph_from(snapshot_stream->underlying_stream_graph);
	FullStreamGraph* fsg			= (FullStreamGraph*)full_stream_graph.stream_data;
	LinksIterator links_iterator	= FullStreamGraph_stream_functions.neighbours_of_node(fsg, node);
	return links_iterator;
}

typedef struct {
	size_t current_time;
	size_t current_id;
} SSS_TimesIdPresentAtIteratorData; // TODO : rename ?

Interval SSS_TimesNodePresentAt_next(TimesIterator* iter) {
	SSS_TimesIdPresentAtIteratorData* times_iter_data = (SSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	SnapshotStream* snapshot_stream					  = (SnapshotStream*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph						  = snapshot_stream->underlying_stream_graph;
	NodeId node										  = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	nth_time		  = Interval_intersection(nth_time, snapshot_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

void SSS_TimesNodePresentAtIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator SnapshotStream_times_node_present(StreamData* stream_data, NodeId node) {
	SnapshotStream* snapshot_stream					= (SnapshotStream*)stream_data;
	SSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(SSS_TimesIdPresentAtIteratorData));
	size_t nb_skips									= 0;
	while (nb_skips < snapshot_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
		   snapshot_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end <
			   snapshot_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (SSS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id	  = node,
	};
	Stream stream = {.type = SNAPSHOT_STREAM, .stream_data = snapshot_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = snapshot_stream;
	// stream->stream = snapshot_stream;
	TimesIterator times_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = SSS_TimesNodePresentAt_next,
		.destroy	   = SSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
Interval SSS_TimesLinkPresentAt_next(TimesIterator* iter) {
	SSS_TimesIdPresentAtIteratorData* times_iter_data = (SSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	SnapshotStream* snapshot_stream					  = (SnapshotStream*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph						  = snapshot_stream->underlying_stream_graph;
	LinkId link										  = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	nth_time		  = Interval_intersection(nth_time, snapshot_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

TimesIterator SnapshotStream_times_link_present(StreamData* stream_data, LinkId link) {
	SnapshotStream* snapshot_stream					= (SnapshotStream*)stream_data;
	SSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(SSS_TimesIdPresentAtIteratorData));
	size_t nb_skips									= 0;
	while (nb_skips < snapshot_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
		   snapshot_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end <
			   snapshot_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (SSS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id	  = link,
	};
	Stream stream = {.type = SNAPSHOT_STREAM, .stream_data = snapshot_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = snapshot_stream;
	// stream->stream = snapshot_stream;
	TimesIterator times_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = SSS_TimesLinkPresentAt_next,
		.destroy	   = SSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

Link SnapshotStream_link_by_id(StreamData* stream_data, size_t link_id) {
	SnapshotStream* snapshot_stream = (SnapshotStream*)stream_data;
	return snapshot_stream->underlying_stream_graph->links.links[link_id];
}

const StreamFunctions SnapshotStream_stream_functions = {
	.nodes_set			= SnapshotStream_nodes_set,
	.links_set			= SnapshotStream_links_set,
	.lifespan			= SnapshotStream_lifespan,
	.scaling			= SnapshotStream_scaling,
	.nodes_present_at_t = SnapshotStream_nodes_present_at_t,
	.links_present_at_t = SnapshotStream_links_present_at_t,
	.times_node_present = SnapshotStream_times_node_present,
	.times_link_present = SnapshotStream_times_link_present,
	.link_by_id			= SnapshotStream_link_by_id,
	.neighbours_of_node = SnapshotStream_neighbours_of_node,
};

const MetricsFunctions SnapshotStream_metrics_functions = {
	.cardinalOfW   = NULL,
	.cardinalOfT   = NULL,
	.cardinalOfV   = NULL,
	.coverage	   = NULL,
	.node_duration = NULL,
};
