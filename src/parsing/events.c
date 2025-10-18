#include "events.h"
#include "../generic_data_structures/arraylist.h"
#include "../generic_data_structures/hashset.h"
#include "../interval.h"
#include "cursor.h"
#include <stddef.h>

NO_FREE(SGA_ParsedEvent);
DefineArrayListDeriveRemove(SGA_ParsedEvent);

void print_event_format() {
	fprintf(stderr, "Hint - Expected event format: \"time_instant sign letter id(s)\"\n");
	fprintf(stderr, "Where time_instant is an unsigned integer representing the time instant at which the event occurs,\n");
	fprintf(stderr, "sign is either '+' (appearance of a node/link) or '-' (disappearance of a node/link),\n");
	fprintf(stderr, "letter is either 'N' (node) or 'L' (link),\n");
	fprintf(stderr,
		"and id(s) is either one unsigned integer (for nodes) or two unsigned integers (for links), representing the id(s) "
		"of the "
		"element(s) involved in the event.\n");
	fprintf(stderr, "Check provided examples in data/examples to see correct formatting.\n");
}

size_t LinkIdMap_hash(const LinkIdMap* key) {
	return key->nodes[0] + key->nodes[1];
}

bool LinkIdMap_equals(const LinkIdMap* map1, const LinkIdMap* map2) {
	return map1->nodes[0] == map2->nodes[0] && map1->nodes[1] == map2->nodes[1];
}

NO_FREE(LinkIdMap)
DefineArrayList(LinkIdMap);
DefineArrayListDeriveRemove(LinkIdMap);
DefineHashset(LinkIdMap);
DefineHashsetDeriveRemove(LinkIdMap);

