#ifndef EVENTS_TABLE_H
#define EVENTS_TABLE_H

#include "../bit_array.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	size_t nb_info;
	size_t* events;
} Event;

typedef struct {
	Event* events;
	TimeId disappearance_index;
	BitArray presence_mask;
} Events;

typedef struct {
	size_t nb_events;
	Events node_events;
	Events link_events;
} EventsTable;

EventsTable EventsTable_alloc(size_t nb_node_regular_key_moments, size_t nb_node_removal_only_key_moments);

#endif