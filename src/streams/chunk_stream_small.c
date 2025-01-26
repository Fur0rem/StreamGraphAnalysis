#define SGA_INTERNAL

#include "chunk_stream_small.h"
#include "../interval.h"
#include "../iterators.h"
#include "../stream_data_access/key_moments.h"
#include "full_stream_graph.h"

#include <stddef.h>

SGA_Stream SGA_ChunkStreamSmall_from(SGA_StreamGraph* stream_graph, SGA_NodeIdArrayList nodes, SGA_LinkIdArrayList links,
				     SGA_Interval snapshot) {
	ChunkStreamSmall* chunk_stream = MALLOC(sizeof(ChunkStreamSmall));

	// Initialise the ChunkStreamSmall
	*chunk_stream = (ChunkStreamSmall){
	    .nb_nodes		     = nodes.length,
	    .nodes_present	     = nodes.array,
	    .nb_links		     = links.length,
	    .links_present	     = links.array,
	    .underlying_stream_graph = stream_graph,
	    .snapshot		     = snapshot,
	};

	// Remove links whose nodes are not present
	for (size_t i = 0; i < chunk_stream->nb_links; i++) {
		SGA_Link link	 = stream_graph->links.links[links.array[i]];
		SGA_NodeId node1 = link.nodes[0];
		SGA_NodeId node2 = link.nodes[1];
		if (!SGA_NodeIdArrayList_contains(nodes, node1) || !SGA_NodeIdArrayList_contains(nodes, node2)) {
			SGA_LinkIdArrayList_remove_and_swap(&links, i);
			chunk_stream->nb_links--;
			i--;
		}
	}

	return (SGA_Stream){
	    .type	 = CHUNK_STREAM_SMALL,
	    .stream_data = chunk_stream,
	};
}

void SGA_ChunkStreamSmall_destroy(SGA_Stream stream) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream.stream_data;
	free(chunk_stream->nodes_present);
	free(chunk_stream->links_present);
	free(chunk_stream);
}

/**
 * @brief Check if a node is present in the chunk.
 * @param[in] node The node to check.
 * @param[in] chunk_stream The ChunkStreamSmall to check in.
 * @return true if the node is present, false otherwise.
 */
// OPTIMISE : Maybe sort the nodes_present and links_present arrays before if we do a lot of lookups
bool is_node_present(SGA_NodeId node, ChunkStreamSmall* chunk_stream) {
	for (size_t i = 0; i < chunk_stream->nb_nodes; i++) {
		if (chunk_stream->nodes_present[i] == node) {
			return true;
		}
	}
	return false;
}

/**
 * @brief Check if a link is present in the chunk.
 * @param[in] link The link to check.
 * @param[in] chunk_stream The ChunkStreamSmall to check in.
 * @return true if the link is present, false otherwise.
 */
