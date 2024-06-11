#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interval.h"
#include "stream_graph.h"
#include "units.h"
#include "utils.h"

char* get_to_header(const char* str, const char* header) {
	if (str == NULL) {
		fprintf(stderr, "Could not find header %s because the string is NULL\n", header);
		exit(1);
	}
	// check if current char is EOF
	if (*str == '\0') {
		fprintf(stderr,
				"Could not find header %s because" TEXT_RED TEXT_BOLD " the end of the file "
				"was reached\n" TEXT_RESET,
				header);
		exit(1);
	}
	char* str2 = strstr(str, header);
	if (str2 == NULL) {
		fprintf(stderr, "Could not find header %s\n", header);
		exit(1);
	}
	str2 = strchr(str2, '\n') + 1;
	return str2;
}

// TODO : turn these into functions
#define NEXT_HEADER(section)                                                                       \
	current_header = "" #section "";                                                               \
	(str) = get_to_header(str, current_header);

#define EXPECTED_NB_SCANNED(expected)                                                              \
	if (nb_scanned != (expected)) {                                                                \
		fprintf(stderr,                                                                            \
				TEXT_RED TEXT_BOLD "Could not parse the header " TEXT_RESET                        \
								   "%s\nError at line %d\n",                                       \
				current_header, __LINE__);                                                         \
		fprintf(stderr, "Number of scanned elements: %d, expected: %d\n", nb_scanned, expected);   \
		fprintf(stderr, "str: %s\n", str);                                                         \
		exit(1);                                                                                   \
	}

#define GO_TO_NEXT_LINE(str)                                                                       \
	if (*(str) != '\0') {                                                                          \
		(str) = strchr((str), '\n');                                                               \
		if ((str) == NULL) {                                                                       \
			fprintf(stderr, TEXT_BOLD TEXT_RED "End of file was reached prematurely while trying " \
											   "to find next line \n" TEXT_RESET);                 \
			exit(1);                                                                               \
		}                                                                                          \
		(str)++;                                                                                   \
	}                                                                                              \
	else {                                                                                         \
		fprintf(stderr, "End of file was reached prematurely while trying to find next line \n");  \
		exit(1);                                                                                   \
	}

#define PRINT_LINE(str)                                                                            \
	{                                                                                              \
		char* end = strchr(str, '\n');                                                             \
		char* line = (char*)malloc(end - (str) + 1);                                               \
		strncpy(line, str, end - (str));                                                           \
		line[end - (str)] = '\0';                                                                  \
		printf("line: %s\n", line);                                                                \
		free(line);                                                                                \
	}

#define NEXT_TUPLE(str)                                                                            \
	{                                                                                              \
		if (*(str) == '\0') {                                                                      \
			fprintf(stderr, TEXT_RED TEXT_BOLD "End of file was reached prematurely while trying " \
											   "to find next tuple \n" TEXT_RESET);                \
			exit(1);                                                                               \
		}                                                                                          \
                                                                                                   \
		(str)++;                                                                                   \
		while ((*(str) != '(') && (*(str) != '\n') && (*(str) != '\0')) {                          \
			(str)++;                                                                               \
		}                                                                                          \
	}

