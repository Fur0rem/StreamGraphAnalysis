#define SGA_INTERNAL

#include "events_table.h"
#include "../bit_array.h"
#include "../utils.h"
#include <stddef.h>

EventsTable EventsTable_create(const size_tArrayList* node_events_list, const size_tArrayList* link_events_list,
			       BitArray node_presence_mask, BitArray link_presence_mask, size_t nb_events) {

	// Realloc the arraylists into the events
	Events node_events = {
	    .events	   = MALLOC(sizeof(Event) * nb_events),
	    .presence_mask = node_presence_mask,
	};

	Events link_events = {
	    .events	   = MALLOC(sizeof(Event) * nb_events),
	    .presence_mask = link_presence_mask,
	};

	// Re-copy the events from the arraylists to the events
	for (size_t i = 0; i < nb_events; i++) {
		// Fetch the nodes
		Event node_event;
		node_event.nb_info = node_events_list[i].length;
		// If there was no node event at this time, no need to allocate memory
		if (node_event.nb_info == 0) {
			node_event.events = NULL;
		}
		else {
			node_event.events = MALLOC(sizeof(size_t) * node_event.nb_info);
			memcpy(node_event.events, node_events_list[i].array, sizeof(size_t) * node_event.nb_info);
		}
		node_events.events[i] = node_event;

		// Fetch the links
		Event link_event;
		link_event.nb_info = link_events_list[i].length;
		if (link_event.nb_info == 0) {
			// If there was no link event at this time, no need to allocate memory
			link_event.events = NULL;
		}
		else {
			link_event.events = MALLOC(sizeof(size_t) * link_event.nb_info);
			memcpy(link_event.events, link_events_list[i].array, sizeof(size_t) * link_event.nb_info);
		}
		link_events.events[i] = link_event;
	}

	return (EventsTable){
	    .nb_events	 = nb_events,
	    .node_events = node_events,
	    .link_events = link_events,
	};
}

String EventsTable_to_string(const EventsTable* events_table, const KeyInstantsTable* key_instants) {

	if (events_table->nb_events == 0) {
		return String_from_owned("No events\n");
	}

	String str = String_with_capacity(1024); // Buffer for the string representation

	for (size_t i = 0; i < events_table->nb_events; i++) {
		String_append_formatted(&str, "Event %zu (t = %zu): ", i, KeyInstantsTable_nth_key_instant(key_instants, i));

		// Node events
		if (events_table->node_events.events[i].nb_info > 0) {
			String_push_str(&str, "Node events: ");
			for (size_t j = 0; j < events_table->node_events.events[i].nb_info; j++) {
				String_append_formatted(&str, "%zu ", events_table->node_events.events[i].events[j]);
			}
		}
		else {
			String_push_str(&str, "No node events ");
		}
		bool node_presence = BitArray_is_one(events_table->node_events.presence_mask, i);
		if (node_presence) {
			String_push_str(&str, "+");
		}
		else {
			String_push_str(&str, "-");
		}

		String_push_str(&str, " | ");

		// Link events
		if (events_table->link_events.events[i].nb_info > 0) {
			String_push_str(&str, "Link events: ");
			for (size_t j = 0; j < events_table->link_events.events[i].nb_info; j++) {
				String_append_formatted(&str, "%zu ", events_table->link_events.events[i].events[j]);
			}
		}
		else {
			String_push_str(&str, "No link events ");
		}
		bool link_presence = BitArray_is_one(events_table->link_events.presence_mask, i);
		if (link_presence) {
			String_push_str(&str, "+");
		}
		else {
			String_push_str(&str, "-");
		}
		String_push_str(&str, "\n");
	}

	return str;
}

void EventsTable_destroy(EventsTable events_table) {
	for (size_t i = 0; i < events_table.nb_events; i++) {
		if (events_table.node_events.events[i].events != NULL) {
			free(events_table.node_events.events[i].events);
		}
		if (events_table.link_events.events[i].events != NULL) {
			free(events_table.link_events.events[i].events);
		}
	}
	free(events_table.node_events.events);
	free(events_table.link_events.events);
	BitArray_destroy(events_table.node_events.presence_mask);
	BitArray_destroy(events_table.link_events.presence_mask);
}