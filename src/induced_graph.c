#include "induced_graph.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// TODO : Lots of copy-paste here and in its .h file. Refactor this somehow without making it unreadable
// TODO : Links work fully (tested in the extreme cases such as 0 links present), so use those as a reference for the
// nodes

size_t NodesPresentAtT_next_after_disappearence(NodesPresentAtT* nodes_iter) {

	// If the current event has 0 nodes
	while (nodes_iter->stream_graph->events.node_events.events[nodes_iter->current_event].nb_info == 0) {
		if (nodes_iter->current_event == nodes_iter->stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}
		nodes_iter->current_event++;
		nodes_iter->current_node = 0;
	}
	// If you're the last node of the event
	size_t return_val;
	if (nodes_iter->current_node ==
		nodes_iter->stream_graph->events.node_events.events[nodes_iter->current_event].nb_info) {
		// If you're the last event
		if (nodes_iter->current_event == nodes_iter->stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}

		nodes_iter->current_event++;
		nodes_iter->current_node = 0;
	}
	// return_val = Event_access_nth_node(nodes_iter->stream_graph->events.events[nodes_iter->current_event],
	//								   nodes_iter->current_node);
	return_val =
		nodes_iter->stream_graph->events.node_events.events[nodes_iter->current_event].events[nodes_iter->current_node];

	nodes_iter->current_node++;
	return return_val;
}

// Returns NULL if there are no more nodes present at time t
size_t NodesPresentAtT_next_before_disappearance(NodesPresentAtT* nodes_iter) {

	// If you're the last node of the event
	size_t return_val;
	if (nodes_iter->current_node ==
		nodes_iter->stream_graph->events.node_events.events[nodes_iter->current_event].nb_info) {
		// If you're the first event
		if (nodes_iter->current_event == 0) {
			return SIZE_MAX;
		}

		// If your event has deletion (presence mask bit 0)
		if (!BitArray_is_one(nodes_iter->stream_graph->events.node_events.presence_mask,
							 nodes_iter->current_event - 1)) {
			return SIZE_MAX;
		}
		nodes_iter->current_event--;
		nodes_iter->current_node = 0;
	}
	// If the current event has 0 nodes
	while (nodes_iter->stream_graph->events.node_events.events[nodes_iter->current_event].nb_info == 0) {
		if (nodes_iter->current_event == 0) {
			return SIZE_MAX;
		}
		nodes_iter->current_event--;
		nodes_iter->current_node = 0;
	}
	return_val =
		nodes_iter->stream_graph->events.node_events.events[nodes_iter->current_event].events[nodes_iter->current_node];
	nodes_iter->current_node++;
	return return_val;
}

NodesPresentAtT get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t) {
	size_t current_event = KeyMomentsTable_find_time_index(&stream_graph->key_moments, t);
	NodesPresentAtT nodes_present_at_t = {
		.stream_graph = stream_graph, .current_event = current_event, .current_node = 0};
	if (current_event >= stream_graph->events.node_events.disappearance_index) {
		nodes_present_at_t.next = (size_t(*)(void*))NodesPresentAtT_next_after_disappearence;
	}
	else {
		nodes_present_at_t.next = (size_t(*)(void*))NodesPresentAtT_next_before_disappearance;
	}
	return nodes_present_at_t;
}

// Okay so next_after i have to put the skip loop before and in the next_before i have to put it after

size_t LinksPresentAtT_next_after_disappearence(LinksPresentAtT* links_iter) {

	// If the current event has 0 links
	while (links_iter->stream_graph->events.link_events.events[links_iter->current_event].nb_info == 0) {
		if (links_iter->current_event >= links_iter->stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}
		links_iter->current_event++;
		links_iter->current_link = 0;
	}
	// If you're the last link of the event
	size_t return_val;
	if (links_iter->current_link ==
		links_iter->stream_graph->events.link_events.events[links_iter->current_event].nb_info) {
		// If you're the last event
		if (links_iter->current_event >= links_iter->stream_graph->events.nb_events - 1) {
			return SIZE_MAX;
		}

		links_iter->current_event++;
		links_iter->current_link = 0;
	}
	if (links_iter->stream_graph->events.link_events.events[links_iter->current_event].nb_info == 0) {
		return SIZE_MAX;
	}
	return_val =
		links_iter->stream_graph->events.link_events.events[links_iter->current_event].events[links_iter->current_link];
	links_iter->current_link++;
	return return_val;
}

// Returns NULL if there are no more links present at time t
size_t LinksPresentAtT_next_before_disappearance(LinksPresentAtT* links_iter) {

	// If you're the last link of the event
	size_t return_val;
	if (links_iter->current_link ==
		links_iter->stream_graph->events.link_events.events[links_iter->current_event].nb_info) {
		// If you're the first event
		if (links_iter->current_event == 0) {
			return SIZE_MAX;
		}

		// If your event has deletion (presence mask bit 0)
		if (!BitArray_is_one(links_iter->stream_graph->events.link_events.presence_mask,
							 links_iter->current_event - 1)) {
			return SIZE_MAX;
		}
		links_iter->current_event--;
		links_iter->current_link = 0;
	}

	// If the current event has 0 links
	while (links_iter->stream_graph->events.link_events.events[links_iter->current_event].nb_info == 0) {
		if ((links_iter->current_event == 0) ||
			(!BitArray_is_one(links_iter->stream_graph->events.link_events.presence_mask,
							  links_iter->current_event - 1))) {
			return SIZE_MAX;
		}
		links_iter->current_event--;
		links_iter->current_link = 0;
	}
	return_val =
		links_iter->stream_graph->events.link_events.events[links_iter->current_event].events[links_iter->current_link];
	links_iter->current_link++;
	return return_val;
}

// TODO : issue with timestamp that corresponds exactly to an event after disappearance
LinksPresentAtT get_links_present_at_t(StreamGraph* stream_graph, TimeId t) {
	size_t current_event = KeyMomentsTable_find_time_index(&stream_graph->key_moments, t);
	if (t >= KeyMomentsTable_nth_key_moment(&stream_graph->key_moments,
											stream_graph->events.link_events.disappearance_index)) {
		current_event++;
	}
	if (current_event > stream_graph->events.nb_events) {
		return (LinksPresentAtT){.stream_graph = stream_graph, .current_event = SIZE_MAX, .current_link = 0};
	}

	LinksPresentAtT links_present_at_t = {
		.stream_graph = stream_graph, .current_event = current_event, .current_link = 0};
	if (current_event > stream_graph->events.link_events.disappearance_index) {
		links_present_at_t.next = (size_t(*)(void*))LinksPresentAtT_next_after_disappearence;
	}
	else {
		links_present_at_t.next = (size_t(*)(void*))LinksPresentAtT_next_before_disappearance;
	}
	return links_present_at_t;
}