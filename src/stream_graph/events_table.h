#include "../bit_array.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	size_t nb_nodes;
	size_t nb_links;
	size_t* events;
} Event;

typedef struct {
	TimeId disappearance_index;
	BitArray presence_mask;
	size_t nb_events;
	Event* events; // The first element contains the number of nodes in the array, then the n
} EventsTable;

size_t* Event_access_nth_node(Event event, size_t n);
size_t* Event_access_nth_link(Event event, size_t n);

EventsTable EventsTable_alloc(size_t nb_regular_key_moments, size_t nb_removal_only_key_moments);