#include "chunk_stream.h"

#include <stddef.h>
#include <stdlib.h>

ChunkStream ChunkStream_from(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
							 size_t time_end) {
	ChunkStream chunk_stream = (ChunkStream){
		.underlying_stream_graph = stream_graph,
		.snapshot = Interval_from(time_start, time_end),
		.nodes_present = BitArray_with_n_bits(stream_graph->nodes.nb_nodes),
		.links_present = BitArray_with_n_bits(stream_graph->links.nb_links),
	};
	for (size_t i = 0; i < nodes->size; i++) {
		BitArray_set_one(chunk_stream.nodes_present, nodes->array[i]);
	}
	for (size_t i = 0; i < links->size; i++) {
		BitArray_set_one(chunk_stream.links_present, links->array[i]);
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

void CS_destroy(Stream* stream) {
	ChunkStream* chunk_stream = (ChunkStream*)stream->stream;
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

DEFAULT_TO_STRING(NodeId, "%zu");
DEFAULT_COMPARE(NodeId);

DEFAULT_TO_STRING(LinkId, "%zu");
DEFAULT_COMPARE(LinkId);

const StreamFunctions ChunkStream_stream_functions = {
	.nodes_set = (NodesIterator(*)(void*))ChunkStream_nodes_set,
	/*.links_set = (LinksIterator(*)(void*))ChunkStream_links_set,
	.lifespan = (Interval(*)(void*))ChunkStream_lifespan,
	.scaling = (size_t(*)(void*))ChunkStream_scaling,
	.nodes_present_at_t = (NodesIterator(*)(void*, TimeId))ChunkStream_nodes_present_at_t,
	.links_present_at_t = (LinksIterator(*)(void*, TimeId))ChunkStream_links_present_at_t,
	.times_node_present = (TimesIterator(*)(void*, NodeId))ChunkStream_times_node_present,
	.times_link_present = (TimesIterator(*)(void*, LinkId))ChunkStream_times_link_present,
	.nth_link = (Link(*)(void*, size_t))ChunkStream_nth_link,
	.neighbours_of_node = (LinksIterator(*)(void*, NodeId))ChunkStream_neighbours_of_node,*/
};

const MetricsFunctions ChunkStream_metrics_functions = {
	.cardinalOfW = NULL,
	.cardinalOfT = NULL,
	.cardinalOfV = NULL,
	.coverage = NULL,
	.node_duration = NULL,
};