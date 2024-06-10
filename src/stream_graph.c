#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream_graph.h"
#include "utils.h"

/* Format :
SGA Internal version 1.0.0


[General]
Lifespan=(0 100)
Named=true


[Memory]
NumberOfNodes=4
NumberOfLinks=4
RegularKeyMoments=9
RemovalOnlyMoments=3
NumberOfSlices=1

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

[[KeyMoments]]
[[[NumberOfEvents]]]
2
2
1
0
3
2
1
1
1
0
1
2
2

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
0=((N 0) (N 1))
[[[Regular]]]
10=(+ (N 3) (L 0))
20=(+ (L 1))
30=(+)
40=(- (N 0) (N 1) (N 2))
45=(- (N 0) (N 2))
50=(+ (N 1))
60=(+ (L 3))
70=(+ (L 0))
75=(+)
[[[RemovalOnly]]]
80=((L 0))
90=((N 2) (L 3))
100=((N 0) (N 1))

[[PresenceIntervals]]
[[[Nodes]]]
((0 100))
((0 50) (60 100))
((40 90))
((10 30))
[[[Links]]]
((10 30) (70 80))
((20 30))
((45 75))
((60 90))


[Names]
a
b
c
d
*/

size_t* access_nb_node_events(Event event) {
	return event + 0;
}

size_t* access_nb_link_events(Event event) {
	return event + 1;
}

size_t* access_nth_node(Event event, size_t n) {
	return event + 2 + n;
}

size_t* access_nth_link(Event event, size_t n) {
	return event + 2 + *access_nb_node_events(event) + n;
}

void SGA_init_key_moments(StreamGraph* sg, size_t* key_moments) {
}

char* get_to_header(const char* str, const char* header) {
	char* str2 = strstr(str, header);
	if (str2 == NULL) {
		fprintf(stderr, "Could not find header %s\n", header);
		exit(1);
	}
	str2 = strchr(str2, '\n') + 1;
	return str2;
}

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

#define GO_TO_NEXT_LINE(str) (str) = strchr(str, '\n') + 1;

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
		(str)++;                                                                                   \
		while ((*(str) != '(') && (*(str) != '\n')) {                                              \
			(str)++;                                                                               \
		}                                                                                          \
	}

void push_key_moment(StreamGraph* sg, size_t key_moment) {
	static size_t current_slice = 0;
	static size_t current_moment = 0;
	size_t slice = key_moment / (RelativeMoment)~0;
	size_t relative_moment = key_moment % (RelativeMoment)~0;
	if (slice != current_slice) {
		current_slice = slice;
		current_moment = 0;
	}
	sg->moments.slices[slice].moments[current_moment] = relative_moment;
	current_moment++;
}

