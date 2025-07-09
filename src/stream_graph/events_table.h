/**
 * @file src/stream_graph/events_table.h
 */

#ifndef EVENTS_TABLE_H
#define EVENTS_TABLE_H

#include "../bit_array.h"
#include "../units.h"
#include "key_instants_table.h"
#include <stddef.h>
#include <stdint.h>

typedef struct Event Event;
#ifdef SGA_INTERNAL

/**
 * @brief An event in the events table. Contains the information about the nodes or links present at that event.
 * This structure needs to be used with it's associated Events structure for the presence mask to know if the event has only appearances or
 * also disappearances.
 */
struct Event {
	size_t nb_info; ///< The number of informations present at this event.
	size_t* events; ///< The array of events, each event is a node or a link id that was added at this event.
};
#endif // SGA_INTERNAL

/**
 * @brief A collection of events, where each event contains the information about the nodes or links present at that event.
 * The presence mask indicates whether the event has only appearances (1) or has at least one disappearance (0).
 * If the event has only appearances, the events array will contain the ids of the nodes or links that appeared at that event.
 * If the event has at least one disappearance, the events array will contain all the ids of the nodes or links that are present at that
 * event.
 */
typedef struct {
	Event* events;		///< Array of events, each event contains the information about the nodes or links present at that event
	BitArray presence_mask; ///< Presence mask, where each bit represents whether the event has only appearances (1) or has at least one
				/// disappearance (0).
} Events;

/**
 * @brief A table of events, containing the information about the nodes and links present at each event.
 * It contains the number of events, and two separate collections of events for nodes and links.
 */
typedef struct {
	size_t nb_events;   ///< The number of events in the table. This is the same for both node and link events. Should be equal to the
			    ///< number of key instants in the key instants table.
	Events node_events; ///< Collection of events for nodes.
	Events link_events; ///< Collection of events for links.
} EventsTable;

#ifdef SGA_INTERNAL

/**
 * @brief Create an EventsTable from the given node and link events lists, presence masks, and number of events.
 * @param node_events_list The list of node events, where each event is a list of IDs, should be of length nb_events.
 * @param link_events_list The list of link events, where each event is a list of IDs, should be of lentth nb_events.
 * @param node_presence_mask The presence mask for node events.
 * @param link_presence_mask The presence mask for link events.
 * @param nb_events The number of events in the table.
 * @return The EventsTable initialised and ready to be used.
 */
EventsTable EventsTable_create(const size_tArrayList* node_events_list, const size_tArrayList* link_events_list,
			       BitArray node_presence_mask, BitArray link_presence_mask, size_t nb_events);

/**
 * @brief Destroy the given EventsTable, freeing all allocated memory.
 * @param events_table The EventsTable to destroy.
 */
void EventsTable_destroy(EventsTable events_table);

/**
 * @brief Format the EventsTable as a human-readable string.
 * @param[in] events_table The EventsTable to format.
 * @param[in] key_instants_table The KeyInstantsTable to use for formatting the key instants.
 * @return A string representation of the EventsTable.
 */
String EventsTable_to_string(const EventsTable* events_table, const KeyInstantsTable* key_instants_table);

#endif // SGA_INTERNAL

#endif