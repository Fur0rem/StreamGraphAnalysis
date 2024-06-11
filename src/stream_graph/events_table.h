#include "../bit_array.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef size_t* Event;

typedef struct {
	TimeId disappearance_index;
	SGA_BitArray presence_mask;
	size_t nb_events;
	Event* events; // The first element contains the number of nodes in the array, then the n
} EventsTable;

size_t* SGA_Event_access_nb_nodes(Event event);
size_t* SGA_Event_access_nb_links(Event event);
size_t* SGA_Event_access_nth_node(Event event, size_t n);
size_t* SGA_Event_access_nth_link(Event event, size_t n);