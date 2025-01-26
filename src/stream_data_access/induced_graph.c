#define SGA_INTERNAL

#include "../utils.h"
#include "key_moments.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief The data of the iterator over the nodes present at a given time in a StreamGraph.
 */
typedef struct {
	size_t current_event;
	size_t current_node;
} NodesPresentAtTIterator;

/**
 * @brief The data of the iterator over the links present at a given time in a StreamGraph.
 */
typedef struct {
	size_t current_event;
	size_t current_link;
} LinksPresentAtTIterator;

// TODO : Lots of copy-paste here and in its .h file. Refactor this somehow if you can do it without making it
// unreadable
// TODO : Links work fully (tested in the extreme cases such as 0 links present), so use those as a reference for the
// nodes
// TODO : Maybe merge all the data access files into one, since they're all quite similar and small

size_t LinksPresentAtT_next_after_disappearence(SGA_LinksIterator* links_iter) {

	LinksPresentAtTIterator* links_iter_data = (LinksPresentAtTIterator*)links_iter->iterator_data;
	SGA_StreamGraph* stream_graph		 = links_iter->stream_graph.stream_data;

	// If you asked for the very last key moment, nothing is present
	// I need to check if the current event is equal to the number of events because of the shift by 1 in the after
	// disappearance case
	if (links_iter_data->current_event >= stream_graph->events.nb_events) {
		return SIZE_MAX;
	}

	// If the current event has 0 links
	while (stream_graph->events.link_events.events[links_iter_data->current_event].nb_info == 0) {
		if (links_iter_data->current_event >= stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}
		links_iter_data->current_event++;
		links_iter_data->current_link = 0;
	}
	// If you're the last link of the event
	size_t return_val;
	if (links_iter_data->current_link == stream_graph->events.link_events.events[links_iter_data->current_event].nb_info) {
		// If you're the last event
		if (links_iter_data->current_event >= stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}

		links_iter_data->current_event++;
		links_iter_data->current_link = 0;
	}
	if (stream_graph->events.link_events.events[links_iter_data->current_event].nb_info == 0) {
		return SIZE_MAX;
	}
	return_val = stream_graph->events.link_events.events[links_iter_data->current_event].events[links_iter_data->current_link];
	links_iter_data->current_link++;
	return return_val;
}

// Returns NULL if there are no more links present at time t
size_t LinksPresentAtT_next_before_disappearance(SGA_LinksIterator* links_iter) {

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
		if (!BitArray_is_one(stream_graph->events.link_events.presence_mask, links_iter_data->current_event - 1)) {
			return SIZE_MAX;
		}
		links_iter_data->current_event--;
		links_iter_data->current_link = 0;
	}

	// If the current event has 0 links
	while (stream_graph->events.link_events.events[links_iter_data->current_event].nb_info == 0) {
		if ((links_iter_data->current_event == 0) ||
		    (!BitArray_is_one(stream_graph->events.link_events.presence_mask, links_iter_data->current_event - 1))) {
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
	DEV_ASSERT(SGA_Interval_contains(stream_graph->lifespan, time));

	size_t current_event = KeyMomentsTable_find_time_index(&stream_graph->key_moments, time);

	// If the time is exactly a key moment after disappearance, we need to skip the disappearance event
	if (time >= KeyMomentsTable_nth_key_moment(&stream_graph->key_moments, stream_graph->events.link_events.disappearance_index)) {
		current_event++;
	}

	// printf("Current event: %zu\n", current_event);

	LinksPresentAtTIterator* links_iter_data = MALLOC(sizeof(LinksPresentAtTIterator));
	links_iter_data->current_event		 = current_event;
	links_iter_data->current_link		 = 0;

	SGA_Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = stream_graph};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = stream_graph;

	if (current_event > stream_graph->events.nb_events) {
		return (SGA_LinksIterator){.stream_graph  = stream,
					   .iterator_data = links_iter_data,
					   .next	  = LinksPresentAtT_next_after_disappearence,
					   .destroy	  = LinksPresentAtTIterator_destroy};
	}

	SGA_LinksIterator links_iter = {
	    .stream_graph  = stream,
	    .iterator_data = links_iter_data,
	    .destroy	   = LinksPresentAtTIterator_destroy,
	};
	if (current_event > stream_graph->events.link_events.disappearance_index) {
		links_iter.next = LinksPresentAtT_next_after_disappearence;
	}
	else {
		links_iter.next = LinksPresentAtT_next_before_disappearance;
	}
	return links_iter;
}

