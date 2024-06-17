#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interval.h"
#include "stream_graph.h"
#include "stream_graph/events_table.h"
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
#define NEXT_HEADER(section)                                                                                           \
	current_header = "" #section "";                                                                                   \
	(str) = get_to_header(str, current_header);

#define EXPECTED_NB_SCANNED(expected)                                                                                  \
	if (nb_scanned != (expected)) {                                                                                    \
		fprintf(stderr, TEXT_RED TEXT_BOLD "Could not parse the header " TEXT_RESET "%s\nError at line %d\n",          \
				current_header, __LINE__);                                                                             \
		fprintf(stderr, "Number of scanned elements: %d, expected: %d\n", nb_scanned, expected);                       \
		fprintf(stderr, "str: %s\n", str);                                                                             \
		exit(1);                                                                                                       \
	}

#define GO_TO_NEXT_LINE(str)                                                                                           \
	if (*(str) != '\0') {                                                                                              \
		(str) = strchr((str), '\n');                                                                                   \
		if ((str) == NULL) {                                                                                           \
			fprintf(stderr, TEXT_BOLD TEXT_RED "End of file was reached prematurely while trying "                     \
											   "to find next line \n" TEXT_RESET);                                     \
			exit(1);                                                                                                   \
		}                                                                                                              \
		(str)++;                                                                                                       \
	}                                                                                                                  \
	else {                                                                                                             \
		fprintf(stderr, "End of file was reached prematurely while trying to find next line \n");                      \
		exit(1);                                                                                                       \
	}

#define PRINT_LINE(str)                                                                                                \
	{                                                                                                                  \
		char* end = strchr(str, '\n');                                                                                 \
		char* line = (char*)malloc(end - (str) + 1);                                                                   \
		strncpy(line, str, end - (str));                                                                               \
		line[end - (str)] = '\0';                                                                                      \
		printf("line: %s\n", line);                                                                                    \
		free(line);                                                                                                    \
	}

#define NEXT_TUPLE(str)                                                                                                \
	{                                                                                                                  \
		if (*(str) == '\0') {                                                                                          \
			fprintf(stderr, TEXT_RED TEXT_BOLD "End of file was reached prematurely while trying "                     \
											   "to find next tuple \n" TEXT_RESET);                                    \
			exit(1);                                                                                                   \
		}                                                                                                              \
                                                                                                                       \
		(str)++;                                                                                                       \
		while ((*(str) != '(') && (*(str) != '\n') && (*(str) != '\0')) {                                              \
			(str)++;                                                                                                   \
		}                                                                                                              \
	}

