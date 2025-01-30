#define SGA_INTERNAL

#include "chunk_stream.h"
#include "../stream_data_access/key_instants.h"
#include "full_stream_graph.h"

#include <stddef.h>
#include <stdlib.h>

SGA_Stream SGA_ChunkStream_with(SGA_StreamGraph* stream_graph, SGA_NodeIdArrayList* nodes_present, SGA_LinkIdArrayList* links_present,
				SGA_Interval timeframe) {
	ChunkStream* chunk_stream = MALLOC(sizeof(ChunkStream));

	// Initialise the ChunkStream
	*chunk_stream = (ChunkStream){
	    .underlying_stream_graph = stream_graph,
	    .timeframe		     = timeframe,
	    .nodes_present	     = BitArray_n_zeros(stream_graph->nodes.nb_nodes),
	    .links_present	     = BitArray_n_zeros(stream_graph->links.nb_links),
	};

	// Add the nodes
	for (size_t i = 0; i < nodes_present->length; i++) {
		BitArray_set_one(chunk_stream->nodes_present, nodes_present->array[i]);
	}

	// Add the links
	for (size_t i = 0; i < links_present->length; i++) {
		SGA_Link link = stream_graph->links.links[links_present->array[i]];
		// Check if the two nodes are present in the chunk stream before setting the link as present
		if (BitArray_is_one(chunk_stream->nodes_present, link.nodes[0]) &&
		    BitArray_is_one(chunk_stream->nodes_present, link.nodes[1])) {
			BitArray_set_one(chunk_stream->links_present, links_present->array[i]);
		}
	}

	return (SGA_Stream){
	    .type	 = CHUNK_STREAM,
	    .stream_data = chunk_stream,
	};
}

SGA_Stream SGA_ChunkStream_without(SGA_StreamGraph* stream_graph, SGA_NodeIdArrayList* nodes_absent, SGA_LinkIdArrayList* links_absent,
				   SGA_Interval timeframe) {
	ChunkStream* chunk_stream = MALLOC(sizeof(ChunkStream));

	// Initialise the ChunkStream
	*chunk_stream = (ChunkStream){
	    .underlying_stream_graph = stream_graph,
	    .timeframe		     = timeframe,
	    .nodes_present	     = BitArray_n_ones(stream_graph->nodes.nb_nodes),
	    .links_present	     = BitArray_n_ones(stream_graph->links.nb_links),
	};

	// Remove the nodes
	for (size_t i = 0; i < nodes_absent->length; i++) {
		BitArray_set_zero(chunk_stream->nodes_present, nodes_absent->array[i]);
	}

	// Remove the links
	for (size_t i = 0; i < links_absent->length; i++) {
		// If the two nodes are not present in the chunk stream, then the link is not present
		SGA_Link link = stream_graph->links.links[links_absent->array[i]];
		if (BitArray_is_zero(chunk_stream->nodes_present, link.nodes[0]) ||
		    BitArray_is_zero(chunk_stream->nodes_present, link.nodes[1])) {
			BitArray_set_zero(chunk_stream->links_present, links_absent->array[i]);
		}
	}

	return (SGA_Stream){
	    .type	 = CHUNK_STREAM,
	    .stream_data = chunk_stream,
	};
}

void SGA_ChunkStream_destroy(SGA_Stream stream) {
	ChunkStream* chunk_stream = (ChunkStream*)stream.stream_data;
	BitArray_destroy(chunk_stream->nodes_present);
	BitArray_destroy(chunk_stream->links_present);
	free(chunk_stream);
}

typedef struct {
	SGA_NodeId current_node;
} NodesSetIteratorData;

