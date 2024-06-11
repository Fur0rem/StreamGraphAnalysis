#include "events_table.h"

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