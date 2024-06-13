#include "induced_graph.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// TODO : Lots of copy-paste here and in its .h file. Refactor this somehow without making it unreadable
// TODO : Links work fully (tested in the extreme cases such as 0 links present), so use those as a reference for the
// nodes

size_t* SGA_NodesPresentAtT_next_after_disappearence(SGA_NodesPresentAtT* nodes_iter) {

	// If the current event has 0 nodes
	while (*SGA_Event_access_nb_nodes(nodes_iter->stream_graph->events.events[nodes_iter->current_event]) == 0) {
		if (nodes_iter->current_event == nodes_iter->stream_graph->events.nb_events - 1) {
			return NULL;
		}
		nodes_iter->current_event++;
		nodes_iter->current_node = 0;
	}
	// If you're the last node of the event
	size_t* return_val;
	if (nodes_iter->current_node ==
		*SGA_Event_access_nb_nodes(nodes_iter->stream_graph->events.events[nodes_iter->current_event])) {
		// If you're the last event
		if (nodes_iter->current_event == nodes_iter->stream_graph->events.nb_events - 1) {
			return NULL;
		}

		nodes_iter->current_event++;
		nodes_iter->current_node = 0;
	}
	return_val = SGA_Event_access_nth_node(nodes_iter->stream_graph->events.events[nodes_iter->current_event],
										   nodes_iter->current_node);
	nodes_iter->current_node++;
	return return_val;
}

// Returns NULL if there are no more nodes present at time t
size_t* SGA_NodesPresentAtT_next_before_disappearance(SGA_NodesPresentAtT* nodes_iter) {

	// If you're the last node of the event
	size_t* return_val;
	if (nodes_iter->current_node ==
		*SGA_Event_access_nb_nodes(nodes_iter->stream_graph->events.events[nodes_iter->current_event])) {
		// If you're the first event
		if (nodes_iter->current_event == 0) {
			return NULL;
		}

		// If your event has deletion (presence mask bit 0)
		if (!SGA_BitArray_is_one(nodes_iter->stream_graph->events.presence_mask, nodes_iter->current_event - 1)) {
			return NULL;
		}
		nodes_iter->current_event--;
		nodes_iter->current_node = 0;
	}
	// If the current event has 0 nodes
	while (*SGA_Event_access_nb_nodes(nodes_iter->stream_graph->events.events[nodes_iter->current_event]) == 0) {
		if (nodes_iter->current_event == 0) {
			return NULL;
		}
		nodes_iter->current_event--;
		nodes_iter->current_node = 0;
	}
	return_val = SGA_Event_access_nth_node(nodes_iter->stream_graph->events.events[nodes_iter->current_event],
										   nodes_iter->current_node);
	nodes_iter->current_node++;
	return return_val;
}

SGA_NodesPresentAtT SGA_get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t) {
	size_t current_event = SGA_KeyMomentsTable_find_time_index(&stream_graph->key_moments, t);
	SGA_NodesPresentAtT nodes_present_at_t = {
		.stream_graph = stream_graph, .current_event = current_event, .current_node = 0};
	if (current_event >= stream_graph->events.disappearance_index) {
		nodes_present_at_t.next = (size_t * (*)(void*)) SGA_NodesPresentAtT_next_after_disappearence;
	}
	else {
		nodes_present_at_t.next = (size_t * (*)(void*)) SGA_NodesPresentAtT_next_before_disappearance;
	}
	return nodes_present_at_t;
}

// Okay so next_after i have to put the skip loop before and in the next_before i have to put it after

size_t* SGA_LinksPresentAtT_next_after_disappearence(SGA_LinksPresentAtT* links_iter) {

	// If the current event has 0 links
	while (*SGA_Event_access_nb_links(links_iter->stream_graph->events.events[links_iter->current_event]) == 0) {
		if (links_iter->current_event == links_iter->stream_graph->events.nb_events - 1) {
			return NULL;
		}
		links_iter->current_event++;
		links_iter->current_link = 0;
	}
	// If you're the last link of the event
	size_t* return_val;
	if (links_iter->current_link ==
		*SGA_Event_access_nb_links(links_iter->stream_graph->events.events[links_iter->current_event])) {
		// If you're the last event
		if (links_iter->current_event == links_iter->stream_graph->events.nb_events - 1) {
			return NULL;
		}

		links_iter->current_event++;
		links_iter->current_link = 0;
	}
	return_val = SGA_Event_access_nth_link(links_iter->stream_graph->events.events[links_iter->current_event],
										   links_iter->current_link);
	links_iter->current_link++;
	return return_val;
}

// Returns NULL if there are no more links present at time t
size_t* SGA_LinksPresentAtT_next_before_disappearance(SGA_LinksPresentAtT* links_iter) {

	// If you're the last link of the event
	size_t* return_val;
	if (links_iter->current_link ==
		*SGA_Event_access_nb_links(links_iter->stream_graph->events.events[links_iter->current_event])) {
		// If you're the first event
		if (links_iter->current_event == 0) {
			return NULL;
		}

		// If your event has deletion (presence mask bit 0)
		if (!SGA_BitArray_is_one(links_iter->stream_graph->events.presence_mask, links_iter->current_event - 1)) {
			return NULL;
		}
		links_iter->current_event--;
		links_iter->current_link = 0;
	}

	// If the current event has 0 links
	while (*SGA_Event_access_nb_links(links_iter->stream_graph->events.events[links_iter->current_event]) == 0) {
		if ((links_iter->current_event == 0) ||
			(!SGA_BitArray_is_one(links_iter->stream_graph->events.presence_mask, links_iter->current_event - 1))) {
			return NULL;
		}
		links_iter->current_event--;
		links_iter->current_link = 0;
	}
	return_val = SGA_Event_access_nth_link(links_iter->stream_graph->events.events[links_iter->current_event],
										   links_iter->current_link);
	links_iter->current_link++;
	return return_val;
}

SGA_LinksPresentAtT SGA_get_links_present_at_t(StreamGraph* stream_graph, TimeId t) {
	size_t current_event = SGA_KeyMomentsTable_find_time_index(&stream_graph->key_moments, t);
	SGA_LinksPresentAtT links_present_at_t = {
		.stream_graph = stream_graph, .current_event = current_event, .current_link = 0};
	if (current_event >= stream_graph->events.disappearance_index) {
		links_present_at_t.next = (size_t * (*)(void*)) SGA_LinksPresentAtT_next_after_disappearence;
	}
	else {
		links_present_at_t.next = (size_t * (*)(void*)) SGA_LinksPresentAtT_next_before_disappearance;
	}
	return links_present_at_t;
}