size_t CS_NodesSet_next(SGA_NodesIterator* iter) {
	NodesSetIteratorData* nodes_iter_data = (NodesSetIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream	      = (ChunkStream*)iter->stream_graph.stream_data;
	if (nodes_iter_data->current_node >= chunk_stream->underlying_stream_graph->nodes.nb_nodes) {
		return SGA_NODES_ITERATOR_END;
	}
	size_t return_val = BitArray_leading_zeros_from(chunk_stream->nodes_present, nodes_iter_data->current_node);
	nodes_iter_data->current_node += return_val + 1;
	if (nodes_iter_data->current_node > chunk_stream->underlying_stream_graph->nodes.nb_nodes) {
		return SGA_NODES_ITERATOR_END;
	}
	return nodes_iter_data->current_node - 1;
}

void CS_NodesSetIterator_destroy(SGA_NodesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_NodesIterator ChunkStream_nodes_set(SGA_StreamData* stream_data) {
	ChunkStream* chunk_stream	    = (ChunkStream*)stream_data;
	NodesSetIteratorData* iterator_data = MALLOC(sizeof(NodesSetIteratorData));
	iterator_data->current_node	    = 0;
	SGA_Stream stream		    = {.type = CHUNK_STREAM, .stream_data = chunk_stream};

	SGA_NodesIterator nodes_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = CS_NodesSet_next,
	    .destroy	   = CS_NodesSetIterator_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	SGA_LinkId current_link;
} CS_LinksSetIteratorData;

size_t CS_LinksSet_next(SGA_LinksIterator* iter) {
	CS_LinksSetIteratorData* links_iter_data = (CS_LinksSetIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream		 = (ChunkStream*)iter->stream_graph.stream_data;
	if (links_iter_data->current_link >= chunk_stream->underlying_stream_graph->links.nb_links) {
		return SGA_LINKS_ITERATOR_END;
	}
	size_t return_val = BitArray_leading_zeros_from(chunk_stream->links_present, links_iter_data->current_link);
	links_iter_data->current_link += return_val + 1;
	if (links_iter_data->current_link > chunk_stream->underlying_stream_graph->links.nb_links) {
		return SGA_LINKS_ITERATOR_END;
	}
	return links_iter_data->current_link - 1;
}

void CS_LinksSetIterator_destroy(SGA_LinksIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_LinksIterator ChunkStream_links_set(SGA_StreamData* stream_data) {
	ChunkStream* chunk_stream	       = (ChunkStream*)stream_data;
	CS_LinksSetIteratorData* iterator_data = MALLOC(sizeof(CS_LinksSetIteratorData));
	iterator_data->current_link	       = 0;

	SGA_Stream stream = {
	    .type	 = CHUNK_STREAM,
	    .stream_data = chunk_stream,
	};

	SGA_LinksIterator links_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = CS_LinksSet_next,
	    .destroy	   = CS_LinksSetIterator_destroy,
	};
	return links_iterator;
}

SGA_Interval ChunkStream_lifespan(SGA_StreamData* stream_data) {
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return chunk_stream->timeframe;
}

size_t ChunkStream_time_scale(SGA_StreamData* stream_data) {
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return chunk_stream->underlying_stream_graph->time_scale;
}

typedef struct {
	SGA_NodeId node_to_get_neighbours;
	SGA_NodeId current_neighbour;
} CS_NeighboursOfNodeIteratorData;

size_t ChunkStream_NeighboursOfNode_next(SGA_LinksIterator* iter) {
	CS_NeighboursOfNodeIteratorData* neighbours_iter_data = (CS_NeighboursOfNodeIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream			      = (ChunkStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			      = chunk_stream->underlying_stream_graph;
	SGA_NodeId node					      = neighbours_iter_data->node_to_get_neighbours;
	// Print all the neighbours of the node
	if (neighbours_iter_data->current_neighbour >= stream_graph->nodes.nodes[node].nb_neighbours) {
		return SGA_LINKS_ITERATOR_END;
	}
	size_t return_val = chunk_stream->underlying_stream_graph->nodes.nodes[node].neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	if (BitArray_is_zero(chunk_stream->links_present, return_val)) {
		return ChunkStream_NeighboursOfNode_next(iter);
	}
	return return_val;
}

void ChunkStream_NeighboursOfNodeIterator_destroy(SGA_LinksIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_LinksIterator ChunkStream_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node) {
	ChunkStream* chunk_stream		       = (ChunkStream*)stream_data;
	CS_NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(CS_NeighboursOfNodeIteratorData));
	*iterator_data				       = (CS_NeighboursOfNodeIteratorData){
					    .node_to_get_neighbours = node,
					    .current_neighbour	    = 0,
	};
	SGA_Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	SGA_LinksIterator neighbours_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = ChunkStream_NeighboursOfNode_next,
	    .destroy	   = ChunkStream_NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

typedef struct {
	size_t current_time;
	size_t current_id;
} CS_TimesIdPresentAtIteratorData;

SGA_Interval CS_TimesNodePresentAt_next(SGA_TimesIterator* iter) {
	CS_TimesIdPresentAtIteratorData* times_iter_data = (CS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream			 = (ChunkStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			 = chunk_stream->underlying_stream_graph;
	SGA_NodeId node					 = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	nth_time	      = SGA_Interval_intersection(nth_time, chunk_stream->timeframe);

	times_iter_data->current_time++;
	return nth_time;
}

void CS_TimesNodePresentAtIterator_destroy(SGA_TimesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_TimesIterator ChunkStream_times_node_present(SGA_StreamData* stream_data, SGA_NodeId node) {
	ChunkStream* chunk_stream		       = (ChunkStream*)stream_data;
	CS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CS_TimesIdPresentAtIteratorData));
	size_t nb_skips				       = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
	       chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end < chunk_stream->timeframe.start) {
		nb_skips++;
	}
	*iterator_data = (CS_TimesIdPresentAtIteratorData){
	    .current_time = nb_skips,
	    .current_id	  = node,
	};
	SGA_Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = CS_TimesNodePresentAt_next,
	    .destroy	   = CS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
SGA_Interval CS_TimesLinkPresentAt_next(SGA_TimesIterator* iter) {
	CS_TimesIdPresentAtIteratorData* times_iter_data = (CS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStream* chunk_stream			 = (ChunkStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			 = chunk_stream->underlying_stream_graph;
	SGA_LinkId link					 = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	nth_time	      = SGA_Interval_intersection(nth_time, chunk_stream->timeframe);
	times_iter_data->current_time++;
	return nth_time;
}

SGA_TimesIterator ChunkStream_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link) {
	ChunkStream* chunk_stream		       = (ChunkStream*)stream_data;
	CS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CS_TimesIdPresentAtIteratorData));
	size_t nb_skips				       = 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
	       chunk_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end < chunk_stream->timeframe.start) {
		nb_skips++;
	}
	*iterator_data = (CS_TimesIdPresentAtIteratorData){
	    .current_time = nb_skips,
	    .current_id	  = link,
	};
	SGA_Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = CS_TimesLinkPresentAt_next,
	    .destroy	   = CS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

SGA_Link ChunkStream_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	ChunkStream* chunk_stream = (ChunkStream*)stream_data;
	return chunk_stream->underlying_stream_graph->links.links[link_id];
}

typedef struct {
	SGA_NodesIterator nodes_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamNPATIterData;

SGA_NodeId ChunkStreamNPAT_next(SGA_NodesIterator* iter) {
	// call the next function of the underlying iterator
	ChunkStreamNPATIterData* iterator_data = (ChunkStreamNPATIterData*)iter->iterator_data;
	ChunkStream* chunk_stream	       = (ChunkStream*)iter->stream_graph.stream_data;
	SGA_NodeId node			       = iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
	// if the node is not present in the chunk stream, call the next function
	// again
	while (node != SIZE_MAX && BitArray_is_zero(chunk_stream->nodes_present, node)) {
		node = iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
	}
	return node;
}

void ChunkStreamNPAT_destroy(SGA_NodesIterator* iterator) {
	ChunkStreamNPATIterData* iterator_data = (ChunkStreamNPATIterData*)iterator->iterator_data;
	iterator_data->nodes_iterator_fsg.destroy(&iterator_data->nodes_iterator_fsg);
	free(iterator_data->underlying_stream_graph);
	free(iterator->iterator_data);
}

// TRICK : kind of weird hack but it works ig?
SGA_NodesIterator ChunkStream_nodes_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	ChunkStream* chunk_stream	       = (ChunkStream*)stream_data;
	ChunkStreamNPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamNPATIterData));
	/*FullStreamGraph* full_stream_graph = MALLOC(sizeof(FullStreamGraph));
	 *full_stream_graph =
	 *FullStreamGraph_from(chunk_stream->underlying_stream_graph);*/
	SGA_Stream fsg			       = SGA_FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* full_stream_graph     = (FullStreamGraph*)fsg.stream_data;
	iterator_data->nodes_iterator_fsg      = FullStreamGraph_stream_functions.nodes_present_at_t(full_stream_graph, instant);
	iterator_data->underlying_stream_graph = full_stream_graph;

	SGA_Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	SGA_NodesIterator nodes_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamNPAT_next,
	    .destroy	   = ChunkStreamNPAT_destroy,
	};
	return nodes_iterator;
}

typedef struct {
	SGA_LinksIterator links_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamLPATIterData;

SGA_LinkId ChunkStreamLPAT_next(SGA_LinksIterator* iter) {
	// call the next function of the underlying iterator
	ChunkStreamLPATIterData* iterator_data = (ChunkStreamLPATIterData*)iter->iterator_data;
	ChunkStream* chunk_stream	       = (ChunkStream*)iter->stream_graph.stream_data;
	SGA_LinkId link			       = iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
	// if the link is not present in the chunk stream, call the next function
	// again
	while (link != SIZE_MAX && BitArray_is_zero(chunk_stream->links_present, link)) {
		link = iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
	}
	return link;
}

void ChunkStreamLPAT_destroy(SGA_LinksIterator* iterator) {
	ChunkStreamLPATIterData* iterator_data = (ChunkStreamLPATIterData*)iterator->iterator_data;
	iterator_data->links_iterator_fsg.destroy(&iterator_data->links_iterator_fsg);
	free(iterator_data->underlying_stream_graph);
	free(iterator->iterator_data);
}

SGA_LinksIterator ChunkStream_links_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	ChunkStream* chunk_stream	       = (ChunkStream*)stream_data;
	ChunkStreamLPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamLPATIterData));
	SGA_Stream fsg			       = SGA_FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* full_stream_graph     = (FullStreamGraph*)fsg.stream_data;
	iterator_data->links_iterator_fsg      = FullStreamGraph_stream_functions.links_present_at_t(full_stream_graph, instant);
	iterator_data->underlying_stream_graph = full_stream_graph;
	SGA_Stream stream		       = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM;
	// stream->stream = chunk_stream;
	SGA_LinksIterator links_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamLPAT_next,
	    .destroy	   = ChunkStreamLPAT_destroy,
	};
	return links_iterator;
}

