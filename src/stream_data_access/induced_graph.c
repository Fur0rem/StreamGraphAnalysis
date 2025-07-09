#define SGA_INTERNAL

#include "../utils.h"
#include "key_instants.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The data of the iterator over the nodes present at a given time in a StreamGraph.
 */
typedef struct {
	size_t current_event; ///< The current event index in the events table
	size_t current_node;  ///< The current node index in the current event
} NodesPresentAtTIterator;

// Returns NULL if there are no more nodes present at time t
size_t NodesPresentAtT_next(SGA_NodesIterator* nodes_iter) {
	NodesPresentAtTIterator* nodes_iter_data = (NodesPresentAtTIterator*)nodes_iter->iterator_data;
	SGA_StreamGraph* stream_graph		 = nodes_iter->stream_graph.stream_data;

	// If you're the last node of the event
	size_t return_val;
	if (nodes_iter_data->current_node == stream_graph->events.node_events.events[nodes_iter_data->current_event].nb_info) {
		// If you're the first event
		if (nodes_iter_data->current_event == 0) {
			return SIZE_MAX;
		}

		// If your event has deletion (presence mask bit 0)
		if (!BitArray_is_one(stream_graph->events.node_events.presence_mask, nodes_iter_data->current_event)) {
			return SIZE_MAX;
		}
		nodes_iter_data->current_event--;
		nodes_iter_data->current_node = 0;
	}

	// If the current event has 0 nodes
	while (stream_graph->events.node_events.events[nodes_iter_data->current_event].nb_info == 0) {
		if ((nodes_iter_data->current_event == 0) ||
		    (!BitArray_is_one(stream_graph->events.node_events.presence_mask, nodes_iter_data->current_event))) {
			return SIZE_MAX;
		}
		nodes_iter_data->current_event--;
		nodes_iter_data->current_node = 0;
	}
	return_val = stream_graph->events.node_events.events[nodes_iter_data->current_event].events[nodes_iter_data->current_node];
	nodes_iter_data->current_node++;

	return return_val;
}

void NodesPresentAtTIterator_destroy(SGA_NodesIterator* nodes_iter) {
	free(nodes_iter->iterator_data);
}

SGA_NodesIterator SGA_StreamGraph_nodes_present_at(SGA_StreamGraph* stream_graph, SGA_Time time) {
	if (!SGA_Interval_contains(stream_graph->lifespan, time)) {
		return SGA_NodesIterator_empty();
	}

	size_t current_event = KeyInstantsTable_find_time_index_equivalent(&stream_graph->key_instants, time);

	SGA_Stream stream = {
	    .type	 = FULL_STREAM_GRAPH,
	    .stream_data = stream_graph,
	};
	NodesPresentAtTIterator* nodes_iter_data = MALLOC(sizeof(NodesPresentAtTIterator));
	nodes_iter_data->current_event		 = current_event;
	nodes_iter_data->current_node		 = 0;
	SGA_NodesIterator nodes_iter		 = {
			.stream_graph  = stream,
			.iterator_data = nodes_iter_data,
			.next	       = NodesPresentAtT_next,
			.destroy       = NodesPresentAtTIterator_destroy,
	    };

	return nodes_iter;
}

/**
 * @brief The data of the iterator over the links present at a given time in a StreamGraph.
 */
typedef struct {
	size_t current_event; ///< The current event index in the events table
	size_t current_link;  ///< The current link index in the current event
} LinksPresentAtTIterator;

// Returns NULL if there are no more links present at time t
size_t LinksPresentAtT_next(SGA_LinksIterator* links_iter) {

	LinksPresentAtTIterator* links_iter_data = (LinksPresentAtTIterator*)links_iter->iterator_data;
	SGA_StreamGraph* stream_graph		 = links_iter->stream_graph.stream_data;

	// If you're the last link of the event
	size_t return_val;
	if (links_iter_data->current_link == stream_graph->events.link_events.events[links_iter_data->current_event].nb_info) {
		// If you're the first event
		if (links_iter_data->current_event == 0) {
			return SIZE_MAX;
		}

		// If your event has deletion (presence mask bit 0)
		if (!BitArray_is_one(stream_graph->events.link_events.presence_mask, links_iter_data->current_event)) {
			return SIZE_MAX;
		}
		links_iter_data->current_event--;
		links_iter_data->current_link = 0;
	}

	// If the current event has 0 links
	while (stream_graph->events.link_events.events[links_iter_data->current_event].nb_info == 0) {
		if ((links_iter_data->current_event == 0) ||
		    (!BitArray_is_one(stream_graph->events.link_events.presence_mask, links_iter_data->current_event))) {
			return SIZE_MAX;
		}
		links_iter_data->current_event--;
		links_iter_data->current_link = 0;
	}
	return_val = stream_graph->events.link_events.events[links_iter_data->current_event].events[links_iter_data->current_link];
	links_iter_data->current_link++;

	return return_val;
}

void LinksPresentAtTIterator_destroy(SGA_LinksIterator* links_iter) {
	free(links_iter->iterator_data);
}

SGA_LinksIterator SGA_StreamGraph_links_present_at(SGA_StreamGraph* stream_graph, SGA_Time time) {
	if (!SGA_Interval_contains(stream_graph->lifespan, time)) {
		return SGA_LinksIterator_empty();
	}

	size_t current_event = KeyInstantsTable_find_time_index_equivalent(&stream_graph->key_instants, time);

	SGA_Stream stream = {
	    .type	 = FULL_STREAM_GRAPH,
	    .stream_data = stream_graph,
	};

	LinksPresentAtTIterator* links_iter_data = MALLOC(sizeof(LinksPresentAtTIterator));
	links_iter_data->current_event		 = current_event;
	links_iter_data->current_link		 = 0;
	SGA_LinksIterator links_iter		 = {
			.stream_graph  = stream,
			.iterator_data = links_iter_data,
			.destroy       = LinksPresentAtTIterator_destroy,
			.next	       = LinksPresentAtT_next,
	    };
	return links_iter;
}