// TODO : Make the code better and less unreadable copy pasted code
/* Format :
SGA Internal version 1.0.0


[General]
Lifespan=(0 100)
Scaling=10


[Memory]
NumberOfNodes=4
NumberOfLinks=4
RegularKeyMoments=8
RemovalOnlyMoments=4

[[Nodes]]
[[[NumberOfNeighbours]]]
2
2
2
1
[[[NumberOfIntervals]]]
1
2
1
1

[[Links]]
[[[NumberOfIntervals]]]
2
1
1
1

[[[NumberOfSlices]]]
13


[Data]

[[Neighbours]]
[[[NodesToLinks]]]
(0 2)
(1 3)
(2 3)
(1)
[[[LinksToNodes]]]
(0 1)
(1 3)
(0 2)
(1 2)

[[Events]]
0=((+ N 0) (+ N 1))
10=((+ N 3) (+ L 0))
20=((+ L 1))
30=((- L 0) (- L 1))
40=((- N 1) (+ N 2))
45=((+ L 2))
50=((+ N 1))
60=((+ L 3))
70=((+ L 0))
75=((- L 2))
80=((- L 0))
90=((- N 2) (- L 3))
100=((- N 0) (- N 1))

[EndOfFile]

[EndOfFile]
*/
StreamGraph StreamGraph_from_string(const char* str) {

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
	size_t scaling;
	nb_scanned = sscanf(str, "Scaling=%zu\n", &scaling);
	sg.scaling = scaling;
	EXPECTED_NB_SCANNED(1);
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
	size_t nb_key_moments;
	nb_scanned = sscanf(str, "NumberOfKeyMoments=%zu\n", &nb_key_moments);
	EXPECTED_NB_SCANNED(1);
	GO_TO_NEXT_LINE(str);

	size_t* key_moments = (size_t*)malloc(nb_key_moments * sizeof(size_t));
	// Allocate the stream graph
	sg.key_moments = KeyMomentsTable_alloc(nb_slices);
	sg.nodes = TemporalNodesSet_alloc(nb_nodes);
	sg.links = LinksSet_alloc(nb_links);

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
		IntervalsSet presence = IntervalsSet_alloc(nb_intervals);
		sg.nodes.nodes[node].presence = presence;
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
		IntervalsSet presence = IntervalsSet_alloc(nb_intervals);
		sg.links.links[link].presence = presence;
	}

	NEXT_HEADER([[[NumberOfSlices]]]);
	size_t moments_in_slice;
	for (size_t i = 0; i < nb_slices; i++) {
		nb_scanned = sscanf(str, "%zu\n", &moments_in_slice);
		EXPECTED_NB_SCANNED(1);
		KeyMomentsTable_alloc_slice(&sg.key_moments, i, moments_in_slice);
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
	// Parse all the tuples afterwards
	int* nb_pushed_for_nodes = malloc(nb_nodes * sizeof(int));
	int* nb_pushed_for_links = malloc(nb_links * sizeof(int));
	for (size_t i = 0; i < nb_nodes; i++) {
		nb_pushed_for_nodes[i] = 0;
	}
	for (size_t i = 0; i < nb_links; i++) {
		nb_pushed_for_links[i] = 0;
	}

	for (size_t i = 0; i < nb_key_moments; i++) {
		size_t key_moment;
		nb_scanned = sscanf(str, "%zu=", &key_moment);
		EXPECTED_NB_SCANNED(1);
		str = strchr(str, '(') + 1;
		KeyMomentsTable_push_in_order(&sg.key_moments, key_moment);
		while (*str != '\n') {
			char letter, sign;
			size_t id;
			nb_scanned = sscanf(str, "(%c %c %zu)", &sign, &letter, &id);
			EXPECTED_NB_SCANNED(3);
			// TODO : refactor this
			if (letter == 'N') {
				if (nb_pushed_for_nodes[id] % 2 == 0) {
					if (sign != '+') {
						fprintf(stderr, "Node %zu added twice without being removed\n", id);
						exit(1);
					}
					nb_pushed_for_nodes[id]++;
					sg.nodes.nodes[id].presence.intervals[nb_pushed_for_nodes[id] / 2].start = key_moment;
				}
				else {
					if (sign != '-') {
						fprintf(stderr, "Node %zu removed twice without being added\n", id);
						exit(1);
					}
					sg.nodes.nodes[id].presence.intervals[nb_pushed_for_nodes[id] / 2].end = key_moment;
					nb_pushed_for_nodes[id]++;
				}
			}
			else if (letter == 'L') {
				if (nb_pushed_for_links[id] % 2 == 0) {
					if (sign != '+') {
						fprintf(stderr, "Link %zu added twice without being removed\n", id);
						exit(1);
					}
					nb_pushed_for_links[id]++;
					sg.links.links[id].presence.intervals[nb_pushed_for_links[id] / 2].start = key_moment;
				}
				else {
					if (sign != '-') {
						fprintf(stderr, "Link %zu removed twice without being added\n", id);
						exit(1);
					}
					sg.links.links[id].presence.intervals[nb_pushed_for_links[id] / 2].end = key_moment;
					nb_pushed_for_links[id]++;
				}
			}
			else {
				fprintf(stderr, "Could not parse the letter %c\n", letter);
				exit(1);
			}
			NEXT_TUPLE(str);
		}
		GO_TO_NEXT_LINE(str);
	}

	sg.events.nb_events = nb_key_moments;

	// printf("nb_key_moments: %zu\n", nb_key_moments);
	// printf("nb_events: %zu\n", sg.events.nb_events);

	NEXT_HEADER([EndOfFile]);

	free(key_moments);
	free(nb_pushed_for_nodes);
	free(nb_pushed_for_links);

	return sg;
}