// TODO : Make the code better and less unreadable copy pasted code
// TODO : Add field for precision loss
StreamGraph SGA_StreamGraph_from_string(const char* str) {

	StreamGraph sg;
	int nb_scanned;
	char* current_header;

	// Skip first line (version control)

	NEXT_HEADER([General]);
	size_t lifespan_start;
	size_t lifespan_end;
	nb_scanned = sscanf(str, "Lifespan=(%zu %zu)\n", &lifespan_start, &lifespan_end);
	EXPECTED_NB_SCANNED(2);
	GO_TO_NEXT_LINE(str);
	// next line should be Named=true or Named=false

	bool named;
	if (strncmp(str, "Named=true\n", 10) == 0) {
		named = true;
	}
	else if (strncmp(str, "Named=false\n", 11) == 0) {
		named = false;
	}
	else {
		fprintf(stderr, "Could not parse the header %s", current_header);
		exit(1);
	}
	GO_TO_NEXT_LINE(str);

	// Parse the Memory section
	NEXT_HEADER([Memory]);
	// Count how many slices are needed
	size_t relative_max = (RelativeMoment)~0;
	size_t nb_slices = (lifespan_end / relative_max) + 1;

	// Parse the memory header
	size_t nb_nodes;
	nb_scanned = sscanf(str, "NumberOfNodes=%zu\n", &nb_nodes);
	EXPECTED_NB_SCANNED(1);
	GO_TO_NEXT_LINE(str);
	size_t nb_links;
	nb_scanned = sscanf(str, "NumberOfLinks=%zu\n", &nb_links);
	EXPECTED_NB_SCANNED(1);
	GO_TO_NEXT_LINE(str);
	size_t nb_regular_key_moments;
	nb_scanned = sscanf(str, "RegularKeyMoments=%zu\n", &nb_regular_key_moments);
	EXPECTED_NB_SCANNED(1);
	GO_TO_NEXT_LINE(str);
	size_t nb_removal_only_moments;
	nb_scanned = sscanf(str, "RemovalOnlyMoments=%zu\n", &nb_removal_only_moments);
	EXPECTED_NB_SCANNED(1);
	GO_TO_NEXT_LINE(str);
	size_t nb_key_moments = nb_regular_key_moments + nb_removal_only_moments + 1;
	size_t* key_moments = (size_t*)malloc(nb_key_moments * sizeof(size_t));
	// Allocate the stream graph
	sg.key_moments = SGA_KeyMomentsTable_alloc(nb_slices);
	sg.nodes = SGA_TemporalNodesSet_alloc(nb_nodes);
	sg.links = SGA_LinksSet_alloc(nb_links);
	sg.events = SGA_EventsTable_alloc(nb_regular_key_moments, nb_removal_only_moments);

	if (named) {
		sg.node_names = (const char**)malloc(nb_nodes * sizeof(const char*));
	}
	else {
		sg.node_names = NULL;
	}

	// Parse the memory needed for the nodes
	NEXT_HEADER([[Nodes]]);
	NEXT_HEADER([[[NumberOfNeighbours]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		// Parse the node
		size_t nb_neighbours;
		nb_scanned = sscanf(str, "%zu\n", &nb_neighbours);
		EXPECTED_NB_SCANNED(1);
		GO_TO_NEXT_LINE(str);
		// Allocate the neighbours
		sg.nodes.nodes[node].nb_neighbours = nb_neighbours;
		sg.nodes.nodes[node].neighbours = (EdgeId*)malloc(nb_neighbours * sizeof(EdgeId));
	}

	NEXT_HEADER([[[NumberOfIntervals]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		// Parse the node
		size_t nb_intervals;
		nb_scanned = sscanf(str, "%zu\n", &nb_intervals);
		EXPECTED_NB_SCANNED(1);
		GO_TO_NEXT_LINE(str);
		// Allocate the intervals
		sg.nodes.nodes[node].nb_intervals = nb_intervals;
		sg.nodes.nodes[node].present_at = (Interval*)malloc(nb_intervals * sizeof(Interval));
	}

	NEXT_HEADER([[Links]]);
	NEXT_HEADER([[[NumberOfIntervals]]]);
	for (size_t link = 0; link < nb_links; link++) {
		// Parse the edge
		size_t nb_intervals;
		nb_scanned = sscanf(str, "%zu\n", &nb_intervals);
		EXPECTED_NB_SCANNED(1);
		GO_TO_NEXT_LINE(str);
		// Allocate the intervals
		sg.links.links[link].nb_intervals = nb_intervals;
		sg.links.links[link].present_at = (Interval*)malloc(nb_intervals * sizeof(Interval));
	}

	NEXT_HEADER([[KeyMoments]]);
	NEXT_HEADER([[[NumberOfEvents]]]);
	size_t* nb_events_per_key_moment = (size_t*)malloc(nb_key_moments * sizeof(size_t));
	for (size_t key_moment = 0; key_moment < nb_key_moments; key_moment++) {
		// Parse the key moment
		size_t nb_events;
		nb_scanned = sscanf(str, "%zu\n", &nb_events);
		EXPECTED_NB_SCANNED(1);
		GO_TO_NEXT_LINE(str);
		// Allocate the events
		sg.events.events[key_moment] = malloc((nb_events + 2) * sizeof(size_t));
		nb_events_per_key_moment[key_moment] = nb_events;
	}

	NEXT_HEADER([[[NumberOfSlices]]]);
	size_t moments_in_slice;
	for (size_t i = 0; i < nb_slices; i++) {
		nb_scanned = sscanf(str, "%zu\n", &moments_in_slice);
		EXPECTED_NB_SCANNED(1);
		SGA_KeyMomentsTable_alloc_slice(&sg.key_moments, i, moments_in_slice);
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([Data]);
	NEXT_HEADER([[Neighbours]]);

	NEXT_HEADER([[[NodesToLinks]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		str = strchr(str, '(') + 1;
		for (size_t j = 0; j < sg.nodes.nodes[node].nb_neighbours; j++) {
			size_t link;
			nb_scanned = sscanf(str, "%zu", &link);
			EXPECTED_NB_SCANNED(1);
			sg.nodes.nodes[node].neighbours[j] = link;
			while ((*str != '(') && (*str != ' ') && (*str != '\n')) {
				str++;
			}
		}
	}

	NEXT_HEADER([[[LinksToNodes]]]);
	for (size_t link = 0; link < nb_links; link++) {
		size_t node1, node2;
		sscanf(str, "(%zu %zu)", &node1, &node2);
		sg.links.links[link].nodes[0] = node1;
		sg.links.links[link].nodes[1] = node2;
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([[Events]]);
	// Parse the first one
	size_t key_moment;
	nb_scanned = sscanf(str, "%zu=", &key_moment);
	EXPECTED_NB_SCANNED(1);
	str = strchr(str, '(') + 1;
	SGA_KeyMomentsTable_push_in_order(&sg.key_moments, key_moment);
	// Parse all the node events
	size_t j = 0;
	for (; j < nb_events_per_key_moment[0]; j++) {
		char type;
		size_t node;
		nb_scanned = sscanf(str, "(%c %zu)", &type, &node);
		EXPECTED_NB_SCANNED(2);
		NEXT_TUPLE(str);
		*SGA_Event_access_nth_node(sg.events.events[0], j) = node;
	}
	*SGA_Event_access_nb_nodes(sg.events.events[0]) = j;

	size_t k;
	for (k = j; k < nb_events_per_key_moment[0]; k++) {
		char type;
		size_t link;
		nb_scanned = sscanf(str, "(%c %zu)", &type, &link);
		EXPECTED_NB_SCANNED(2);
		NEXT_TUPLE(str);
		*SGA_Event_access_nth_link(sg.events.events[0], k - j) = link;
	}
	*SGA_Event_access_nb_links(sg.events.events[0]) = k - j;
	key_moments[0] = key_moment;

	NEXT_HEADER([[[Regular]]]);
	for (size_t i = 1; i < nb_regular_key_moments + 1; i++) {
		// Parse the key moment
		size_t key_moment;
		char type;
		nb_scanned = sscanf(str, "%zu=(%c", &key_moment, &type);
		EXPECTED_NB_SCANNED(2);
		str = strchr(str, '=') + sizeof("(x ");
		if (type == '-') {
			SGA_BitArray_set_zero(sg.events.presence_mask, i - 1);
		}
		SGA_KeyMomentsTable_push_in_order(&sg.key_moments, key_moment);
		//  Parse all the node events
		size_t j = 0;
		for (; j < nb_events_per_key_moment[i]; j++) {
			char node_or_link;
			size_t id;
			nb_scanned = sscanf(str, "(%c %zu)", &node_or_link, &id);
			if (node_or_link == 'L') {
				goto parse_links;
			}
			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			*SGA_Event_access_nth_node(sg.events.events[i], j) = id;
		}
	parse_links:
		*SGA_Event_access_nb_nodes(sg.events.events[i]) = j;
		size_t k;
		for (k = j; k < nb_events_per_key_moment[i]; k++) {
			char link;
			size_t id;
			nb_scanned = sscanf(str, "(%c %zu)", &link, &id);

			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			*SGA_Event_access_nth_link(sg.events.events[i], k - j) = id;
		}
		*SGA_Event_access_nb_links(sg.events.events[i]) = k - j;
		key_moments[i] = key_moment;
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([[[RemovalOnly]]]);
	for (size_t i = nb_regular_key_moments + 1; i < nb_key_moments; i++) {
		// Parse the key moment
		size_t key_moment;
		nb_scanned = sscanf(str, "%zu=", &key_moment);
		EXPECTED_NB_SCANNED(1);
		str = strchr(str, '(') + 1;
		SGA_KeyMomentsTable_push_in_order(&sg.key_moments, key_moment);
		//  Parse all the node events
		size_t j = 0;
		for (; j < nb_events_per_key_moment[i]; j++) {
			char node_or_link;
			size_t id;
			nb_scanned = sscanf(str, "(%c %zu)", &node_or_link, &id);
			EXPECTED_NB_SCANNED(2);
			if (node_or_link == 'L') {
				goto parse_links_removal;
			}
			NEXT_TUPLE(str);
			*SGA_Event_access_nth_node(sg.events.events[i], j) = id;
		}
	parse_links_removal:
		*SGA_Event_access_nb_nodes(sg.events.events[i]) = j;
		size_t k;
		for (k = j; k < nb_events_per_key_moment[i]; k++) {
			char link;
			size_t id;
			nb_scanned = sscanf(str, "(%c %zu)", &link, &id);
			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			*SGA_Event_access_nth_link(sg.events.events[i], k - j) = id;
		}
		*SGA_Event_access_nb_links(sg.events.events[i]) = k - j;
		key_moments[i] = key_moment;
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([[PresenceIntervals]]);
	NEXT_HEADER([[[Nodes]]]);
	// Parse the presence intervals
	for (size_t node = 0; node < nb_nodes; node++) {
		str++;
		for (size_t i = 0; i < sg.nodes.nodes[node].nb_intervals; i++) {
			size_t start;
			size_t end;
			nb_scanned = sscanf(str, "(%zu %zu)", &start, &end);
			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			sg.nodes.nodes[node].present_at[i].start = start;
			sg.nodes.nodes[node].present_at[i].end = end;
		}
		GO_TO_NEXT_LINE(str);
	}
	NEXT_HEADER([[[Links]]]);
	for (size_t link = 0; link < nb_links; link++) {
		str++;
		for (size_t i = 0; i < sg.links.links[link].nb_intervals; i++) {
			size_t start;
			size_t end;
			nb_scanned = sscanf(str, "(%zu %zu)", &start, &end);
			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			sg.links.links[link].present_at[i].start = start;
			sg.links.links[link].present_at[i].end = end;
		}
		GO_TO_NEXT_LINE(str);
	}

	// Parse the names
	if (named) {
		NEXT_HEADER([Names]);
		for (size_t node = 0; node < nb_nodes; node++) {
			char name[256];
			nb_scanned = sscanf(str, "%255s\n", name);
			EXPECTED_NB_SCANNED(1);
			sg.node_names[node] = strdup(name);
			GO_TO_NEXT_LINE(str);
		}
	}

	NEXT_HEADER([EndOfFile]);

	free(nb_events_per_key_moment);
	free(key_moments);

	return sg;
}

StreamGraph SGA_StreamGraph_from_file(const char* filename) {
	// Load the file
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Could not open file %s\n", filename);
		exit(1);
	}

	// Read the whole file
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	char* buffer = (char*)malloc(size + 2);
	if (buffer == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	fread(buffer, 1, size, file);
	buffer[size] = '\0';
	buffer[size + 1] = '\0';
	fclose(file);

	// Parse the stream graph
	StreamGraph sg = SGA_StreamGraph_from_string(buffer);
	free(buffer);
	return sg;
}

#include "vector.h"
DEFAULT_COMPARE(char)
DEFAULT_TO_STRING(char, "%c")
#define APPEND_CONST(str) str, sizeof(str) - 1
DefVector(char, NO_FREE(char));

void append_node_name(charVector* vec, size_t node_idx, const char** node_names) {
	if (node_names == NULL) {
		char name[50];
		sprintf(name, "%zu", node_idx);
		charVector_append(vec, name, strlen(name));
	}
	else {
		const char* name = node_names[node_idx];
		charVector_append(vec, name, strlen(name));
	}
}

size_t SGA_StreamGraph_lifespan_begin(StreamGraph* sg) {
	return SGA_KeyMomentsTable_first_moment(&sg->key_moments);
}

size_t SGA_StreamGraph_lifespan_end(StreamGraph* sg) {
	return SGA_KeyMomentsTable_last_moment(&sg->key_moments);
}

char* SGA_TemporalNode_to_string(StreamGraph* sg, size_t node_idx) {
	charVector vec = charVector_new();
	TemporalNode* node = &sg->nodes.nodes[node_idx];
	charVector_append(&vec, APPEND_CONST("\tNode "));
	append_node_name(&vec, node_idx, sg->node_names);
	charVector_append(&vec, APPEND_CONST(" {\n\t\tIntervals=[\n"));
	// Append first interval
	char interval[100];
	sprintf(interval, "\t\t\t(%zu %zu)", node->present_at[0].start, node->present_at[0].end);
	charVector_append(&vec, interval, strlen(interval));
	// Append the other intervals
	for (size_t i = 1; i < node->nb_intervals; i++) {
		char interval[100];
		sprintf(interval, " U (%zu %zu)", node->present_at[i].start, node->present_at[i].end);
		charVector_append(&vec, interval, strlen(interval));
	}
	charVector_append(&vec, APPEND_CONST("\n"));
	charVector_append(&vec, APPEND_CONST("\t\t]\n"));
	charVector_append(&vec, APPEND_CONST("\t\tNeighbours={\n\t\t\t"));
	for (size_t i = 0; i < node->nb_neighbours; i++) {
		// get the names of the neighbours through the links
		size_t link_idx = node->neighbours[i];
		size_t node1 = sg->links.links[link_idx].nodes[0];
		size_t node2 = sg->links.links[link_idx].nodes[1];
		size_t neighbour_idx = (node1 == node_idx) ? node2 : node1;
		append_node_name(&vec, neighbour_idx, sg->node_names);
	}
	charVector_append(&vec, APPEND_CONST("\n\t\t}\n\n\t}\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

char* SGA_Link_to_string(StreamGraph* sg, size_t link_idx) {
	charVector vec = charVector_new();
	const char** node_names = sg->node_names;
	Link* link = &sg->links.links[link_idx];
	charVector_append(&vec, APPEND_CONST("\tLink ("));
	append_node_name(&vec, link->nodes[0], node_names);
	charVector_append(&vec, APPEND_CONST(" "));
	append_node_name(&vec, link->nodes[1], node_names);
	charVector_append(&vec, APPEND_CONST(") {\n\t\tIntervals=[\n"));
	// Append first interval
	char interval[100];
	sprintf(interval, "\t\t\t(%zu %zu)", link->present_at[0].start, link->present_at[0].end);
	charVector_append(&vec, interval, strlen(interval));
	// Append the other intervals
	for (size_t i = 1; i < link->nb_intervals; i++) {
		char interval[100];
		sprintf(interval, " U (%zu %zu)", link->present_at[i].start, link->present_at[i].end);
		charVector_append(&vec, interval, strlen(interval));
	}
	charVector_append(&vec, APPEND_CONST("\n"));
	charVector_append(&vec, APPEND_CONST("\t\t]\n\n\t}\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

char* SGA_Event_to_string(StreamGraph* sg, size_t event_idx) {
	Event event = sg->events.events[event_idx];
	charVector vec = charVector_new();
	charVector_append(&vec, APPEND_CONST("\t"));

	char buffer[100];
	buffer[0] = '\0';
	char letter_event;
	if (event_idx == 0) {
		letter_event = 'V';
	}
	// if less than the deletion index
	else if (event_idx < sg->events.disappearance_index) {
		if (SGA_BitArray_is_one(sg->events.presence_mask, event_idx - 1)) {
			letter_event = '+';
		}
		else {
			letter_event = '-';
		}
	}
	else {
		letter_event = 'X';
	}

	sprintf(buffer, "%zu = %c ", SGA_KeyMomentsTable_nth_key_moment(&sg->key_moments, event_idx),
			letter_event);
	charVector_append(&vec, buffer, strlen(buffer));
	charVector_append(&vec, APPEND_CONST("( Nodes : "));
	for (size_t i = 0; i < *SGA_Event_access_nb_nodes(event); i++) {
		append_node_name(&vec, *SGA_Event_access_nth_node(event, i), sg->node_names);
		charVector_append(&vec, APPEND_CONST(" "));
	}
	charVector_append(&vec, APPEND_CONST("| Links : "));
	for (size_t i = 0; i < *SGA_Event_access_nb_links(event); i++) {
		// TODO : modify this to use the node names
		if (sg->node_names != NULL) {
			Link* link = &sg->links.links[*SGA_Event_access_nth_link(event, i)];
			sprintf(buffer, "%s-%s ", sg->node_names[link->nodes[0]],
					sg->node_names[link->nodes[1]]);
		}
		else {
			sprintf(buffer, "%zu ", *SGA_Event_access_nth_link(event, i));
		}
		charVector_append(&vec, buffer, strlen(buffer));
	}
	charVector_append(&vec, APPEND_CONST(")\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

char* SGA_EventsTable_to_string(StreamGraph* sg) {
	charVector vec = charVector_new();
	charVector_append(&vec, APPEND_CONST("EventsTable {\n"));
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		char* event_str = SGA_Event_to_string(sg, i);
		charVector_append(&vec, event_str, strlen(event_str));
		free(event_str);
	}
	charVector_append(&vec, APPEND_CONST("}\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

char* SGA_StreamGraph_to_string(StreamGraph* sg) {
	charVector vec = charVector_new();
	charVector_append(&vec, APPEND_CONST("StreamGraph {\n"));
	// Append the lifespan
	charVector_append(&vec, APPEND_CONST("\tLifespan="));
	char* lifespan = (char*)malloc(100);
	sprintf(lifespan, "(%zu %zu)", SGA_StreamGraph_lifespan_begin(sg),
			SGA_StreamGraph_lifespan_end(sg));
	charVector_append(&vec, lifespan, strlen(lifespan));
	free(lifespan);

	// tostring the nodes
	charVector_append(&vec, APPEND_CONST("\n\tNodes=[\n"));
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		char* node_str = SGA_TemporalNode_to_string(sg, i);
		charVector_append(&vec, node_str, strlen(node_str));
		free(node_str);
	}

	// tostring the links
	charVector_append(&vec, APPEND_CONST("\n\tLinks=[\n"));
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		char* link_str = SGA_Link_to_string(sg, i);
		charVector_append(&vec, link_str, strlen(link_str));
		free(link_str);
	}

	// tostring the events
	charVector_append(&vec, APPEND_CONST("\n\tEvents=[\n"));
	char* events_str = SGA_EventsTable_to_string(sg);
	charVector_append(&vec, events_str, strlen(events_str));
	free(events_str);

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

void SGA_StreamGraph_destroy(StreamGraph* sg) {
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		free(sg->nodes.nodes[i].neighbours);
		free(sg->nodes.nodes[i].present_at);
	}
	free(sg->nodes.nodes);
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		free(sg->links.links[i].present_at);
	}
	free(sg->links.links);
	SGA_KeyMomentsTable_destroy(&sg->key_moments);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		free(sg->events.events[i]);
	}
	free(sg->events.events);
	if (sg->node_names != NULL) {
		for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
			free((char*)sg->node_names[i]);
		}
		free(sg->node_names);
	}
	SGA_BitArray_destroy(sg->events.presence_mask);
}