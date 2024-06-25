#include "chunk_stream.h"

#include <stddef.h>
#include <stdlib.h>

ChunkStream ChunkStream_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
							 size_t time_end) {
	ChunkStream chunk_stream = (ChunkStream){
		.underlying_stream_graph = stream_graph,
		.snapshot = Interval_from(time_start, time_end),
		.nodes_present = BitArray_n_zeros(stream_graph->nodes.nb_nodes),
		.links_present = BitArray_n_zeros(stream_graph->links.nb_links),
	};
	for (size_t i = 0; i < nodes->size; i++) {
		BitArray_set_one(chunk_stream.nodes_present, nodes->array[i]);
	}
	for (size_t i = 0; i < links->size; i++) {
		// If the two nodes are not present in the chunk stream, then the link is not present
		Link link = stream_graph->links.links[links->array[i]];
		if (BitArray_is_one(chunk_stream.nodes_present, link.nodes[0]) &&
			BitArray_is_one(chunk_stream.nodes_present, link.nodes[1])) {
			BitArray_set_one(chunk_stream.links_present, links->array[i]);
		}
	}
	return chunk_stream;
}

Stream CS_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
			   size_t time_end) {
	ChunkStream* chunk_stream = MALLOC(sizeof(ChunkStream));
	*chunk_stream = ChunkStream_from(stream_graph, nodes, links, time_start, time_end);
	Stream stream = {.type = CHUNK_STREAM, .stream = chunk_stream};
	return stream;
}

void CS_destroy(Stream stream) {
	ChunkStream* chunk_stream = (ChunkStream*)stream.stream;
	BitArray_destroy(chunk_stream->nodes_present);
	BitArray_destroy(chunk_stream->links_present);
	free(chunk_stream);
}

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

size_t CS_NodesSet_next(NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream = (ChunkStream*)iter->stream_graph.stream;
	if (nodes_iter_data->current_node >= chunk_stream->underlying_stream_graph->nodes.nb_nodes) {
		return SIZE_MAX;
	}
	size_t return_val = BitArray_leading_zeros_from(chunk_stream->nodes_present, nodes_iter_data->current_node);
	nodes_iter_data->current_node += return_val + 1;
	if (nodes_iter_data->current_node > chunk_stream->underlying_stream_graph->nodes.nb_nodes) {
		return SIZE_MAX;
	}
	return nodes_iter_data->current_node - 1;
}

void CS_NodesSetIterator_destroy(NodesIterator* iterator) {
	free(iterator->iterator_data);
}

NodesIterator ChunkStream_nodes_set(ChunkStream* chunk_stream) {
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node = 0;
	Stream stream = {.type = CHUNK_STREAM, .stream = chunk_stream};
	NodesIterator nodes_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (size_t(*)(void*))CS_NodesSet_next,
		.destroy = (void (*)(void*))CS_NodesSetIterator_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	LinkId current_link;
} CS_LinksSetIteratorData;

size_t CS_LinksSet_next(LinksIterator* iter) {
	CS_LinksSetIteratorData* links_iter_data = (CS_LinksSetIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream = (ChunkStream*)iter->stream_graph.stream;
	if (links_iter_data->current_link >= chunk_stream->underlying_stream_graph->links.nb_links) {
		return SIZE_MAX;
	}
	size_t return_val = BitArray_leading_zeros_from(chunk_stream->links_present, links_iter_data->current_link);
	links_iter_data->current_link += return_val + 1;
	if (links_iter_data->current_link > chunk_stream->underlying_stream_graph->links.nb_links) {
		return SIZE_MAX;
	}
	return links_iter_data->current_link - 1;
}

void CS_LinksSetIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator ChunkStream_links_set(ChunkStream* chunk_stream) {
	CS_LinksSetIteratorData* iterator_data = MALLOC(sizeof(CS_LinksSetIteratorData));
	iterator_data->current_link = 0;
	Stream stream = {.type = CHUNK_STREAM, .stream = chunk_stream};
	LinksIterator links_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (size_t(*)(void*))CS_LinksSet_next,
		.destroy = (void (*)(void*))CS_LinksSetIterator_destroy,
	};
	return links_iterator;
}

Interval ChunkStream_lifespan(ChunkStream* chunk_stream) {
	return chunk_stream->snapshot;
}

size_t ChunkStream_scaling(ChunkStream* chunk_stream) {
	return chunk_stream->underlying_stream_graph->scaling;
}

DEFAULT_TO_STRING(NodeId, "%zu");
DEFAULT_COMPARE(NodeId);

DEFAULT_TO_STRING(LinkId, "%zu");
DEFAULT_COMPARE(LinkId);

typedef struct {
	NodeId node_to_get_neighbours;
	NodeId current_neighbour;
} CS_NeighboursOfNodeIteratorData;

