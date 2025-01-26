/**
 * @file src/stream_graph/events_table.h
 */

#ifndef EVENTS_TABLE_H
#define EVENTS_TABLE_H

#include "../bit_array.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef struct Event Event;
#ifdef SGA_INTERNAL
struct Event {
	size_t nb_info;
	size_t* events;
};
#endif // SGA_INTERNAL

typedef struct {
	Event* events;
	SGA_TimeId disappearance_index;
	BitArray presence_mask;
} Events;

typedef struct {
	size_t nb_events;
	Events node_events;
	Events link_events;
} EventsTable;

#ifdef SGA_INTERNAL

EventsTable EventsTable_alloc(size_t nb_node_regular_key_moments, size_t nb_node_removal_only_key_moments);
String EventsTable_to_string(EventsTable* events_table);
// TODO: no destroy ?

#endif // SGA_INTERNAL

#endif