SGA_ParsedEvent SGA_parse_single_event(SGA_ParsingCursor* cursor, SGA_IntervalsSetBuilderArrayList* node_presences,
				       SGA_IntervalsSetBuilderArrayList* link_presences, LinkIdMapHashset* link_id_map,
				       SGA_LinkIdArrayListArrayList* neighbours_of_nodes) {
	SGA_ParsedEvent event;

	// Parse instant
	SGA_ParsingResult result = SGA_ParsingCursor_scan_and_move(cursor, SGA_CODE_HERE, "%zu", &event.instant);
	if (!result.success) {
		fprintf(stderr, "Failed to parse event instant!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		print_event_format();
		exit(1);
	}

	SGA_ParsingCursor_skip_whitespace(cursor);

	// Parse sign
	char sign;
	result = SGA_ParsingCursor_scan_and_move(cursor, SGA_CODE_HERE, "%c", &sign);
	if (!result.success) {
		fprintf(stderr, "Failed to parse event sign!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		print_event_format();
		exit(1);
	}
	if (sign == '+') {
		event.event_kind = Appearance;
	}
	else if (sign == '-') {
		event.event_kind = Disappearance;
	}
	else {
		result = SGA_ParsingResult_error(
		    cursor, String_from_format("Event sign was neither '+' nor '-', got '%c'", sign), SGA_CODE_HERE);
		SGA_ParsingResult_print_error(&result, cursor);
		print_event_format();
		exit(1);
	}

	SGA_ParsingCursor_skip_whitespace(cursor);

	// Parse letter
	char letter;
	result = SGA_ParsingCursor_scan_and_move(cursor, SGA_CODE_HERE, "%c", &letter);
	if (!result.success) {
		fprintf(stderr, "Failed to parse event letter!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		print_event_format();
		exit(1);
	}
	if (letter == 'N') {
		event.elem_kind = Node;
	}
	else if (letter == 'L') {
		event.elem_kind = Link;
	}
	else {
		result = SGA_ParsingResult_error(
		    cursor, String_from_format("Event letter was neither 'N' nor 'L', got '%c'", letter), SGA_CODE_HERE);
		SGA_ParsingResult_print_error(&result, cursor);
		print_event_format();
		exit(1);
	}

	SGA_ParsingCursor_skip_whitespace(cursor);

	// Parse id(s)
	if (event.elem_kind == Node) {
		size_t node_id;
		result = SGA_ParsingCursor_scan_and_move(cursor, SGA_CODE_HERE, "%zu", &node_id);
		if (!result.success) {
			fprintf(stderr, "Failed to parse node id!\n");
			SGA_ParsingResult_print_error(&result, cursor);
			print_event_format();
			exit(1);
		}
		event.id.node = node_id;
	}
	else {
		size_t node1_id, node2_id;
		result = SGA_ParsingCursor_scan_and_move(cursor, SGA_CODE_HERE, "%zu %zu", &node1_id, &node2_id);
		if (!result.success) {
			fprintf(stderr, "Failed to parse link node ids!\n");
			SGA_ParsingResult_print_error(&result, cursor);
			print_event_format();
			exit(1);
		}
		event.id.link.node1 = node1_id;
		event.id.link.node2 = node2_id;
	}

	// Update presence intervals
	if (event.elem_kind == Node) {
		// Extend the node presences and neighbours array if needed
		while (event.id.node >= node_presences->length) {
			SGA_IntervalsSetBuilder builder = SGA_IntervalsSetBuilder_new();
			SGA_IntervalsSetBuilderArrayList_push(node_presences, builder);
			SGA_LinkIdArrayListArrayList_push(neighbours_of_nodes, SGA_LinkIdArrayList_new());
		}

		SGA_IntervalsSetBuilderError err;
		if (event.event_kind == Appearance) {
			err = SGA_IntervalsSetBuilder_add_appearance(&node_presences->array[event.id.node], event.instant);
		}
		else {
			err = SGA_IntervalsSetBuilder_add_disappearance(&node_presences->array[event.id.node], event.instant);
		}
		if (err.type != None) {
			String err_msg = SGA_IntervalsSetBuilderError_to_string(&err);

			result = SGA_ParsingResult_error(
			    cursor,
			    String_from_format("Error while updating presence intervals for node %zu : %s", event.id.node, err_msg.data),
			    SGA_CODE_HERE);

			SGA_ParsingResult_print_error(&result, cursor);
			String_destroy(err_msg);
			exit(1);
		}
	}
	else {
		// Add the link to the link id map if it doesn't exist yet
		LinkIdMap link_key		= {.nodes = {event.id.link.node1, event.id.link.node2}};
		const LinkIdMap* existing_entry = LinkIdMapHashset_find(*link_id_map, link_key);
		SGA_LinkId link_id;
		if (existing_entry == NULL) {
			LinkIdMap link_map_entry = {.nodes = {event.id.link.node1, event.id.link.node2},
						    .id	   = LinkIdMapHashset_nb_elems(link_id_map)};
			LinkIdMapHashset_insert(link_id_map, link_map_entry);
			link_id = link_map_entry.id;

			// Extend the neighbours_of_nodes structure
			while (event.id.link.node1 >= neighbours_of_nodes->length) {
				SGA_LinkIdArrayListArrayList_push(neighbours_of_nodes, SGA_LinkIdArrayList_new());
			}
			while (event.id.link.node2 >= neighbours_of_nodes->length) {
				SGA_LinkIdArrayListArrayList_push(neighbours_of_nodes, SGA_LinkIdArrayList_new());
			}

			// Add the link id to the neighbours of both nodes
			SGA_LinkIdArrayList_push(&neighbours_of_nodes->array[event.id.link.node1], link_id);
			SGA_LinkIdArrayList_push(&neighbours_of_nodes->array[event.id.link.node2], link_id);
		}
		else {
			link_id = existing_entry->id;
		}

		// Extend the link presences array if needed
		while (link_id >= link_presences->length) {
			SGA_IntervalsSetBuilder builder = SGA_IntervalsSetBuilder_new();
			SGA_IntervalsSetBuilderArrayList_push(link_presences, builder);
		}

		SGA_IntervalsSetBuilderError err;
		if (event.event_kind == Appearance) {
			err = SGA_IntervalsSetBuilder_add_appearance(&link_presences->array[link_id], event.instant);
		}
		else {
			err = SGA_IntervalsSetBuilder_add_disappearance(&link_presences->array[link_id], event.instant);
		}
		if (err.type != None) {
			String builder_err_msg = SGA_IntervalsSetBuilderError_to_string(&err);
			String err_msg	       = String_from_format(
			    "Error while updating presence intervals for link (%zu, %zu) : ", event.id.link.node1, event.id.link.node2);
			String_concat_consume(&err_msg, builder_err_msg);
			result = SGA_ParsingResult_error(cursor, err_msg, SGA_CODE_HERE);
			SGA_ParsingResult_print_error(&result, cursor);
			exit(1);
		}
	}

	return event;
}

int SGA_ParsedEvent_compare(SGA_ParsedEvent* a, SGA_ParsedEvent* b) {
	if (a->instant < b->instant) {
		return -1;
	}
	if (a->instant > b->instant) {
		return 1;
	}
	return 0;
}

DefineArrayList(SGA_ParsedEvent);
DefineArrayListDeriveOrdered(SGA_ParsedEvent);

SGA_ParsedEventArrayList SGA_parse_events(SGA_ParsingCursor* cursor, SGA_IntervalsSetBuilderArrayList* node_presences,
					  SGA_IntervalsSetBuilderArrayList* link_presences, LinkIdMapHashset* link_id_map,
					  SGA_LinkIdArrayListArrayList* neighbours_of_nodes) {
	SGA_ParsedEventArrayList events = SGA_ParsedEventArrayList_new();

	while (true) {
		// Check for next section
		if (SGA_ParsingCursor_line_starts_with(cursor, "[end]")) {
			break;
		}
		if (SGA_ParsingCursor_line_starts_with(cursor, "[weights]")) {
			break;
		}

		// Skip empty lines
		if (SGA_ParsingCursor_line_is_empty(cursor)) {
			cursor->cursor++;
			continue;
		}

		// Parse the event and add it to the list
		SGA_ParsedEvent event = SGA_parse_single_event(cursor, node_presences, link_presences, link_id_map, neighbours_of_nodes);
		SGA_ParsedEventArrayList_push(&events, event);
		SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);
		SGA_ParsingCursor_skip_whitespace(cursor);
	}

	SGA_ParsedEventArrayList_sort_unstable(&events);

	return events;
}