size_t ChunkStream_NeighboursOfNode_next(LinksIterator* iter) {
	CS_NeighboursOfNodeIteratorData* neighbours_iter_data = (CS_NeighboursOfNodeIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream = (ChunkStream*)iter->stream_graph.stream;
	StreamGraph* stream_graph = chunk_stream->underlying_stream_graph;
	NodeId node = neighbours_iter_data->node_to_get_neighbours;
	// Print all the neighbours of the node
	if (neighbours_iter_data->current_neighbour >= stream_graph->nodes.nodes[node].nb_neighbours) {
		return SIZE_MAX;
	}
	size_t return_val =
		chunk_stream->underlying_stream_graph->nodes.nodes[node].neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	if (BitArray_is_zero(chunk_stream->links_present, return_val)) {
		return ChunkStream_NeighboursOfNode_next(iter);
	}
	return return_val;
}

void ChunkStream_NeighboursOfNodeIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator ChunkStream_neighbours_of_node(ChunkStream* chunk_stream, NodeId node) {
	CS_NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(CS_NeighboursOfNodeIteratorData));
	*iterator_data = (CS_NeighboursOfNodeIteratorData){
		.node_to_get_neighbours = node,
		.current_neighbour = 0,
	};
	Stream stream = {.type = CHUNK_STREAM, .stream = chunk_stream};
	LinksIterator neighbours_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (size_t(*)(void*))ChunkStream_NeighboursOfNode_next,
		.destroy = (void (*)(void*))ChunkStream_NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

typedef struct {
	size_t current_time;
	size_t current_id;
} CS_TimesIdPresentAtIteratorData;

Interval CS_TimesNodePresentAt_next(TimesIterator* iter) {
	CS_TimesIdPresentAtIteratorData* times_iter_data = (CS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream = (ChunkStream*)iter->stream_graph.stream;
	StreamGraph* stream_graph = chunk_stream->underlying_stream_graph;
	NodeId node = times_iter_data->current_id;
	Interval return_val;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	if (nth_time.start >= chunk_stream->snapshot.end) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	if (nth_time.start <= chunk_stream->snapshot.start) {
		if (nth_time.end >= chunk_stream->snapshot.end) {
			times_iter_data->current_time++;
			return Interval_from(chunk_stream->snapshot.start, chunk_stream->snapshot.end);
		}
		if (nth_time.end <= chunk_stream->snapshot.start) {
			return Interval_from(SIZE_MAX, SIZE_MAX);
		}
		times_iter_data->current_time++;
		return Interval_from(chunk_stream->snapshot.start, nth_time.end);
	}
	if (nth_time.end >= chunk_stream->snapshot.end) {
		times_iter_data->current_time++;
		return Interval_from(nth_time.start, chunk_stream->snapshot.end);
	}
	times_iter_data->current_time++;
	return nth_time;
}

void CS_TimesNodePresentAtIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator ChunkStream_times_node_present(ChunkStream* chunk_stream, NodeId node) {
	CS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CS_TimesIdPresentAtIteratorData));
	size_t nb_skips = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
		   chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end <
			   chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id = node,
	};
	Stream stream = {.type = CHUNK_STREAM, .stream = chunk_stream};
	TimesIterator times_iterator = {
		.stream_graph = stream,
		.iterator_data = iterator_data,
		.next = (Interval(*)(void*))CS_TimesNodePresentAt_next,
		.destroy = (void (*)(void*))CS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

const StreamFunctions ChunkStream_stream_functions = {
	.nodes_set = (NodesIterator(*)(void*))ChunkStream_nodes_set,
	.links_set = (LinksIterator(*)(void*))ChunkStream_links_set,
	.lifespan = (Interval(*)(void*))ChunkStream_lifespan,
	.scaling = (size_t(*)(void*))ChunkStream_scaling,
	/*.nodes_present_at_t = (NodesIterator(*)(void*, TimeId))ChunkStream_nodes_present_at_t,
	.links_present_at_t = (LinksIterator(*)(void*, TimeId))ChunkStream_links_present_at_t,*/
	.times_node_present = (TimesIterator(*)(void*, NodeId))ChunkStream_times_node_present,
	/*.times_link_present = (TimesIterator(*)(void*, LinkId))ChunkStream_times_link_present,
	.nth_link = (Link(*)(void*, size_t))ChunkStream_nth_link,*/
	.neighbours_of_node = (LinksIterator(*)(void*, NodeId))ChunkStream_neighbours_of_node,
};

const MetricsFunctions ChunkStream_metrics_functions = {
	.cardinalOfW = NULL,
	.cardinalOfT = NULL,
	.cardinalOfV = NULL,
	.coverage = NULL,
	.node_duration = NULL,
};