// TODO : Make the code better and less unreadable copy pasted code
// TODO : Add parsing for the key moments table
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
	sg.moments.nb_slices = nb_slices;
	sg.moments.slices = (MomentsSlice*)malloc(nb_slices * sizeof(MomentsSlice));
	sg.nodes.nb_nodes = nb_nodes;
	sg.nodes.nodes = (TemporalNode*)malloc(nb_nodes * sizeof(TemporalNode));
	sg.links.nb_links = nb_links;
	sg.links.links = (Link*)malloc(nb_links * sizeof(Link));
	sg.events.nb_events = nb_key_moments;
	sg.events.events = (Event*)malloc(nb_key_moments * sizeof(Event));
	if (named) {
		sg.node_names.names = (const char**)malloc(nb_nodes * sizeof(const char*));
	}
	else {
		sg.node_names.names = NULL;
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
	SGA_BitArray presence_mask = SGA_BitArray_n_ones(nb_regular_key_moments);
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

	// Allocate the slices
	sg.moments.nb_slices = nb_slices;
	printf("nb_slices: %zu\n", nb_slices);
	sg.moments.slices = (MomentsSlice*)malloc(nb_slices * sizeof(MomentsSlice));
	NEXT_HEADER([[[NumberOfSlices]]]);
	size_t nb_slices2;
	for (size_t i = 0; i < nb_slices; i++) {
		nb_scanned = sscanf(str, "%zu\n", &nb_slices2);
		printf("nb_slices2: %zu\n", nb_slices2);
		EXPECTED_NB_SCANNED(1);
		sg.moments.slices[i].nb_moments = nb_slices2;
		sg.moments.slices[i].moments = (RelativeMoment*)malloc(nb_slices2 * sizeof(RelativeMoment));
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
	push_key_moment(&sg, key_moment);
	// Parse all the node events
	size_t j = 0;
	for (; j < nb_events_per_key_moment[0]; j++) {
		char type;
		size_t node;
		nb_scanned = sscanf(str, "(%c %zu)", &type, &node);
		EXPECTED_NB_SCANNED(2);
		NEXT_TUPLE(str);
		*access_nth_node(sg.events.events[0], j) = node;
	}
	*access_nb_node_events(sg.events.events[0]) = j;
	size_t k;
	for (k = j; k < nb_events_per_key_moment[0]; k++) {
		char type;
		size_t link;
		nb_scanned = sscanf(str, "(%c %zu)", &type, &link);
		EXPECTED_NB_SCANNED(2);
		NEXT_TUPLE(str);
		*access_nth_link(sg.events.events[0], k - j) = link;
	}
	*access_nb_link_events(sg.events.events[0]) = k - j;
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
			SGA_BitArray_set_zero(presence_mask, i - 1);
		}
		push_key_moment(&sg, key_moment);
		// Parse all the node events
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
			*access_nth_node(sg.events.events[i], j) = id;
		}
	parse_links:
		*access_nb_node_events(sg.events.events[i]) = j;
		size_t k;
		for (k = j; k < nb_events_per_key_moment[i]; k++) {
			char link;
			size_t id;
			nb_scanned = sscanf(str, "(%c %zu)", &link, &id);
			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			*access_nth_link(sg.events.events[i], k - j) = id;
		}
		*access_nb_link_events(sg.events.events[i]) = k - j;
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
		push_key_moment(&sg, key_moment);
		// Parse all the node events
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
			*access_nth_node(sg.events.events[i], j) = id;
		}
	parse_links_removal:
		*access_nb_node_events(sg.events.events[i]) = j;
		size_t k;
		for (k = j; k < nb_events_per_key_moment[i]; k++) {
			char link;
			size_t id;
			nb_scanned = sscanf(str, "(%c %zu)", &link, &id);
			EXPECTED_NB_SCANNED(2);
			NEXT_TUPLE(str);
			*access_nth_link(sg.events.events[i], k - j) = id;
		}
		*access_nb_link_events(sg.events.events[i]) = k - j;
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
			sg.node_names.names[node] = strdup(name);
			GO_TO_NEXT_LINE(str);
		}
	}

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
	char* buffer = (char*)malloc(size + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	fread(buffer, 1, size, file);
	buffer[size] = '\0';
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

size_t SGA_StreamGraph_lifespan_begin(StreamGraph* sg) {
	size_t first_slice_idx = 0;
	for (size_t i = 0; i < sg->moments.nb_slices; i++) {
		if (sg->moments.slices[i].nb_moments > 0) {
			first_slice_idx = i;
			break;
		}
	}
	size_t first_moment =
		sg->moments.slices[first_slice_idx].moments[0] + (first_slice_idx * (RelativeMoment)~0);
	return first_moment;
}

size_t SGA_StreamGraph_lifespan_end(StreamGraph* sg) {
	size_t last_slice_idx = sg->moments.nb_slices - 1;
	size_t last_moment_idx = sg->moments.slices[last_slice_idx].nb_moments - 1;
	size_t last_moment = sg->moments.slices[last_slice_idx].moments[last_moment_idx] +
						 (last_slice_idx * (RelativeMoment)~0);
	return last_moment;
}

char* SGA_TemporalNode_to_string_named(StreamGraph* sg, size_t node_idx) {
	charVector vec = charVector_new();
	const char** node_names = sg->node_names.names;
	const char* name = node_names[node_idx];
	TemporalNode* node = &sg->nodes.nodes[node_idx];
	charVector_append(&vec, APPEND_CONST("\tNode "));
	charVector_append(&vec, name, strlen(name));
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
		char neighbour[100];
		size_t neighbour_idx = (node1 == node_idx) ? node2 : node1;
		sprintf(neighbour, "%s ", node_names[neighbour_idx]);
		charVector_append(&vec, neighbour, strlen(neighbour));
	}
	charVector_append(&vec, APPEND_CONST("\n\t\t}\n\n\t}\n"));
	return vec.array;
}

