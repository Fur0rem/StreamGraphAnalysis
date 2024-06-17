#include "events_table.h"
#include "../utils.h"
#include <stddef.h>

EventsTable EventsTable_alloc(size_t nb_node_regular_key_moments, size_t nb_node_removal_only_key_moments,
							  size_t nb_link_regular_key_moments, size_t nb_link_removal_only_key_moments) {
	EventsTable et;
	DEBUG_ASSERT((nb_node_regular_key_moments + nb_node_removal_only_key_moments) ==
				 (nb_link_regular_key_moments + nb_link_removal_only_key_moments));
	size_t nb_key_moments = nb_node_regular_key_moments + nb_node_removal_only_key_moments;
	et.node_events.disappearance_index = nb_node_regular_key_moments + 1;
	et.node_events.presence_mask = BitArray_n_ones(nb_node_regular_key_moments);
	et.node_events.events = (Event*)MALLOC(nb_key_moments * sizeof(Event));
	return et;
}