StreamGraph StreamGraph_from_file(const char* filename) {
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
	StreamGraph sg = StreamGraph_from_string(buffer);
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

size_t StreamGraph_lifespan_begin(StreamGraph* sg) {
	return KeyMomentsTable_first_moment(&sg->key_moments);
}

size_t StreamGraph_lifespan_end(StreamGraph* sg) {
	return KeyMomentsTable_last_moment(&sg->key_moments);
}

char* TemporalNode_to_string(StreamGraph* sg, size_t node_idx) {
	charVector vec = charVector_new();
	TemporalNode* node = &sg->nodes.nodes[node_idx];
	charVector_append(&vec, APPEND_CONST("\tNode "));
	char number[50];
	sprintf(number, "%zu", node_idx);
	charVector_append(&vec, number, strlen(number));
	charVector_append(&vec, APPEND_CONST(" {\n\t\tIntervals=[\n"));
	// Append first interval
	char interval[100];
	sprintf(interval, "\t\t\t(%zu %zu)", node->presence.intervals[0].start, node->presence.intervals[0].end);
	charVector_append(&vec, interval, strlen(interval));
	// Append the other intervals
	for (size_t i = 1; i < node->presence.nb_intervals; i++) {
		char interval[100];
		sprintf(interval, " U (%zu %zu)", node->presence.intervals[i].start, node->presence.intervals[i].end);
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
		sprintf(number, "%zu", neighbour_idx);
		charVector_append(&vec, number, strlen(number));
		charVector_append(&vec, APPEND_CONST(" "));
	}
	charVector_append(&vec, APPEND_CONST("\n\t\t}\n\n\t}\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

char* Link_to_string(StreamGraph* sg, size_t link_idx) {
	charVector vec = charVector_new();
	char number[50];
	Link* link = &sg->links.links[link_idx];
	charVector_append(&vec, APPEND_CONST("\tLink"));
	sprintf(number, " %zu (", link_idx);
	charVector_append(&vec, number, strlen(number));
	sprintf(number, "%zu", link->nodes[0]);
	charVector_append(&vec, number, strlen(number));
	charVector_append(&vec, APPEND_CONST(" "));
	sprintf(number, "%zu", link->nodes[1]);
	charVector_append(&vec, number, strlen(number));
	charVector_append(&vec, APPEND_CONST(") {\n\t\tIntervals=[\n"));
	// Append first interval
	char interval[100];
	sprintf(interval, "\t\t\t(%zu %zu)", link->presence.intervals[0].start, link->presence.intervals[0].end);
	charVector_append(&vec, interval, strlen(interval));
	// Append the other intervals
	for (size_t i = 1; i < link->presence.nb_intervals; i++) {
		char interval[100];
		sprintf(interval, " U (%zu %zu)", link->presence.intervals[i].start, link->presence.intervals[i].end);
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

/*char* Event_to_string(StreamGraph* sg, size_t event_idx) {
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
		if (BitArray_is_one(sg->events.presence_mask, event_idx - 1)) {
			letter_event = '+';
		}
		else {
			letter_event = '-';
		}
	}
	else {
		letter_event = 'X';
	}

	sprintf(buffer, "%zu = %c ", KeyMomentsTable_nth_key_moment(&sg->key_moments, event_idx), letter_event);
	charVector_append(&vec, buffer, strlen(buffer));
	charVector_append(&vec, APPEND_CONST("( Nodes : "));
	for (size_t i = 0; i < event.nb_links; i++) {
		sprintf(buffer, "%zu ", *Event_access_nth_node(event, i));
		charVector_append(&vec, buffer, strlen(buffer));
		charVector_append(&vec, APPEND_CONST(" "));
	}
	charVector_append(&vec, APPEND_CONST("| Links : "));
	for (size_t i = 0; i < event.nb_links; i++) {
		// TODO : modify this to use the node names
		sprintf(buffer, "%zu ", *Event_access_nth_link(event, i));
		charVector_append(&vec, buffer, strlen(buffer));
	}
	charVector_append(&vec, APPEND_CONST(")\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

char* EventsTable_to_string(StreamGraph* sg) {
	charVector vec = charVector_new();
	charVector_append(&vec, APPEND_CONST("EventsTable {\n"));
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		char* event_str = Event_to_string(sg, i);
		charVector_append(&vec, event_str, strlen(event_str));
		free(event_str);
	}
	charVector_append(&vec, APPEND_CONST("}\n"));

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}*/

char* StreamGraph_to_string(StreamGraph* sg) {
	charVector vec = charVector_new();
	charVector_append(&vec, APPEND_CONST("StreamGraph {\n"));
	// Append the lifespan
	charVector_append(&vec, APPEND_CONST("\tLifespan="));
	char* lifespan = (char*)malloc(100);
	sprintf(lifespan, "(%zu %zu)", StreamGraph_lifespan_begin(sg), StreamGraph_lifespan_end(sg));
	charVector_append(&vec, lifespan, strlen(lifespan));
	free(lifespan);

	// tostring the nodes
	charVector_append(&vec, APPEND_CONST("\n\tNodes=[\n"));
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		char* node_str = TemporalNode_to_string(sg, i);
		charVector_append(&vec, node_str, strlen(node_str));
		free(node_str);
	}

	// tostring the links
	charVector_append(&vec, APPEND_CONST("\n\tLinks=[\n"));
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		char* link_str = Link_to_string(sg, i);
		charVector_append(&vec, link_str, strlen(link_str));
		free(link_str);
	}

	// tostring the events
	/*charVector_append(&vec, APPEND_CONST("\n\tEvents=[\n"));
	char* events_str = EventsTable_to_string(sg);
	charVector_append(&vec, events_str, strlen(events_str));
	free(events_str);*/

	char* final_str = (char*)malloc((vec.size + 1) * sizeof(char));
	memcpy(final_str, vec.array, vec.size);
	final_str[vec.size] = '\0';
	free(vec.array);
	return final_str;
}

void StreamGraph_destroy(StreamGraph* sg) {
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		free(sg->nodes.nodes[i].neighbours);
		free(sg->nodes.nodes[i].presence.intervals);
	}
	free(sg->nodes.nodes);
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		free(sg->links.links[i].presence.intervals);
	}
	free(sg->links.links);
	KeyMomentsTable_destroy(&sg->key_moments);

	// Free the events if they were initialized
}

#include "vector.h"
DEFAULT_COMPARE(size_t)
DEFAULT_MIN_MAX(size_t)
DEFAULT_TO_STRING(size_t, "%zu")
DefVector(size_t, NO_FREE(size_t));

void events_table_write(StreamGraph* sg, size_tVector* node_events, size_tVector* link_events) {
	// Realloc the vectors into the events
	sg->events.node_events.events = MALLOC(sizeof(Event) * sg->events.nb_events);
	sg->events.link_events.events = MALLOC(sizeof(Event) * sg->events.nb_events);

	// Write the events
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		Event node_event;
		node_event.nb_info = node_events[i].size;
		if (node_event.nb_info == 0) {
			node_event.events = NULL;
		}
		else {
			node_event.events = MALLOC(sizeof(size_t) * node_event.nb_info);
			for (size_t j = 0; j < node_event.nb_info; j++) {
				node_event.events[j] = node_events[i].array[j];
			}
		}
		sg->events.node_events.events[i] = node_event;

		Event link_event;
		link_event.nb_info = link_events[i].size;
		if (link_event.nb_info == 0) {
			link_event.events = NULL;
		}
		else {
			link_event.events = MALLOC(sizeof(size_t) * link_event.nb_info);
			for (size_t j = 0; j < link_event.nb_info; j++) {
				link_event.events[j] = link_events[i].array[j];
			}
		}
		sg->events.link_events.events[i] = link_event;
	}
}

// TODO : refactor this because the code for nodes and links is the same
// TODO : probably not very efficient either (presence mask propagation lookup is slow)
void init_events_table(StreamGraph* sg) {
	// only the number of events is known and node and link presence intervals are known

	// Find the index of the last time a node appears
	size_t last_node_addition = 0;
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		size_t last_node_time = IntervalsSet_last(&sg->nodes.nodes[i].presence).start;
		if (last_node_time > last_node_addition) {
			last_node_addition = last_node_time;
		}
	}

	size_t last_link_addition = 0;
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		size_t last_link_time = IntervalsSet_last(&sg->links.links[i].presence).start;
		if (last_link_time > last_link_addition) {
			last_link_addition = last_link_time;
		}
	}

	size_t index_of_last_node_addition = KeyMomentsTable_find_time_index(&sg->key_moments, last_node_addition);
	size_t index_of_last_link_addition = KeyMomentsTable_find_time_index(&sg->key_moments, last_link_addition);

	// printf("last_node_addition: %zu\n", last_node_addition);
	// printf("last_link_addition: %zu\n", last_link_addition);
	// printf("index_of_last_node_addition: %zu\n", index_of_last_node_addition);
	// printf("index_of_last_link_addition: %zu\n", index_of_last_link_addition);

	sg->events.node_events.disappearance_index = index_of_last_node_addition;
	sg->events.link_events.disappearance_index = index_of_last_link_addition;

	// Allocate the accumulator for the events
	// printf("nb_events: %zu\n", sg->events.nb_events);
	size_tVector* node_events = MALLOC(sizeof(size_tVector) * sg->events.nb_events);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		node_events[i] = size_tVector_new();
	}

	// Fill the events
	sg->events.node_events.presence_mask = BitArray_n_ones(sg->events.node_events.disappearance_index);

	// For each node
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		TemporalNode* node = &sg->nodes.nodes[i];
		// For each interval
		for (size_t j = 0; j < node->presence.nb_intervals; j++) {
			Interval interval = node->presence.intervals[j];
			size_t start = KeyMomentsTable_find_time_index(&sg->key_moments, interval.start);
			size_t end = KeyMomentsTable_find_time_index(&sg->key_moments, interval.end);
			// Invalidate the bit of the presence mask
			if (end < sg->events.node_events.disappearance_index) {
				// printf("invalidating %zu\n", end);
				BitArray_set_zero(sg->events.node_events.presence_mask, end - 1);
			}
			// Push the creation
			size_tVector_push(&node_events[start], i);

			// Push the deletion if
			if (end >= sg->events.node_events.disappearance_index) {
				size_tVector_push(&node_events[end], i);
			}
		}
	}

	/*printf("node presence mask: ");
	char* str = BitArray_to_string(sg->events.node_events.presence_mask);
	printf("%s\n", str);
	free(str);*/

	// print all the node events
	/*for (size_t i = 0; i < sg->events.nb_events; i++) {
		printf("Event %zu (time %zu): ", i, KeyMomentsTable_nth_key_moment(&sg->key_moments, i));
		str = size_tVector_to_string(node_events[i]);
		printf("%s\n", str);
		free(str);
	}*/

	// Do the same for links
	sg->events.link_events.presence_mask = BitArray_n_ones(sg->events.link_events.disappearance_index + 1);

	size_tVector* link_events = MALLOC(sizeof(size_tVector) * sg->events.nb_events);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		link_events[i] = size_tVector_new();
	}

	for (size_t i = 0; i < sg->links.nb_links; i++) {
		Link* link = &sg->links.links[i];
		for (size_t j = 0; j < link->presence.nb_intervals; j++) {
			Interval interval = link->presence.intervals[j];
			size_t start = KeyMomentsTable_find_time_index(&sg->key_moments, interval.start);
			size_t end = KeyMomentsTable_find_time_index(&sg->key_moments, interval.end);
			if (end < sg->events.link_events.disappearance_index) {
				BitArray_set_zero(sg->events.link_events.presence_mask, end);
			}
			size_tVector_push(&link_events[start], i);
			if (end > sg->events.link_events.disappearance_index) {
				size_tVector_push(&link_events[end], i);
			}
		}
	}

	// print all the link events
	/*for (size_t i = 0; i < sg->events.nb_events; i++) {
		printf("Event %zu (time %zu): ", i, KeyMomentsTable_nth_key_moment(&sg->key_moments, i));
		str = size_tVector_to_string(link_events[i]);
		printf("%s\n", str);
		free(str);
	}*/

	// Propagate the events
	// For each event before the disappearence index, if it has deletions, you recopy from the left until you find
	// another deletion

	for (size_t i = 1; i < sg->events.node_events.disappearance_index; i++) {
		if (BitArray_is_zero(sg->events.node_events.presence_mask, i - 1)) {
			for (int j = i - 2; j >= 0; j--) {
				if ((j == 0) || (BitArray_is_one(sg->events.node_events.presence_mask, j - 1))) {
					for (size_t k = 0; k < node_events[j].size; k++) {
						if (IntervalsSet_contains(sg->nodes.nodes[node_events[j].array[k]].presence,
												  KeyMomentsTable_nth_key_moment(&sg->key_moments, i))) {
							size_tVector_push(&node_events[i], node_events[j].array[k]);
						}
					}
				}
				else {
					break;
				}
			}
		}
	}

	// print all the node events
	/*char* str;
	printf("After propagation (nodes)\n");
	char* mask_str_nodes = BitArray_to_string(sg->events.node_events.presence_mask);
	printf("node presence mask: %s\n", mask_str_nodes);
	free(mask_str_nodes);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		printf("Event %zu (time %zu): ", i, KeyMomentsTable_nth_key_moment(&sg->key_moments, i));
		str = size_tVector_to_string(node_events[i]);
		printf("%s\n", str);
		free(str);
	}*/

	// Do the same for links
	for (size_t i = 1; i < sg->events.link_events.disappearance_index; i++) {
		if (BitArray_is_zero(sg->events.link_events.presence_mask, i - 1)) {
			for (int j = i - 2; j >= 0; j--) {
				if ((j == 0) || (BitArray_is_one(sg->events.link_events.presence_mask, j - 1))) {
					for (size_t k = 0; k < link_events[j].size; k++) {
						if (IntervalsSet_contains(sg->links.links[link_events[j].array[k]].presence,
												  KeyMomentsTable_nth_key_moment(&sg->key_moments, i))) {
							size_tVector_push(&link_events[i], link_events[j].array[k]);
						}
					}
				}
				else {
					break;
				}
			}
			// printf("done\n");
		}
	}

	// print all the link events
	/*printf("After propagation (links)\n");
	char* mask_str = BitArray_to_string(sg->events.link_events.presence_mask);
	printf("link presence mask: %s\n", mask_str);
	free(mask_str);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		printf("Event %zu (time %zu): ", i, KeyMomentsTable_nth_key_moment(&sg->key_moments, i));
		str = size_tVector_to_string(link_events[i]);
		printf("%s\n", str);
		free(str);
	}*/

	// Write the events
	events_table_write(sg, node_events, link_events);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		size_tVector_destroy(node_events[i]);
		size_tVector_destroy(link_events[i]);
	}
	free(node_events);
	free(link_events);
}

void events_destroy(StreamGraph* sg) {
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		if (sg->events.node_events.events[i].events != NULL) {
			free(sg->events.node_events.events[i].events);
		}
		if (sg->events.link_events.events[i].events != NULL) {
			free(sg->events.link_events.events[i].events);
		}
	}
	free(sg->events.node_events.events);
	free(sg->events.link_events.events);
	BitArray_destroy(sg->events.node_events.presence_mask);
	BitArray_destroy(sg->events.link_events.presence_mask);
}