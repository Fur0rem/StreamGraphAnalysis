#include "chunk_stream.h"
#include "full_stream_graph.h"

#include <stddef.h>
#include <stdlib.h>

ChunkStream ChunkStream_with(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
							 size_t time_end) {
	ChunkStream chunk_stream = (ChunkStream){
		.underlying_stream_graph = stream_graph,
		.snapshot				 = Interval_from(time_start, time_end),
		.nodes_present			 = BitArray_n_zeros(stream_graph->nodes.nb_nodes),
		.links_present			 = BitArray_n_zeros(stream_graph->links.nb_links),
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

Stream CS_with(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
			   size_t time_end) {
	ChunkStream* chunk_stream = MALLOC(sizeof(ChunkStream));
	*chunk_stream			  = ChunkStream_with(stream_graph, nodes, links, time_start, time_end);
	Stream stream			  = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	init_cache(&stream);
	return stream;
}

ChunkStream ChunkStream_without(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
								size_t time_end) {
	ChunkStream chunk_stream = (ChunkStream){
		.underlying_stream_graph = stream_graph,
		.snapshot				 = Interval_from(time_start, time_end),
		.nodes_present			 = BitArray_n_ones(stream_graph->nodes.nb_nodes),
		.links_present			 = BitArray_n_ones(stream_graph->links.nb_links),
	};
	for (size_t i = 0; i < nodes->size; i++) {
		BitArray_set_zero(chunk_stream.nodes_present, nodes->array[i]);
	}
	for (size_t i = 0; i < links->size; i++) {
		// If the two nodes are not present in the chunk stream, then the link is not present
		Link link = stream_graph->links.links[links->array[i]];
		if (BitArray_is_zero(chunk_stream.nodes_present, link.nodes[0]) ||
			BitArray_is_zero(chunk_stream.nodes_present, link.nodes[1])) {
			BitArray_set_zero(chunk_stream.links_present, links->array[i]);
		}
	}
	return chunk_stream;
}

Stream CS_without(StreamGraph* stream_graph, NodeIdVector* nodes, LinkIdVector* links, size_t time_start,
				  size_t time_end) {
	ChunkStream* chunk_stream = MALLOC(sizeof(ChunkStream));
	*chunk_stream			  = ChunkStream_without(stream_graph, nodes, links, time_start, time_end);
	Stream stream			  = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	init_cache(&stream);
	return stream;
} // TODO : rename CS to ChunkStream

void CS_destroy(Stream stream) {
	ChunkStream* chunk_stream = (ChunkStream*)stream.stream_data;
	BitArray_destroy(chunk_stream->nodes_present);
	BitArray_destroy(chunk_stream->links_present);
	free(chunk_stream);
}

typedef struct {
	NodeId current_node;
} NodesSetIteratorData;

size_t CS_NodesSet_next(NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream			  = (ChunkStream*)iter->stream_graph.stream_data;
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

NodesIterator ChunkStream_nodes_set(StreamData* stream_data) {
	ChunkStream* chunk_stream			= (ChunkStream*)stream_data;
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node			= 0;
	Stream stream						= {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	NodesIterator nodes_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = CS_NodesSet_next,
		.destroy	   = CS_NodesSetIterator_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	LinkId current_link;
} CS_LinksSetIteratorData;

size_t CS_LinksSet_next(LinksIterator* iter) {
	CS_LinksSetIteratorData* links_iter_data = (CS_LinksSetIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream				 = (ChunkStream*)iter->stream_graph.stream_data;
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

LinksIterator ChunkStream_links_set(StreamData* stream_data) {
	ChunkStream* chunk_stream			   = (ChunkStream*)stream_data;
	CS_LinksSetIteratorData* iterator_data = MALLOC(sizeof(CS_LinksSetIteratorData));
	iterator_data->current_link			   = 0;
	Stream stream						   = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	LinksIterator links_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = CS_LinksSet_next,
		.destroy	   = CS_LinksSetIterator_destroy,
	};
	return links_iterator;
}

Interval ChunkStream_lifespan(StreamData* stream_data) {
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return chunk_stream->snapshot;
}

size_t ChunkStream_scaling(StreamData* stream_data) {
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return chunk_stream->underlying_stream_graph->scaling;
}

typedef struct {
	NodeId node_to_get_neighbours;
	NodeId current_neighbour;
} CS_NeighboursOfNodeIteratorData;

size_t ChunkStream_NeighboursOfNode_next(LinksIterator* iter) {
	CS_NeighboursOfNodeIteratorData* neighbours_iter_data = (CS_NeighboursOfNodeIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream							  = (ChunkStream*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph							  = chunk_stream->underlying_stream_graph;
	NodeId node											  = neighbours_iter_data->node_to_get_neighbours;
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

LinksIterator ChunkStream_neighbours_of_node(StreamData* stream_data, NodeId node) {
	ChunkStream* chunk_stream					   = (ChunkStream*)stream_data;
	CS_NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(CS_NeighboursOfNodeIteratorData));
	*iterator_data								   = (CS_NeighboursOfNodeIteratorData){
										.node_to_get_neighbours = node,
										.current_neighbour		= 0,
	};
	Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	LinksIterator neighbours_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = ChunkStream_NeighboursOfNode_next,
		.destroy	   = ChunkStream_NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

typedef struct {
	size_t current_time;
	size_t current_id;
} CS_TimesIdPresentAtIteratorData;

Interval CS_TimesNodePresentAt_next(TimesIterator* iter) {
	CS_TimesIdPresentAtIteratorData* times_iter_data = (CS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream						 = (ChunkStream*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph						 = chunk_stream->underlying_stream_graph;
	NodeId node										 = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	nth_time		  = Interval_intersection(nth_time, chunk_stream->snapshot);

	times_iter_data->current_time++;
	return nth_time;
}

void CS_TimesNodePresentAtIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator ChunkStream_times_node_present(StreamData* stream_data, NodeId node) {
	ChunkStream* chunk_stream					   = (ChunkStream*)stream_data;
	CS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CS_TimesIdPresentAtIteratorData));
	size_t nb_skips								   = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
		   chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end <
			   chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id	  = node,
	};
	Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	TimesIterator times_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = CS_TimesNodePresentAt_next,
		.destroy	   = CS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
Interval CS_TimesLinkPresentAt_next(TimesIterator* iter) {
	CS_TimesIdPresentAtIteratorData* times_iter_data = (CS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream						 = (ChunkStream*)iter->stream_graph.stream_data;
	StreamGraph* stream_graph						 = chunk_stream->underlying_stream_graph;
	LinkId link										 = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	nth_time		  = Interval_intersection(nth_time, chunk_stream->snapshot);
	times_iter_data->current_time++;
	return nth_time;
}

TimesIterator ChunkStream_times_link_present(StreamData* stream_data, LinkId link) {
	ChunkStream* chunk_stream					   = (ChunkStream*)stream_data;
	CS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CS_TimesIdPresentAtIteratorData));
	size_t nb_skips								   = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
		   chunk_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end <
			   chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CS_TimesIdPresentAtIteratorData){
		.current_time = nb_skips,
		.current_id	  = link,
	};
	Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	TimesIterator times_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = CS_TimesLinkPresentAt_next,
		.destroy	   = CS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

Link ChunkStream_link_by_id(StreamData* stream_data, size_t link_id) {
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return chunk_stream->underlying_stream_graph->links.links[link_id];
}

typedef struct {
	NodesIterator nodes_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamNPATIterData;

NodeId ChunkStreamNPAT_next(NodesIterator* iter) {
	// call the next function of the underlying iterator
	ChunkStreamNPATIterData* iterator_data = (ChunkStreamNPATIterData*)iter->iterator_data;
	ChunkStream* chunk_stream			   = (ChunkStream*)iter->stream_graph.stream_data;
	NodeId node							   = iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
	// if the node is not present in the chunk stream, call the next function again
	while (node != SIZE_MAX && BitArray_is_zero(chunk_stream->nodes_present, node)) {
		node = iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
	}
	return node;
}

void ChunkStreamNPAT_destroy(NodesIterator* iterator) {
	ChunkStreamNPATIterData* iterator_data = (ChunkStreamNPATIterData*)iterator->iterator_data;
	iterator_data->nodes_iterator_fsg.destroy(&iterator_data->nodes_iterator_fsg);
	free(iterator_data->underlying_stream_graph);
	free(iterator->iterator_data);
}

// TRICK : kind of weird hack but it works ig?
NodesIterator ChunkStream_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	ChunkStream* chunk_stream			   = (ChunkStream*)stream_data;
	ChunkStreamNPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamNPATIterData));
	/*FullStreamGraph* full_stream_graph = MALLOC(sizeof(FullStreamGraph));
	 *full_stream_graph = FullStreamGraph_from(chunk_stream->underlying_stream_graph);*/
	Stream fsg						   = FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* full_stream_graph = (FullStreamGraph*)fsg.stream_data;
	iterator_data->nodes_iterator_fsg = FullStreamGraph_stream_functions.nodes_present_at_t(full_stream_graph, instant);
	iterator_data->underlying_stream_graph = full_stream_graph;

	Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	NodesIterator nodes_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = ChunkStreamNPAT_next,
		.destroy	   = ChunkStreamNPAT_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	LinksIterator links_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamLPATIterData;

LinkId ChunkStreamLPAT_next(LinksIterator* iter) {
	// call the next function of the underlying iterator
	ChunkStreamLPATIterData* iterator_data = (ChunkStreamLPATIterData*)iter->iterator_data;
	ChunkStream* chunk_stream			   = (ChunkStream*)iter->stream_graph.stream_data;
	LinkId link							   = iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
	// if the link is not present in the chunk stream, call the next function again
	while (link != SIZE_MAX && BitArray_is_zero(chunk_stream->links_present, link)) {
		link = iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
	}
	return link;
}

void ChunkStreamLPAT_destroy(LinksIterator* iterator) {
	ChunkStreamLPATIterData* iterator_data = (ChunkStreamLPATIterData*)iterator->iterator_data;
	iterator_data->links_iterator_fsg.destroy(&iterator_data->links_iterator_fsg);
	free(iterator_data->underlying_stream_graph);
	free(iterator->iterator_data);
}

LinksIterator ChunkStream_links_present_at_t(StreamData* stream_data, TimeId instant) {
	ChunkStream* chunk_stream			   = (ChunkStream*)stream_data;
	ChunkStreamLPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamLPATIterData));
	Stream fsg							   = FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* full_stream_graph	   = (FullStreamGraph*)fsg.stream_data;
	iterator_data->links_iterator_fsg = FullStreamGraph_stream_functions.links_present_at_t(full_stream_graph, instant);
	iterator_data->underlying_stream_graph = full_stream_graph;
	Stream stream						   = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	LinksIterator links_iterator = {
		.stream_graph  = stream,
		.iterator_data = iterator_data,
		.next		   = ChunkStreamLPAT_next,
		.destroy	   = ChunkStreamLPAT_destroy,
	};
	return links_iterator;
}

const StreamFunctions ChunkStream_stream_functions = {
	.nodes_set			= ChunkStream_nodes_set,
	.links_set			= ChunkStream_links_set,
	.lifespan			= ChunkStream_lifespan,
	.scaling			= ChunkStream_scaling,
	.nodes_present_at_t = ChunkStream_nodes_present_at_t,
	.links_present_at_t = ChunkStream_links_present_at_t,
	.times_node_present = ChunkStream_times_node_present,
	.times_link_present = ChunkStream_times_link_present,
	.link_by_id			= ChunkStream_link_by_id,
	.neighbours_of_node = ChunkStream_neighbours_of_node,
};

size_t ChunkStream_cardinal_of_v(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfV);

	StreamData* stream_data	  = stream->stream_data;
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	size_t v				  = BitArray_count_ones(&chunk_stream->nodes_present);

	UPDATE_CACHE(stream, cardinalOfV, v);
	return v;
}

size_t ChunkStream_cardinal_of_t(Stream* stream) {
	StreamData* stream_data	  = stream->stream_data;
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return Interval_duration(chunk_stream->snapshot);
}

const MetricsFunctions ChunkStream_metrics_functions = {
	.cardinalOfW   = NULL,
	.cardinalOfT   = NULL,
	.cardinalOfV   = ChunkStream_cardinal_of_v,
	.coverage	   = NULL,
	.node_duration = NULL,
};