// OPTIMISE : Same as is_node_present
bool is_link_present(SGA_LinkId link, ChunkStreamSmall* chunk_stream) {
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

SGA_NodeId ChunkStreamSmallNodesSetIterator_next(SGA_NodesIterator* it) {
	ChunkStreamSmallNodesSetIteratorData* iterator_data = it->iterator_data;
	ChunkStreamSmall* chunk_stream			    = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (iterator_data->current_node >= chunk_stream->nb_nodes) {
		return SGA_NODES_ITERATOR_END;
	}
	SGA_NodeId current_node = chunk_stream->nodes_present[iterator_data->current_node];
	iterator_data->current_node++;
	return current_node;
}

void ChunkStreamSmallNodesSetIterator_destroy(SGA_NodesIterator* it) {
	free(it->iterator_data);
}

// OPTIMISE : use iterator_data directly as a size_t to have one less malloc and indirection
SGA_NodesIterator ChunkStreamSmall_nodes_set(SGA_StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream			    = (ChunkStreamSmall*)stream_data;
	ChunkStreamSmallNodesSetIteratorData* iterator_data = MALLOC(sizeof(ChunkStreamSmallNodesSetIteratorData));
	iterator_data->current_node			    = 0;
	SGA_Stream stream				    = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	return (SGA_NodesIterator){
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamSmallNodesSetIterator_next,
	    .destroy	   = ChunkStreamSmallNodesSetIterator_destroy,
	    .stream_graph  = stream,
	};
}

typedef struct {
	size_t current_link;
} ChunkStreamSmallLinksSetIteratorData;

SGA_LinkId ChunkStreamSmallLinksSetIterator_next(SGA_LinksIterator* it) {
	ChunkStreamSmallLinksSetIteratorData* iterator_data = it->iterator_data;
	ChunkStreamSmall* chunk_stream			    = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (iterator_data->current_link >= chunk_stream->nb_links) {
		return SGA_LINKS_ITERATOR_END;
	}
	SGA_LinkId current_link = chunk_stream->links_present[iterator_data->current_link];
	iterator_data->current_link++;
	return current_link;
}

void ChunkStreamSmallLinksSetIterator_destroy(SGA_LinksIterator* it) {
	free(it->iterator_data);
}

SGA_LinksIterator ChunkStreamSmall_links_set(SGA_StreamData* chunk_stream) {
	ChunkStreamSmallLinksSetIteratorData* iterator_data = MALLOC(sizeof(ChunkStreamSmallLinksSetIteratorData));
	iterator_data->current_link			    = 0;
	SGA_Stream stream				    = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	return (SGA_LinksIterator){
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamSmallLinksSetIterator_next,
	    .destroy	   = ChunkStreamSmallLinksSetIterator_destroy,
	    .stream_graph  = stream,
	};
}

SGA_Interval ChunkStreamSmall_lifespan(SGA_StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	return chunk_stream->snapshot;
}

size_t ChunkStreamSmall_time_scale(SGA_StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	return chunk_stream->underlying_stream_graph->time_scale;
}

typedef struct {
	SGA_NodesIterator nodes_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamSmallNPATIterData;

SGA_NodeId ChunkStreamSmallNodesPresentAtTIterator_next(SGA_NodesIterator* it) {
	ChunkStreamSmallNPATIterData* iterator_data = it->iterator_data;
	SGA_NodeId node_id			    = iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
	if (node_id == SIZE_MAX) {
		return SGA_NODES_ITERATOR_END;
	}
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (is_node_present(node_id, chunk_stream)) {
		return node_id;
	}
	return iterator_data->nodes_iterator_fsg.next(&iterator_data->nodes_iterator_fsg);
}

void ChunkStreamSmallNodesPresentAtTIterator_destroy(SGA_NodesIterator* it) {
	ChunkStreamSmallNPATIterData* iterator_data = it->iterator_data;
	iterator_data->nodes_iterator_fsg.destroy(&iterator_data->nodes_iterator_fsg);
	free(iterator_data);
}

SGA_NodesIterator ChunkStreamSmall_nodes_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	ChunkStreamSmall* chunk_stream		    = (ChunkStreamSmall*)stream_data;
	SGA_Stream st				    = SGA_FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* fsg			    = (FullStreamGraph*)st.stream_data;
	SGA_NodesIterator nodes_iterator_fsg	    = FullStreamGraph_stream_functions.nodes_present_at_t(fsg, instant);
	ChunkStreamSmallNPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamSmallNPATIterData));
	iterator_data->nodes_iterator_fsg	    = nodes_iterator_fsg;
	iterator_data->underlying_stream_graph	    = fsg;

	SGA_Stream stream = {
	    .type	 = CHUNK_STREAM_SMALL,
	    .stream_data = chunk_stream,
	};

	return (SGA_NodesIterator){
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamSmallNodesPresentAtTIterator_next,
	    .destroy	   = ChunkStreamSmallNodesPresentAtTIterator_destroy,
	    .stream_graph  = stream,
	};
}

typedef struct {
	SGA_LinksIterator links_iterator_fsg;
	FullStreamGraph* underlying_stream_graph;
} ChunkStreamSmallLPATIterData;

SGA_LinkId ChunkStreamSmallLinksPresentAtTIterator_next(SGA_LinksIterator* it) {
	ChunkStreamSmallLPATIterData* iterator_data = it->iterator_data;
	SGA_LinkId link_id			    = iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
	if (link_id == SIZE_MAX) {
		return SGA_LINKS_ITERATOR_END;
	}
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)it->stream_graph.stream_data;
	if (is_link_present(link_id, chunk_stream)) {
		return link_id;
	}

	return iterator_data->links_iterator_fsg.next(&iterator_data->links_iterator_fsg);
}

void ChunkStreamSmallLinksPresentAtTIterator_destroy(SGA_LinksIterator* it) {
	ChunkStreamSmallLPATIterData* iterator_data = it->iterator_data;
	iterator_data->links_iterator_fsg.destroy(&iterator_data->links_iterator_fsg);
	free(iterator_data);
}