// Okay so next_after i have to put the skip loop before and in the next_before i have to put it after

size_t NodesPresentAtT_next_after_disappearence(SGA_NodesIterator* nodes_iter) {

	NodesPresentAtTIterator* nodes_iter_data = (NodesPresentAtTIterator*)nodes_iter->iterator_data;
	SGA_StreamGraph* stream_graph		 = nodes_iter->stream_graph.stream_data;

	if (nodes_iter_data->current_event >= stream_graph->events.nb_events) {
		return SIZE_MAX;
	}
	// If the current event has 0 nodes
	while (stream_graph->events.node_events.events[nodes_iter_data->current_event].nb_info == 0) {
		if (nodes_iter_data->current_event >= stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}
		nodes_iter_data->current_event++;
		nodes_iter_data->current_node = 0;
	}
	// If you're the last node of the event
	size_t return_val;
	if (nodes_iter_data->current_node == stream_graph->events.node_events.events[nodes_iter_data->current_event].nb_info) {
		// If you're the last event
		if (nodes_iter_data->current_event >= stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}

		nodes_iter_data->current_event++;
		nodes_iter_data->current_node = 0;
	}
	if (stream_graph->events.node_events.events[nodes_iter_data->current_event].nb_info == 0) {
		return SIZE_MAX;
	}
	return_val = stream_graph->events.node_events.events[nodes_iter_data->current_event].events[nodes_iter_data->current_node];
	nodes_iter_data->current_node++;
	return return_val;
}

// Returns NULL if there are no more nodes present at time t
size_t NodesPresentAtT_next_before_disappearance(SGA_NodesIterator* nodes_iter) {

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
		if (!BitArray_is_one(stream_graph->events.node_events.presence_mask, nodes_iter_data->current_event - 1)) {
			return SIZE_MAX;
		}
		nodes_iter_data->current_event--;
		nodes_iter_data->current_node = 0;
	}

	// If the current event has 0 nodes
	while (stream_graph->events.node_events.events[nodes_iter_data->current_event].nb_info == 0) {
		if ((nodes_iter_data->current_event == 0) ||
		    (!BitArray_is_one(stream_graph->events.node_events.presence_mask, nodes_iter_data->current_event - 1))) {
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
	DEV_ASSERT(SGA_Interval_contains(stream_graph->lifespan, time));

	size_t current_event = KeyMomentsTable_find_time_index(&stream_graph->key_moments, time);

	// If the time is exactly a key moment after disappearance, we need to skip the disappearance event
	if (time >= KeyMomentsTable_nth_key_moment(&stream_graph->key_moments, stream_graph->events.node_events.disappearance_index)) {
		current_event++;
	}

	// printf("Current event: %zu\n", current_event);

	SGA_Stream stream = {.type = FULL_STREAM_GRAPH, .stream_data = stream_graph};
	// SGA_Stream* stream = MALLOC(sizeof(SGA_Stream));
	// stream->type = FULL_STREAM_GRAPH;
	// stream->stream = stream_graph;
	if (current_event > stream_graph->events.nb_events) {
		NodesPresentAtTIterator* nodes_iter_data = MALLOC(sizeof(NodesPresentAtTIterator));
		return (SGA_NodesIterator){
		    .stream_graph  = stream,
		    .iterator_data = nodes_iter_data,
		    .next	   = NodesPresentAtT_next_after_disappearence,
		    .destroy	   = NodesPresentAtTIterator_destroy,
		};
	}

	NodesPresentAtTIterator* nodes_iter_data = MALLOC(sizeof(NodesPresentAtTIterator));
	nodes_iter_data->current_event		 = current_event;
	nodes_iter_data->current_node		 = 0;
	SGA_NodesIterator nodes_iter		 = {
			.stream_graph  = stream,
			.iterator_data = nodes_iter_data,
			.destroy       = NodesPresentAtTIterator_destroy,
	    };
	if (current_event > stream_graph->events.node_events.disappearance_index) {
		nodes_iter.next = NodesPresentAtT_next_after_disappearence;
	}
	else {
		nodes_iter.next = NodesPresentAtT_next_before_disappearance;
	}
	return nodes_iter;
}