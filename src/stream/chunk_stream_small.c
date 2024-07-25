#include "chunk_stream_small.h"

#include "../interval.h"
#include "../iterators.h"
#include "../stream_graph.h"
#include "chunk_stream.h"
#include "full_stream_graph.h"
#include <stddef.h>

Stream CSS_from(StreamGraph* stream_graph, NodeId* nodes, LinkId* links, Interval snapshot, size_t nb_nodes,
				size_t nb_links) {
	ChunkStreamSmall* chunk_stream = MALLOC(sizeof(ChunkStreamSmall));
	*chunk_stream = (ChunkStreamSmall){
		.nb_nodes = nb_nodes,
		.nodes_present = nodes,
		.nb_links = nb_links,
		.links_present = links,
		.underlying_stream_graph = stream_graph,
		.snapshot = snapshot,
	};
	return (Stream){
		.type = CHUNK_STREAM_SMALL,
		.stream_data = chunk_stream,
	};
}

void ChunkStreamSmall_destroy(Stream stream) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream.stream_data;
	free(chunk_stream->nodes_present);
	free(chunk_stream->links_present);
	free(chunk_stream);
}

// OPTIMISE : use dichotomic search since the nodes are sorted
bool is_node_present(NodeId node, ChunkStreamSmall* chunk_stream) {
	for (size_t i = 0; i < chunk_stream->nb_nodes; i++) {
		if (chunk_stream->nodes_present[i] == node) {
			return true;
		}
	}
	return false;
}

bool is_link_present(LinkId link, ChunkStreamSmall* chunk_stream) {
	for (size_t i = 0; i < chunk_stream->nb_links; i++) {
		if (chunk_stream->links_present[i] == link) {
			return true;
		}
	}
	return false;
}

typedef struct {
	size_t current_node;
} ChunkStreamSmallNodesSetIteratorData;

NodeId ChunkStreamSmallNodesSetIterator_next(NodesIterator* it) {
	ChunkStreamSmallNodesSetIteratorData* iterator_data = it->iterator_data;
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (iterator_data->current_node >= chunk_stream->nb_nodes) {
		return SIZE_MAX;
	}
	NodeId current_node = chunk_stream->nodes_present[iterator_data->current_node];
	iterator_data->current_node++;
	return current_node;
}

void ChunkStreamSmallNodesSetIterator_destroy(NodesIterator* it) {
	free(it->iterator_data);
}

// OPTIMISE : use iterator_data directly as a size_t to have one less malloc and indirection
NodesIterator ChunkStreamSmall_nodes_set(StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	ChunkStreamSmallNodesSetIteratorData* iterator_data = MALLOC(sizeof(ChunkStreamSmallNodesSetIteratorData));
	iterator_data->current_node = 0;
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM_SMALL;
	// stream->stream = chunk_stream;
	Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	return (NodesIterator){
		.iterator_data = iterator_data,
		.next = ChunkStreamSmallNodesSetIterator_next,
		.destroy = ChunkStreamSmallNodesSetIterator_destroy,
		.stream_graph = stream,
	};
}

typedef struct {
	size_t current_link;
} ChunkStreamSmallLinksSetIteratorData;

LinkId ChunkStreamSmallLinksSetIterator_next(LinksIterator* it) {
	ChunkStreamSmallLinksSetIteratorData* iterator_data = it->iterator_data;
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (iterator_data->current_link >= chunk_stream->nb_links) {
		return SIZE_MAX;
	}
	LinkId current_link = chunk_stream->links_present[iterator_data->current_link];
	iterator_data->current_link++;
	return current_link;
}

void ChunkStreamSmallLinksSetIterator_destroy(LinksIterator* it) {
	free(it->iterator_data);
}

LinksIterator ChunkStreamSmall_links_set(StreamData* chunk_stream) {
	ChunkStreamSmallLinksSetIteratorData* iterator_data = MALLOC(sizeof(ChunkStreamSmallLinksSetIteratorData));
	iterator_data->current_link = 0;
	Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	return (LinksIterator){
		.iterator_data = iterator_data,
		.next = ChunkStreamSmallLinksSetIterator_next,
		.destroy = ChunkStreamSmallLinksSetIterator_destroy,
		.stream_graph = stream,
	};
}