SGA_LinksIterator ChunkStreamSmall_links_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	ChunkStreamSmall* chunk_stream		    = (ChunkStreamSmall*)stream_data;
	SGA_Stream st				    = SGA_FullStreamGraph_from(chunk_stream->underlying_stream_graph);
	FullStreamGraph* fsg			    = (FullStreamGraph*)st.stream_data;
	SGA_LinksIterator links_iterator_fsg	    = FullStreamGraph_stream_functions.links_present_at_t(fsg, instant);
	ChunkStreamSmallLPATIterData* iterator_data = MALLOC(sizeof(ChunkStreamSmallLPATIterData));
	iterator_data->links_iterator_fsg	    = links_iterator_fsg;
	iterator_data->underlying_stream_graph	    = fsg;
	SGA_Stream stream			    = {
				      .type	   = CHUNK_STREAM_SMALL,
				      .stream_data = chunk_stream,
	  };
	return (SGA_LinksIterator){
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamSmallLinksPresentAtTIterator_next,
	    .destroy	   = ChunkStreamSmallLinksPresentAtTIterator_destroy,
	    .stream_graph  = stream,
	};
}

typedef struct {
	SGA_NodeId node_to_get_neighbours;
	SGA_NodeId current_neighbour;
} CSS_NeighboursOfNodeIteratorData;

SGA_LinkId ChunkStreamSmall_NeighboursOfNode_next(SGA_LinksIterator* iter) {
	CSS_NeighboursOfNodeIteratorData* neighbours_iter_data = (CSS_NeighboursOfNodeIteratorData*)iter->iterator_data;
	ChunkStreamSmall* chunk_stream			       = (ChunkStreamSmall*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			       = chunk_stream->underlying_stream_graph;
	SGA_NodeId node					       = neighbours_iter_data->node_to_get_neighbours;
	// Print all the neighbours of the node
	if (neighbours_iter_data->current_neighbour >= stream_graph->nodes.nodes[node].nb_neighbours) {
		return SGA_LINKS_ITERATOR_END;
	}
	size_t return_val = chunk_stream->underlying_stream_graph->nodes.nodes[node].neighbours[neighbours_iter_data->current_neighbour];
	neighbours_iter_data->current_neighbour++;
	/*if (BitArray_is_zero(chunk_stream->links_present, return_val)) {
		return ChunkStreamSmall_NeighboursOfNode_next(iter);
	}*/
	if (!is_link_present(return_val, chunk_stream)) {
		return ChunkStreamSmall_NeighboursOfNode_next(iter);
	}
	return return_val;
}

void ChunkStreamSmall_NeighboursOfNodeIterator_destroy(SGA_LinksIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_LinksIterator ChunkStreamSmall_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node) {
	ChunkStreamSmall* chunk_stream			= (ChunkStreamSmall*)stream_data;
	CSS_NeighboursOfNodeIteratorData* iterator_data = MALLOC(sizeof(CSS_NeighboursOfNodeIteratorData));
	*iterator_data					= (CSS_NeighboursOfNodeIteratorData){
					     .node_to_get_neighbours = node,
					     .current_neighbour	     = 0,
	 };
	// SGA_Stream stream = {.type = CHUNK_STREAM_SMALL, .stream = chunk_stream};
	SGA_Stream stream		      = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	SGA_LinksIterator neighbours_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = ChunkStreamSmall_NeighboursOfNode_next,
	    .destroy	   = ChunkStreamSmall_NeighboursOfNodeIterator_destroy,
	};
	return neighbours_iterator;
}

typedef struct {
	size_t current_time;
	size_t current_id;
} CSS_TimesIdPresentAtIteratorData;

