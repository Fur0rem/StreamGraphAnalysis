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

String Event_to_string(const Event* event) {
	String str = String_with_capacity(100);
	String_append_formatted(&str, "nb_info: %zu, ", event->nb_info);
	String_append_formatted(&str, "info: ");
	for (size_t i = 0; i < event->nb_info; i++) {
		String_append_formatted(&str, "%zu ", event->events[i]);
	}
	return str;
}

// FIXME
String EventsTable_to_string(EventsTable* events_table) {
	String str = String_with_capacity(1000);
	String_push_str(&str, "[[NodeEvents]]\n");
	String_push_str(&str, "[[[PresenceMask]]]\n");
	String_concat_consume(&str, BitArray_to_string(&(events_table->node_events.presence_mask)));
	String_push_str(&str, "[[[Events]]]\n");
	for (size_t i = 0; i < events_table->node_events.disappearance_index; i++) {
		String_append_formatted(&str, "%zu: ", i);
		String_append_formatted(&str, "%s\n", Event_to_string(&events_table->node_events.events[i]).data);
	}
	return str;
}