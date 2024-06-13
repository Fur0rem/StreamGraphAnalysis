#include "events_table.h"
#include "../utils.h"
#include <stddef.h>

size_t* SGA_Event_access_nb_nodes(Event event) {
	return event + 0;
}

size_t* SGA_Event_access_nb_links(Event event) {
	return event + 1;
}

size_t* SGA_Event_access_nth_node(Event event, size_t n) {
	return event + 2 + n;
}

size_t* SGA_Event_access_nth_link(Event event, size_t n) {
	return event + 2 + *SGA_Event_access_nb_nodes(event) + n;
}

EventsTable SGA_EventsTable_alloc(size_t nb_regular_key_moments, size_t nb_removal_only_key_moments) {
	EventsTable et;
	size_t nb_key_moments = nb_regular_key_moments + nb_removal_only_key_moments + 1;
	et.nb_events = nb_key_moments;
	et.events = (Event*)MALLOC(nb_key_moments * sizeof(Event));
	et.disappearance_index = nb_regular_key_moments + 1;
	et.presence_mask = SGA_BitArray_n_ones(nb_regular_key_moments);
	return et;
}