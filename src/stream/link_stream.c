#include "link_stream.h"
#include "../interval.h"
#include "../metrics.h"
#include "../utils.h"
#include "full_stream_graph.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

LinkStream LinkStream_from(StreamGraph* stream_graph) {
	LinkStream link_stream = (LinkStream){
		.underlying_stream_graph = stream_graph,
	};
	return link_stream;
}

// TODO : stream ls from

/*size_t LS_cardinal_of_V(LinkStream* ls) {
	printf("called LS_cardinal_of_V\n");
	return ls->underlying_stream_graph->nodes.nb_nodes;
}

size_t LS_cardinal_of_T(LinkStream* ls) {
	return Interval_size(Interval_from(StreamGraph_lifespan_begin(ls->underlying_stream_graph),
									   StreamGraph_lifespan_end(ls->underlying_stream_graph)));
}

size_t LS_cardinal_of_W(LinkStream* ls) {
	return LS_cardinal_of_V(ls) * LS_cardinal_of_T(ls);
}

double LS_coverage(LinkStream* ls) {
	return 1.0;
}

size_t LS_scaling(LinkStream* ls) {
	return ls->underlying_stream_graph->scaling;
}

BaseGenericFunctions link_stream_base_functions = {
	.cardinalOfT = (size_t(*)(void*))LS_cardinal_of_T,
	.cardinalOfV = (size_t(*)(void*))LS_cardinal_of_V,
	.cardinalOfW = (size_t(*)(void*))LS_cardinal_of_W,
	.scaling = (size_t(*)(void*))LS_scaling,
};

HijackedGenericFunctions link_stream_hijacked_functions = {
	.coverage = (double (*)(void*))LS_coverage,
};*/

/*NodesIterator (*nodes_set)(void*);
LinksIterator (*links_set)(void*);
Interval (*lifespan)(void*);
size_t (*scaling)(void*);

NodesIterator (*nodes_present_at_t)(void*, TimeId);
LinksIterator (*links_present_at_t)(void*, TimeId);

TimesIterator (*times_node_present)(void*, NodeId);
TimesIterator (*times_link_present)(void*, LinkId);*/

typedef struct {
	NodeId current_node;
	FullStreamGraph* fsg;
} LinkStreamNodesSetIteratorData;

void LinkStream_iter_destroy(NodesIterator* iterator) {
	LinkStreamNodesSetIteratorData* iterator_data = (LinkStreamNodesSetIteratorData*)iterator->iterator_data;
	free(iterator_data->fsg);
	free(iterator->iterator_data);
}

// TRICK : kind of weird hack but it works ig?
NodesIterator LinkStream_nodes_set(StreamData* stream_data) {
	LinkStream* link_stream					   = (LinkStream*)stream_data;
	FullStreamGraph* full_stream_graph		   = MALLOC(sizeof(FullStreamGraph));
	full_stream_graph->underlying_stream_graph = link_stream->underlying_stream_graph;
	NodesIterator n							   = FullStreamGraph_stream_functions.nodes_set(full_stream_graph);
	free(n.iterator_data);
	n.iterator_data								  = MALLOC(sizeof(LinkStreamNodesSetIteratorData));
	LinkStreamNodesSetIteratorData* iterator_data = (LinkStreamNodesSetIteratorData*)n.iterator_data;
	iterator_data->fsg							  = full_stream_graph;
	iterator_data->current_node					  = 0;
	n.stream_graph.type							  = LINK_STREAM;
	n.destroy									  = LinkStream_iter_destroy;
	return n;
}

