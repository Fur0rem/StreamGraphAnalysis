#include "link_access.h"
#include "../stream_graph/links_set.h"
#include "node_access.h"

typedef struct {
	LinkId current_link;
} LinksSetIteratorData;

size_t LinksSet_next(LinksIterator* iter) {
	LinksSetIteratorData* links_iter_data = (LinksSetIteratorData*)iter->iterator_data;
	StreamGraph* stream_graph			  = iter->stream_graph.stream_data;

	if (links_iter_data->current_link >= stream_graph->links.nb_links) {
		return LINKS_ITERATOR_END;
	}

	size_t return_val = links_iter_data->current_link;
	links_iter_data->current_link++;
	return return_val;
}

void LinksSetIterator_destroy(LinksIterator* iterator) {
	free(iterator->iterator_data);
}

LinksIterator StreamGraph_links_set(StreamGraph* stream_graph) {
	LinksSetIteratorData* iterator_data = MALLOC(sizeof(LinksSetIteratorData));
	iterator_data->current_link			= 0;
	Stream stream						= {.stream_data = stream_graph};
	LinksIterator links_iterator		= {
			   .stream_graph  = stream,
			   .iterator_data = iterator_data,
			   .next		  = LinksSet_next,
			   .destroy		  = LinksSetIterator_destroy,
	   };
	return links_iterator;
}

typedef struct {
	TimeId current_interval;
	Link* link;
} TimesLinkPresentIteratorData;

Interval TimesLinkPresent_next(TimesIterator* iter) {
	TimesLinkPresentIteratorData* times_iter_data = (TimesLinkPresentIteratorData*)iter->iterator_data;
	Link* link									  = times_iter_data->link;

	if (times_iter_data->current_interval >= link->presence.nb_intervals) {
		return (Interval){.start = SIZE_MAX, .end = SIZE_MAX};
	}

	Interval return_val = link->presence.intervals[times_iter_data->current_interval];
	times_iter_data->current_interval++;
	return return_val;
}

void TimesLinkPresentIterator_destroy(TimesIterator* iterator) {
	free(iterator->iterator_data);
}

TimesIterator StreamGraph_times_link_present(StreamGraph* stream_graph, LinkId link_id) {
	DEV_ASSERT(link_id < stream_graph->links.nb_links);

	TimesLinkPresentIteratorData* iterator_data = MALLOC(sizeof(TimesLinkPresentIteratorData));
	iterator_data->current_interval				= 0;
	iterator_data->link							= &stream_graph->links.links[link_id];
	Stream stream								= {.stream_data = stream_graph};
	TimesIterator times_iterator				= {
					   .stream_graph  = stream,
					   .iterator_data = iterator_data,
					   .next		  = TimesLinkPresent_next,
					   .destroy		  = TimesLinkPresentIterator_destroy,
	   };
	return times_iterator;
}

Link StreamGraph_link_by_id(StreamGraph* stream_graph, LinkId link_id) {
	ASSERT(link_id < stream_graph->links.nb_links);
	return stream_graph->links.links[link_id];
}

LinkId StreamGraph_link_between_nodes(StreamGraph* stream_graph, NodeId node_id, NodeId other_node_id) {
	ASSERT(node_id != other_node_id);
	TemporalNode n1	   = StreamGraph_node_by_id(stream_graph, node_id);
	TemporalNode n2	   = StreamGraph_node_by_id(stream_graph, other_node_id);
	NodeId to_look_for = other_node_id;
	// OPTIMISATION: we iterate over the smallest neighbour list
	if (n1.nb_neighbours > n2.nb_neighbours) {
		TemporalNode tmp = n1;
		n1				 = n2;
		n2				 = tmp;
		to_look_for		 = node_id;
	}

	for (size_t i = 0; i < n1.nb_neighbours; i++) {
		Link link = StreamGraph_link_by_id(stream_graph, n1.neighbours[i]);
		if (link.nodes[0] == to_look_for || link.nodes[1] == to_look_for) {
			return n1.neighbours[i];
		}
	}

	// No link found
	return SIZE_MAX;
}
