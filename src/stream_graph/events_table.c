#include "events_table.h"
#include "../utils.h"
#include <stddef.h>

EventsTable EventsTable_alloc(size_t nb_node_regular_key_moments, size_t nb_node_removal_only_key_moments) {
	size_t nb_key_moments = nb_node_regular_key_moments + nb_node_removal_only_key_moments;
	return (EventsTable){
		.node_events =
			{
				.disappearance_index = nb_node_regular_key_moments + 1,
				.presence_mask		 = BitArray_n_ones(nb_node_regular_key_moments),
				.events				 = (Event*)MALLOC(nb_key_moments * sizeof(Event)),
			},
	};
}