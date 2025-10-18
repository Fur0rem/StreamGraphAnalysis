#ifndef PARSING_EVENTS_H
#define PARSING_EVENTS_H

#include "../generic_data_structures/arraylist.h"
#include "../generic_data_structures/hashset.h"
#include "../interval.h"
#include "cursor.h"
#include <stddef.h>

/**
 * @brief A single event, signifies the appearance of disappearance of a certain link or node
 */
typedef struct SGA_ParsedEvent {
	size_t instant; ///< The instant at which the event occurs
	enum {
		Appearance,
		Disappearance,
	} event_kind; ///< Whether the event was an appearance or disappearance
	enum {
		Node,
		Link,
	} elem_kind; ///< Whether the event was for a node or a link
	union {
		size_t node; ///< The id of the node
		struct {
			size_t node1; ///< The id of the first node of the link
			size_t node2; ///< The id of the second node of the link
		} link;		      ///< A link is represented by two nodes
	} id;			      ///< The id of the node or link
} SGA_ParsedEvent;

DeclareDestroy(SGA_ParsedEvent);

int SGA_ParsedEvent_compare(SGA_ParsedEvent* a, SGA_ParsedEvent* b);

/**
 * @brief A mapping from a couple of node ids to a link id
 */
typedef struct LinkIdMap {
	size_t nodes[2]; ///< The two node ids
	size_t id;	 ///< The link id
} LinkIdMap;

DeclareDestroy(LinkIdMap);

DeclareHash(LinkIdMap);
DeclareEquals(LinkIdMap);

DeclareArrayList(LinkIdMap);
DeclareArrayListDeriveRemove(LinkIdMap);
DeclareHashset(LinkIdMap);
DeclareHashsetDeriveRemove(LinkIdMap);

SGA_ParsedEvent SGA_parse_single_event(SGA_ParsingCursor* cursor, SGA_IntervalsSetBuilderArrayList* node_presences,
				       SGA_IntervalsSetBuilderArrayList* link_presences, LinkIdMapHashset* link_id_map,
				       SGA_LinkIdArrayListArrayList* neighbours_of_nodes);

DeclareArrayList(SGA_ParsedEvent);
DeclareArrayListDeriveOrdered(SGA_ParsedEvent);
DeclareArrayListDeriveRemove(SGA_ParsedEvent);

SGA_ParsedEventArrayList SGA_parse_events(SGA_ParsingCursor* cursor, SGA_IntervalsSetBuilderArrayList* node_presences,
					  SGA_IntervalsSetBuilderArrayList* link_presences, LinkIdMapHashset* link_id_map,
					  SGA_LinkIdArrayListArrayList* neighbours_of_nodes);

#endif // PARSING_EVENTS_H