LinksIterator LinkStream_links_set(StreamData* stream_data) {
	LinkStream* link_stream	 = (LinkStream*)stream_data;
	Stream full_stream_graph = FullStreamGraph_from(link_stream->underlying_stream_graph);
	FullStreamGraph* fsg	 = (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.links_set(fsg);
}

Interval LinkStream_lifespan(StreamData* stream_data) {
	LinkStream* link_stream = (LinkStream*)stream_data;
	return Interval_from(StreamGraph_lifespan_begin(link_stream->underlying_stream_graph),
						 StreamGraph_lifespan_end(link_stream->underlying_stream_graph));
}

size_t LinkStream_scaling(StreamData* stream_data) {
	LinkStream* link_stream = (LinkStream*)stream_data;
	return link_stream->underlying_stream_graph->scaling;
}

NodesIterator LinkStream_nodes_present_at_t(StreamData* stream_data, TimeId instant) {
	LinkStream* link_stream	 = (LinkStream*)stream_data;
	Stream full_stream_graph = FullStreamGraph_from(link_stream->underlying_stream_graph);
	FullStreamGraph* fsg	 = (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.nodes_set(fsg);
}

LinksIterator LinkStream_links_present_at_t(StreamData* stream_data, TimeId instant) {
	LinkStream* link_stream	 = (LinkStream*)stream_data;
	Stream full_stream_graph = FullStreamGraph_from(link_stream->underlying_stream_graph);
	FullStreamGraph* fsg	 = (FullStreamGraph*)full_stream_graph.stream_data;
	return FullStreamGraph_stream_functions.links_present_at_t(fsg, instant);
}

// time of nodes iterator
typedef struct {
	bool has_been_called;
} TimesNodePresentIteratorData;

Interval LinkStream_TimesNodePresent_next(TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = (TimesNodePresentIteratorData*)iter->iterator_data;
	StreamGraph* stream_graph					  = iter->stream_graph.stream_data;
	if (times_iter_data->has_been_called) {
		return (Interval){.start = SIZE_MAX};
	}
	times_iter_data->has_been_called = true;
	return Interval_from(StreamGraph_lifespan_begin(stream_graph), StreamGraph_lifespan_end(stream_graph));
}

void LinkStream_TimesNodePresentIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator LinkStream_times_node_present(StreamData* stream_data, NodeId node_id) {
	LinkStream* link_stream						= (LinkStream*)stream_data;
	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = link_stream->underlying_stream_graph};
	// Stream* stream = MALLOC(sizeof(Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = link_stream->underlying_stream_graph;
	iterator_data->has_been_called = false;
	TimesIterator times_iterator   = {
		  .stream_graph	 = stream,
		  .iterator_data = iterator_data,
		  .next			 = LinkStream_TimesNodePresent_next,
		  .destroy		 = LinkStream_TimesNodePresentIterator_destroy,
	  };
	return times_iterator;
}

// time of links iterator
TimesIterator LinkStream_times_link_present(StreamData* stream_data, LinkId link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;
	return FullStreamGraph_stream_functions.times_link_present(link_stream->underlying_stream_graph, link_id);
}

Stream LS_from(StreamGraph* stream_graph) {
	LinkStream* link_stream				 = MALLOC(sizeof(LinkStream));
	link_stream->underlying_stream_graph = stream_graph;
	Stream stream						 = {.type = LINK_STREAM, .stream_data = link_stream};
	init_cache(&stream);
	return stream;
}

void LS_destroy(Stream stream) {
	free(stream.stream_data);
}

Link LinkStream_link_by_id(StreamData* stream_data, LinkId link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;
	return link_stream->underlying_stream_graph->links.links[link_id];
}

LinksIterator LinkStream_neighbours_of_node(StreamData* stream_data, NodeId node_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;
	Stream st				= FullStreamGraph_from(link_stream->underlying_stream_graph);
	FullStreamGraph* fsg	= (FullStreamGraph*)st.stream_data;
	return FullStreamGraph_stream_functions.neighbours_of_node(fsg, node_id);
}

TemporalNode LinkStream_node_by_id(StreamData* stream_data, NodeId node_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;
	return link_stream->underlying_stream_graph->nodes.nodes[node_id];
}

LinkId LinkStream_links_between_nodes(StreamData* stream_data, NodeId node_id, NodeId other_node_id) {
	TemporalNode n1			 = LinkStream_node_by_id(stream_data, node_id);
	TemporalNode n2			 = LinkStream_node_by_id(stream_data, other_node_id);
	TemporalNode to_research = n1.nb_neighbours < n2.nb_neighbours ? n1 : n2;
	// Since the nodes are sorted, we can use a binary search
	size_t left	 = 0;
	size_t right = to_research.nb_neighbours;
	while (left < right) {
		size_t mid		 = left + (right - left) / 2;
		NodeId neighbour = to_research.neighbours[mid];
		if (neighbour == other_node_id) {
			return to_research.neighbours[mid];
		}
		else if (neighbour < other_node_id) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}
	return SIZE_MAX;
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
	size_t t	   = Interval_size(Interval_from(StreamGraph_lifespan_begin(ls->underlying_stream_graph),
												 StreamGraph_lifespan_end(ls->underlying_stream_graph)));
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
	size_t v = cardinalOfT(stream);
	size_t t = cardinalOfW(stream);
	size_t w = v * t;
	UPDATE_CACHE(stream, cardinalOfW, w);
	return w;
}

double density(Stream* stream) {
	size_t n = LS_cardinal_of_V(stream);
	double m = Stream_number_of_links(stream);
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