Interval ChunkStreamSmall_lifespan(StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	return chunk_stream->snapshot;
}

size_t ChunkStreamSmall_scaling(StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	return chunk_stream->underlying_stream_graph->scaling;
}

typedef struct {
	NodesIterator nodes_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamSmallNPATIterData;

NodeId ChunkStreamSmallNodesPresentAtTIterator_next(NodesIterator* it) {
	ChunkStreamSmallNPATIterData* iterator_data = it->iterator_data;
	NodeId node_id = iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
	if (node_id == SIZE_MAX) {
		return SIZE_MAX;
	}
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (is_node_present(node_id, chunk_stream)) {
		return node_id;
	}
	return iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
}

void ChunkStreamSmallNodesPresentAtTIterator_destroy(NodesIterator* it) {
	ChunkStreamSmallNPATIterData* iterator_data = it->iterator_data;
	iterator_data->nodes_iterator_fsg.destroy(&iterator_data->nodes_iterator_fsg);
	free(iterator_data);
}

NodesIterator ChunkStreamSmall_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	Stream st = FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* fsg = (FullStreamGraph*)st.stream_data;
	NodesIterator nodes_iterator_fsg = FullStreamGraph_stream_functions.nodes_present_at_t(fsg, instant);
	ChunkStreamSmallNPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamSmallNPATIterData));
	iterator_data->nodes_iterator_fsg = nodes_iterator_fsg;
	iterator_data->underlying_stream_graph = fsg;
	Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	return (NodesIterator){
		.iterator_data = iterator_data,
		.next = ChunkStreamSmallNodesPresentAtTIterator_next,
		.destroy = ChunkStreamSmallNodesPresentAtTIterator_destroy,
		.stream_graph = stream,
	};
}

typedef struct {
	LinksIterator links_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamSmallLPATIterData;

LinkId ChunkStreamSmallLinksPresentAtTIterator_next(LinksIterator* it) {
	ChunkStreamSmallLPATIterData* iterator_data = it->iterator_data;
	FullStreamGraph* fsg = iterator_data->underlying_stream_graph;
	LinkId link_id = iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
	if (link_id == SIZE_MAX) {
		return SIZE_MAX;
	}
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (is_link_present(link_id, chunk_stream)) {
		return link_id;
	}

	return iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
}

void ChunkStreamSmallLinksPresentAtTIterator_destroy(LinksIterator* it) {
	ChunkStreamSmallLPATIterData* iterator_data = it->iterator_data;
	iterator_data->links_iterator_fsg.destroy(&iterator_data->links_iterator_fsg);
	free(iterator_data);
}

LinksIterator ChunkStreamSmall_links_present_at_t(StreamData* stream_data, TimeId instant) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	Stream st = FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* fsg = (FullStreamGraph*)st.stream_data;
	LinksIterator links_iterator_fsg = FullStreamGraph_stream_functions.links_present_at_t(fsg, instant);
	ChunkStreamSmallLPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamSmallLPATIterData));
	iterator_data->links_iterator_fsg = links_iterator_fsg;
	iterator_data->underlying_stream_graph = fsg;
	Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	return (LinksIterator){
		.iterator_data = iterator_data,
		.next = ChunkStreamSmallLinksPresentAtTIterator_next,
		.destroy = ChunkStreamSmallLinksPresentAtTIterator_destroy,
		.stream_graph = stream,
	};
}

typedef struct {
	NodeId node_to_get_neighbours;
	NodeId current_neighbour;
} CSS_NeighboursOfNodeIteratorData;

