#include "induced_graph.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

SGA_NodesPresentAtT SGA_get_nodes_present_at_t(StreamGraph* stream_graph, TimeId t) {
	size_t current_event = SGA_KeyMomentsTable_find_time_index(&stream_graph->key_moments, t);
	SGA_NodesPresentAtT nodes_present_at_t = {
		.stream_graph = stream_graph, .current_event = current_event, .current_node = 0};
	return nodes_present_at_t;
}

SGA_LinksPresentAtT SGA_get_links_present_at_t(StreamGraph* stream_graph, TimeId t) {
	size_t current_event = SGA_KeyMomentsTable_find_time_index(&stream_graph->key_moments, t);
	SGA_LinksPresentAtT links_present_at_t = {
		.stream_graph = stream_graph, .current_event = current_event, .current_link = 0};
	return links_present_at_t;
}

size_t* SGA_NodesPresentAtT_next_after_disappearence(SGA_NodesPresentAtT* nodes_iter) {

	// If the current event has 0 nodes
	while (*SGA_Event_access_nb_nodes(nodes_iter->stream_graph->events.events[nodes_iter->current_event]) == 0) {
		nodes_iter->current_event++;
		if (nodes_iter->current_event == nodes_iter->stream_graph->events.nb_events - 1) {
			return NULL;
		}
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

	// If the current event has 0 nodes
	while (*SGA_Event_access_nb_nodes(nodes_iter->stream_graph->events.events[nodes_iter->current_event]) == 0) {
		nodes_iter->current_event--;
		if (nodes_iter->current_event == 0) {
			return NULL;
		}
	}
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
	return_val = SGA_Event_access_nth_node(nodes_iter->stream_graph->events.events[nodes_iter->current_event],
										   nodes_iter->current_node);
	nodes_iter->current_node++;
	return return_val;
}

size_t* SGA_NodesPresentAtT_next(SGA_NodesPresentAtT* nodes_iter) {
	if (nodes_iter->current_event < nodes_iter->stream_graph->events.disappearance_index) {
		return SGA_NodesPresentAtT_next_before_disappearance(nodes_iter);
	}
	else {
		return SGA_NodesPresentAtT_next_after_disappearence(nodes_iter);
	}
}

// Returns NULL if there are no more nodes present at time t
size_t* SGA_LinksPresentAtT_next(SGA_LinksPresentAtT* links_iter) {
	// If the current event has 0 nodes
	while (*SGA_Event_access_nb_nodes(links_iter->stream_graph->events.events[links_iter->current_event]) == 0) {
		links_iter->current_event--;
		if (links_iter->current_event == 0) {
			return NULL;
		}
	}
	// If you're the last node of the event
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
		// Otherwise, go to the previous event
		links_iter->current_event--;
		links_iter->current_link = 0;
	}
	return_val = SGA_Event_access_nth_link(links_iter->stream_graph->events.events[links_iter->current_event],
										   links_iter->current_link);
	links_iter->current_link++;
	return return_val;
}