SGA_TimesIterator ChunkStream_key_instants(SGA_StreamData* stream_data) {
	ChunkStream* chunk_stream     = (ChunkStream*)stream_data;
	SGA_StreamGraph* stream_graph = chunk_stream->underlying_stream_graph;
	return SGA_StreamGraph_key_instants_between(stream_graph, chunk_stream->timeframe);
}

const StreamFunctions ChunkStream_stream_functions = {
    .nodes_set		= ChunkStream_nodes_set,
    .links_set		= ChunkStream_links_set,
    .lifespan		= ChunkStream_lifespan,
    .time_scale		= ChunkStream_time_scale,
    .nodes_present_at_t = ChunkStream_nodes_present_at_t,
    .links_present_at_t = ChunkStream_links_present_at_t,
    .times_node_present = ChunkStream_times_node_present,
    .times_link_present = ChunkStream_times_link_present,
    .link_by_id		= ChunkStream_link_by_id,
    .neighbours_of_node = ChunkStream_neighbours_of_node,
    .key_instants	= ChunkStream_key_instants,
};

size_t ChunkStream_cardinal_of_v(SGA_Stream* stream) {
	SGA_StreamData* stream_data = stream->stream_data;
	ChunkStream* chunk_stream   = (ChunkStream*)stream_data;
	return BitArray_count_ones(&chunk_stream->nodes_present);
}

size_t ChunkStream_cardinal_of_t(SGA_Stream* stream) {
	SGA_StreamData* stream_data = stream->stream_data;
	ChunkStream* chunk_stream   = (ChunkStream*)stream_data;
	return SGA_Interval_duration(chunk_stream->timeframe);
}

size_t ChunkStream_cardinal_distinct_links(SGA_Stream* stream) {

	SGA_StreamData* stream_data = stream->stream_data;
	ChunkStream* chunk_stream   = (ChunkStream*)stream_data;
	return BitArray_count_ones(&chunk_stream->links_present);
}

const MetricsFunctions ChunkStream_metrics_functions = {
    .temporal_cardinal_of_node_set = NULL,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = ChunkStream_cardinal_of_v,
    .distinct_cardinal_of_link_set = ChunkStream_cardinal_distinct_links,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};