size_t ChunkStreamSmall_NeighboursOfNode_next(LinksIterator* iter) {
	CSS_NeighboursOfNodeIteratorData* neighbours_iter_data = (CSS_NeighboursOfNodeIteratorData*)iter->iterator_data;
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph = chunk_stream->underlying_stream_graph;
	NodeId node = neighbours_iter_data->node_to_get_neighbours;
	// Print all the neighbours of the node
	if (neighbours_iter_data->current_neighbour >= stream_graph->nodes.nodes[node].nb_neighbours) {
		return SIZE_MAX;
	}
	size_t return_val =
		chunk_stream->underlying_stream_graph->nodes.nodes[node].neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	/*if (BitArray_is_zero(chunk_stream->links_present, return_val)) {
		return ChunkStreamSmall_NeighboursOfNode_next(iter);
	}*/
	if (!is_link_present(return_val, chunk_stream)) {
		return ChunkStreamSmall_NeighboursOfNode_next(iter);
	}
	return return_val;
}

void ChunkStreamSmall_NeighboursOfNodeIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator ChunkStreamSmall_neighbours_of_node(StreamData* stream_data, NodeId node) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	CSS_NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(CSS_NeighboursOfNodeIteratorData));
	*iterator_data = (CSS_NeighboursOfNodeIteratorData){
		.node_to_get_neighbours = node,
		.current_neighbour = 0,
	};
	// Stream stream = {.type = CHUNK_STREAM_SMALL, .stream = chunk_stream};
	Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	LinksIterator neighbours_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = ChunkStreamSmall_NeighboursOfNode_next,
		.destroy = ChunkStreamSmall_NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

typedef struct {
	size_t current_time;
	size_t current_id;
} CSS_TimesIdPresentAtIteratorData;

Interval CSS_TimesNodePresentAt_next(TimesIterator* iter) {
	CSS_TimesIdPresentAtIteratorData* times_iter_data = (CSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph = chunk_stream->underlying_stream_graph;
	NodeId node = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	filter_interval(&nth_time, chunk_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

void CSS_TimesNodePresentAtIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator ChunkStreamSmall_times_node_present(StreamData* stream_data, NodeId node) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	CSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CSS_TimesIdPresentAtIteratorData));
	size_t nb_skips = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
		   chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end <
			   chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CSS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id = node,
	};
	Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM_SMALL;
	// stream->stream = chunk_stream;
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = CSS_TimesNodePresentAt_next,
		.destroy = CSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
Interval CSS_TimesLinkPresentAt_next(TimesIterator* iter) {
	CSS_TimesIdPresentAtIteratorData* times_iter_data = (CSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph = chunk_stream->underlying_stream_graph;
	LinkId link = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	filter_interval(&nth_time, chunk_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

TimesIterator ChunkStreamSmall_times_link_present(StreamData* stream_data, LinkId link) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	CSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CSS_TimesIdPresentAtIteratorData));
	size_t nb_skips = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
		   chunk_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end <
			   chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CSS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id = link,
	};
	Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM_SMALL;
	// stream->stream = chunk_stream;
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = CSS_TimesLinkPresentAt_next,
		.destroy = CSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

Link ChunkStreamSmall_nth_link(StreamData* stream_data, size_t link_id) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	return chunk_stream->underlying_stream_graph->links.links[link_id];
}
const StreamFunctions ChunkStreamSmall_stream_functions = {
	.nodes_set = ChunkStreamSmall_nodes_set,
	.links_set = ChunkStreamSmall_links_set,
	.lifespan = ChunkStreamSmall_lifespan,
	.scaling = ChunkStreamSmall_scaling,
	.nodes_present_at_t = ChunkStreamSmall_nodes_present_at_t,
	.links_present_at_t = ChunkStreamSmall_links_present_at_t,
	.times_node_present = ChunkStreamSmall_times_node_present,
	.times_link_present = ChunkStreamSmall_times_link_present,
	.nth_link = ChunkStreamSmall_nth_link,
	.neighbours_of_node = ChunkStreamSmall_neighbours_of_node,
};

const MetricsFunctions ChunkStreamSmall_metrics_functions = {
	.cardinalOfW = NULL,
	.cardinalOfT = NULL,
	.cardinalOfV = NULL,
	.coverage = NULL,
	.node_duration = NULL,
};