// TODO: Implement this function
char* SGA_TemporalNode_to_string_unnamed(StreamGraph* sg, size_t node_idx) {
	return NULL;
}

char* SGA_TemporalNode_to_string(StreamGraph* sg, size_t node_idx) {
	if (sg->node_names.names == NULL) {
		return SGA_TemporalNode_to_string_unnamed(sg, node_idx);
	}
	return SGA_TemporalNode_to_string_named(sg, node_idx);
}

char* SGA_Link_to_string_named(StreamGraph* sg, size_t link_idx) {
	charVector vec = charVector_new();
	const char** node_names = sg->node_names.names;
	Link* link = &sg->links.links[link_idx];
	charVector_append(&vec, APPEND_CONST("\tLink ("));
	charVector_append(&vec, node_names[link->nodes[0]], strlen(node_names[link->nodes[0]]));
	charVector_append(&vec, APPEND_CONST(" "));
	charVector_append(&vec, node_names[link->nodes[1]], strlen(node_names[link->nodes[1]]));
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
	return vec.array;
}

// TODO: Implement this function
char* SGA_Link_to_string_unnamed(StreamGraph* sg, size_t link_idx) {
	return NULL;
}

char* SGA_Link_to_string(StreamGraph* sg, size_t link_idx) {
	if (sg->node_names.names == NULL) {
		return SGA_Link_to_string_unnamed(sg, link_idx);
	}
	return SGA_Link_to_string_named(sg, link_idx);
}

char* SGA_EventsTable_to_string(StreamGraph* sg) {
	charVector vec = charVector_new();
	charVector_append(&vec, APPEND_CONST("EventsTable {\n"));

	// For all the events, add its timestamp, its presence mask and its events
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		charVector_append(&vec, APPEND_CONST("\tEvent {\n"));
		char timestamp[100];
		printf("moments: %p\n", sg->moments.slices[i].moments);
		sprintf(timestamp, "\t\tTimestamp=%u\n", sg->moments.slices[i].moments[0]);
		charVector_append(&vec, timestamp, strlen(timestamp));
		charVector_append(&vec, APPEND_CONST("\t\tPresenceMask="));
		for (size_t j = 0; j < sg->moments.slices[i].nb_moments; j++) {
			char bit = SGA_BitArray_is_one(sg->events.presence_mask, j) ? '1' : '0';
			charVector_append(&vec, &bit, 1);
		}
		charVector_append(&vec, APPEND_CONST("\n"));
		charVector_append(&vec, APPEND_CONST("\t\tEvents=[\n"));
		size_t nb_node_events = *access_nb_node_events(sg->events.events[i]);
		size_t nb_link_events = *access_nb_link_events(sg->events.events[i]);
		for (size_t j = 0; j < nb_node_events; j++) {
			char event[100];
			sprintf(event, "\t\t\t(N %zu)\n", *access_nth_node(sg->events.events[i], j));
			charVector_append(&vec, event, strlen(event));
		}
		for (size_t j = 0; j < nb_link_events; j++) {
			char event[100];
			sprintf(event, "\t\t\t(L %zu)\n", *access_nth_link(sg->events.events[i], j));
			charVector_append(&vec, event, strlen(event));
		}
		charVector_append(&vec, APPEND_CONST("\t\t]\n"));
		charVector_append(&vec, APPEND_CONST("\t}\n"));
	}

	return vec.array;
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
	/*charVector_append(&vec, APPEND_CONST("\n\tEvents=[\n"));
	char* events_str = SGA_EventsTable_to_string(sg);
	charVector_append(&vec, events_str, strlen(events_str));
	free(events_str);*/

	return vec.array;
}