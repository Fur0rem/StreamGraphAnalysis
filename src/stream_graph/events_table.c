#define SGA_INTERNAL

#include "events_table.h"
#include "../bit_array.h"
#include "../utils.h"
#include <stddef.h>

EventsTable EventsTable_alloc(size_t nb_node_regular_key_instants, size_t nb_node_removal_only_key_instants) {
	size_t nb_key_instants = nb_node_regular_key_instants + nb_node_removal_only_key_instants;
	return (EventsTable){
	    .node_events =
		{
		    .disappearance_index = nb_node_regular_key_instants + 1,
		    .presence_mask	 = BitArray_n_ones(nb_node_regular_key_instants),
		    .events		 = (Event*)MALLOC(nb_key_instants * sizeof(Event)),
		},
	};
}

String Event_to_string(const Event* self) {
	String str = String_with_capacity(100);
	String_append_formatted(&str, "nb_info: %zu, ", self->nb_info);
	String_append_formatted(&str, "info: ");
	for (size_t i = 0; i < self->nb_info; i++) {
		String_append_formatted(&str, "%zu ", self->events[i]);
	}
	return str;
}

// FIXME
String EventsTable_to_string(EventsTable* events_table) {
	String str = String_with_capacity(1000);

	String_push_str(&str, "[[NodeEvents]]\n");
	String_push_str(&str, "[[[PresenceMask]]] ");
	String_concat_consume(&str, BitArray_to_string(&(events_table->node_events.presence_mask)));
	String_push_str(&str, "\n[[[Events]]]\n");
	for (size_t i = 0; i < events_table->node_events.disappearance_index; i++) {
		String_append_formatted(&str, "%zu: ", i);
		// String_append_formatted(&str, "%s\n", Event_to_string(&events_table->node_events.events[i]).data);
		// String str2 = Event_to_string(&events_table->node_events.events[i]);
		String_concat_consume(&str, Event_to_string(&events_table->node_events.events[i]));
		String_push_str(&str, "\n");
	}

	String_push_str(&str, "[[LinkEvents]]\n");
	String_push_str(&str, "[[[PresenceMask]]] ");
	String_concat_consume(&str, BitArray_to_string(&(events_table->link_events.presence_mask)));
	String_push_str(&str, "\n[[[Events]]]\n");
	for (size_t i = 0; i < events_table->link_events.disappearance_index; i++) {
		String_append_formatted(&str, "%zu: ", i);
		// String_append_formatted(&str, "%s\n", Event_to_string(&events_table->link_events.events[i]).data);
		String_concat_consume(&str, Event_to_string(&events_table->link_events.events[i]));
		String_push_str(&str, "\n");
	}
	return str;
}