SGA_Interval CSS_TimesNodePresentAt_next(SGA_TimesIterator* iter) {
	CSS_TimesIdPresentAtIteratorData* times_iter_data = (CSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStreamSmall* chunk_stream			  = (ChunkStreamSmall*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			  = chunk_stream->underlying_stream_graph;
	SGA_NodeId node					  = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	nth_time	      = SGA_Interval_intersection(nth_time, chunk_stream->snapshot);

	times_iter_data->current_time++;
	return nth_time;
}

void CSS_TimesNodePresentAtIterator_destroy(SGA_TimesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_TimesIterator ChunkStreamSmall_times_node_present(SGA_StreamData* stream_data, SGA_NodeId node) {
	ChunkStreamSmall* chunk_stream			= (ChunkStreamSmall*)stream_data;
	CSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CSS_TimesIdPresentAtIteratorData));
	size_t nb_skips					= 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
	       chunk_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end < chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CSS_TimesIdPresentAtIteratorData){
	    .current_time = nb_skips,
	    .current_id	  = node,
	};
	SGA_Stream stream = {.type = CHUNK_STREAM_SMALL, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM_SMALL;
	// stream->stream = chunk_stream;
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = CSS_TimesNodePresentAt_next,
	    .destroy	   = CSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
SGA_Interval CSS_TimesLinkPresentAt_next(SGA_TimesIterator* iter) {
	CSS_TimesIdPresentAtIteratorData* times_iter_data = (CSS_TimesIdPresentAtIteratorData*)iter->iterator_data;
	ChunkStreamSmall* chunk_stream			  = (ChunkStreamSmall*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			  = chunk_stream->underlying_stream_graph;
	SGA_LinkId link					  = times_iter_data->current_id;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	nth_time	      = SGA_Interval_intersection(nth_time, chunk_stream->snapshot);

	times_iter_data->current_time++;
	return nth_time;
}

SGA_TimesIterator ChunkStreamSmall_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link) {
	ChunkStreamSmall* chunk_stream			= (ChunkStreamSmall*)stream_data;
	CSS_TimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(CSS_TimesIdPresentAtIteratorData));
	size_t nb_skips					= 0;
	while (nb_skips < chunk_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
	       chunk_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end < chunk_stream->snapshot.start) {
		nb_skips++;
	}
	*iterator_data = (CSS_TimesIdPresentAtIteratorData){
	    .current_time = nb_skips,
	    .current_id	  = link,
	};
	SGA_Stream stream = {.type = CHUNK_STREAM, .stream_data = chunk_stream};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = CHUNK_STREAM_SMALL;
	// stream->stream = chunk_stream;
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = CSS_TimesLinkPresentAt_next,
	    .destroy	   = CSS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

SGA_Link ChunkStreamSmall_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	return chunk_stream->underlying_stream_graph->links.links[link_id];
}

// OPTIMISE: why lookup through every link
SGA_LinkId ChunkStreamSmall_link_between_nodes(SGA_StreamData* stream_data, SGA_NodeId node1, SGA_NodeId node2) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	SGA_StreamGraph* stream_graph  = chunk_stream->underlying_stream_graph;
	for (size_t i = 0; i < stream_graph->links.nb_links; i++) {
		SGA_Link link = stream_graph->links.links[i];
		if ((link.nodes[0] == node1 && link.nodes[1] == node2) || (link.nodes[0] == node2 && link.nodes[1] == node1)) {
			if (is_link_present(i, chunk_stream)) {
				return i;
			}
		}
	}
	return SIZE_MAX;
}

SGA_TimesIterator ChunkStreamSmall_key_moments(SGA_StreamData* stream_data) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream_data;
	SGA_StreamGraph* stream_graph  = chunk_stream->underlying_stream_graph;
	return SGA_StreamGraph_key_moments_between(stream_graph, chunk_stream->snapshot);
}

const StreamFunctions ChunkStreamSmall_stream_functions = {
    .nodes_set		= ChunkStreamSmall_nodes_set,
    .links_set		= ChunkStreamSmall_links_set,
    .lifespan		= ChunkStreamSmall_lifespan,
    .time_scale		= ChunkStreamSmall_time_scale,
    .nodes_present_at_t = ChunkStreamSmall_nodes_present_at_t,
    .links_present_at_t = ChunkStreamSmall_links_present_at_t,
    .times_node_present = ChunkStreamSmall_times_node_present,
    .times_link_present = ChunkStreamSmall_times_link_present,
    .link_by_id		= ChunkStreamSmall_link_by_id,
    .neighbours_of_node = ChunkStreamSmall_neighbours_of_node,
    .link_between_nodes = NULL,
    .key_moments	= ChunkStreamSmall_key_moments,
};

size_t ChunkStreamSmall_cardinal_of_v(SGA_Stream* stream) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream->stream_data;
	return chunk_stream->nb_nodes;
}

size_t ChunkStreamSmall_cardinal_of_t(SGA_Stream* stream) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream->stream_data;
	return SGA_Interval_duration(chunk_stream->snapshot);
}

size_t ChunkStreamSmall_cardinal_distinct_links(SGA_Stream* stream) {
	ChunkStreamSmall* chunk_stream = (ChunkStreamSmall*)stream->stream_data;
	return chunk_stream->nb_links;
}

const MetricsFunctions ChunkStreamSmall_metrics_functions = {
    .temporal_cardinal_of_node_set = NULL,
    .duration			   = ChunkStreamSmall_cardinal_of_t,
    .distinct_cardinal_of_node_set = ChunkStreamSmall_cardinal_of_v,
    .distinct_cardinal_of_link_set = ChunkStreamSmall_cardinal_distinct_links,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};