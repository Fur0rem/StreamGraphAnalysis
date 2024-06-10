#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stream_graph.h"
#include "utils.h"

/*
Format :
SGA Internal version 1.0.0


[General]
Lifespan=(0 100)
Named=true


[Memory]
NumberOfNodes=4
NumberOfLinks=4
KeyMoments=13

[[Nodes]]
[[[NumberOfNeighbours]]]
1=2
2=2
3=2
4=1
[[[NumberOfIntervals]]]
1=1
2=2
3=1
4=1

[[Links]]
[[[NumberOfIntervals]]]
1=2
2=1
3=1
4=1

[[KeyMoments]]
[[[NumberOfEvents]]]
0=2
10=1
20=1
30=3
40=1
45=1
50=1
60=1
70=1
75=1
80=1
90=1
100=2


[Data]

[[Neighbours]]
[[[NodesToLinks]]]
0=(0 2)
1=(1 3)
2=(2 3)
3=(1)
[[[LinksToNodes]]]
0=(0 1)
1=(1 3)
2=(0 2)
3=(1 2)

[[Events]]
0=((N 0 +) (N 1 +))
10=((L 0 +) (N 3 +))
20=((L 1 +))
30=((N 3 -) (L 1 -) (L 0 -))
40=((N 2 +) (N 1 -))
45=((L 2 +))
50=((N 1 +))
60=((L 3 +))
70=((L 0 +))
75=((L 2 -))
80=((L 0 -))
90=((N 2 -) (L 3 -))
100=((N 0 -) (N 1 -))


[Names]
1=a
2=b
3=c
4=d
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
	char* end = strchr(str, '\n');                                                                 \
	char* line = (char*)malloc(end - (str) + 1);                                                   \
	strncpy(line, str, end - (str));                                                               \
	line[end - (str)] = '\0';                                                                      \
	printf("line: %s\n", line);                                                                    \
	free(line);

StreamGraph SGA_StreamGraph_from_string(const char* str) {

	StreamGraph sg;
	int nb_scanned;
	char* current_header;

	// Skip first line (version control)

	NEXT_HEADER([General]);
	size_t lifespan_start;
	size_t lifespan_end;
	nb_scanned = sscanf(str, "Lifespan=(%zu %zu)\n", &lifespan_start, &lifespan_end);
	printf("lifespan_start: %zu, lifespan_end: %zu\n", lifespan_start, lifespan_end);
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
	printf("named: %s\n", named ? "true" : "false");

	// Parse the Memory section
	NEXT_HEADER([Memory]);
	// Count how many slices are needed
	size_t relative_max = (RelativeMoment)~0;
	size_t nb_slices = (lifespan_end / relative_max) + 1;
	printf("nb_slices: %zu\n", nb_slices);

	// Parse the memory header
	size_t nb_nodes;
	size_t nb_links;
	size_t nb_key_moments;
	nb_scanned = sscanf(str, "NumberOfNodes=%zu\nNumberOfLinks=%zu\nKeyMoments=%zu\n", &nb_nodes,
						&nb_links, &nb_key_moments);
	EXPECTED_NB_SCANNED(3);
	printf("nb_nodes: %zu, nb_links: %zu, nb_key_moments: %zu\n", nb_nodes, nb_links,
		   nb_key_moments);
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
	for (size_t i = 0; i < nb_nodes; i++) {
		// Parse the node
		size_t nb_neighbors;
		size_t node;
		nb_scanned = sscanf(str, "%zu=%zu\n", &node, &nb_neighbors);
		EXPECTED_NB_SCANNED(2);
		GO_TO_NEXT_LINE(str);
		printf("node: %zu, nb_neighbors: %zu\n", node, nb_neighbors);
		// Allocate the neighbors
		sg.nodes.nodes[i].nb_neighbors = nb_neighbors;
		sg.nodes.nodes[i].neighbors = (EdgeId*)malloc(nb_neighbors * sizeof(EdgeId));
	}

	NEXT_HEADER([[[NumberOfIntervals]]]);
	for (size_t i = 0; i < nb_nodes; i++) {
		// Parse the node
		size_t node;
		size_t nb_intervals;
		nb_scanned = sscanf(str, "%zu=%zu\n", &node, &nb_intervals);
		EXPECTED_NB_SCANNED(2);
		GO_TO_NEXT_LINE(str);
		printf("node: %zu, nb_intervals: %zu\n", node, nb_intervals);
		// Allocate the intervals
		sg.nodes.nodes[i].nb_intervals = nb_intervals;
		sg.nodes.nodes[i].present_at = (Interval*)malloc(nb_intervals * sizeof(Interval));
	}

	NEXT_HEADER([[Links]]);
	NEXT_HEADER([[[NumberOfIntervals]]]);
	for (size_t i = 0; i < nb_links; i++) {
		// Parse the edge
		size_t link;
		size_t nb_intervals;
		nb_scanned = sscanf(str, "%zu=%zu\n", &link, &nb_intervals);
		EXPECTED_NB_SCANNED(2);
		GO_TO_NEXT_LINE(str);
		printf("link: %zu, nb_intervals: %zu\n", link, nb_intervals);
		// Allocate the intervals
		sg.links.links[i].nb_intervals = nb_intervals;
		sg.links.links[i].present_at = (Interval*)malloc(nb_intervals * sizeof(Interval));
	}

	NEXT_HEADER([[KeyMoments]]);
	NEXT_HEADER([[[NumberOfEvents]]]);
	size_t* nb_events_per_key_moment = (size_t*)malloc(nb_key_moments * sizeof(size_t));
	SGA_BitArray presence_mask = SGA_BitArray_n_ones(nb_key_moments);
	for (size_t i = 0; i < nb_key_moments; i++) {
		// Parse the key moment
		size_t key_moment;
		size_t nb_events;
		nb_scanned = sscanf(str, "%zu=%zu\n", &key_moment, &nb_events);
		EXPECTED_NB_SCANNED(2);
		GO_TO_NEXT_LINE(str);
		printf("key_moment: %zu, nb_events: %zu\n", key_moment, nb_events);
		// Allocate the events
		sg.events.events[i] = malloc((nb_events + 2) * sizeof(size_t));
		nb_events_per_key_moment[i] = nb_events;
	}

	NEXT_HEADER([Data]);
	NEXT_HEADER([[Neighbours]]);

	NEXT_HEADER([[[NodesToLinks]]]);
	for (size_t i = 0; i < nb_nodes; i++) {
		// Parse the node
		size_t node;
		nb_scanned = sscanf(str, "%zu=", &node);
		EXPECTED_NB_SCANNED(1);
		str = strchr(str, '(') + 1;
		printf("node: %zu -> ", node);
		for (size_t j = 0; j < sg.nodes.nodes[i].nb_neighbors; j++) {
			size_t link;
			sscanf(str, "%zu", &link);
			sg.nodes.nodes[i].neighbors[j] = link;
			while ((*str != '(') && (*str != ' ') && (*str != '\n')) {
				str++;
			}
			printf("%zu, ", link);
		}
	}

	printf("str: %s\n", str);

	NEXT_HEADER([[[LinksToNodes]]]);
	for (size_t i = 0; i < nb_links; i++) {
		// Parse the edge
		size_t link;
		nb_scanned = sscanf(str, "%zu=", &link);
		EXPECTED_NB_SCANNED(1);
		str = strchr(str, '(') + 1;
		printf("link: %zu -> ", link);
		for (size_t j = 0; j < 2; j++) {
			size_t node;
			sscanf(str, "%zu", &node);
			sg.links.links[i].nodes[j] = node;
			while ((*str != '(') && (*str != ' ') && (*str != '\n')) {
				str++;
			}
			printf("%zu, ", node);
		}
		printf("\n");
	}

	NEXT_HEADER([[Events]]);
	for (size_t i = 0; i < nb_key_moments; i++) {
		// Parse the key moment
		size_t key_moment;
		nb_scanned = sscanf(str, "%zu=", &key_moment);
		EXPECTED_NB_SCANNED(1);
		str = strchr(str, '(') + 1;
		printf("key_moment: %zu -> ", key_moment);
		// Parse all the node events
		size_t j = 0;
		for (; j < nb_events_per_key_moment[i]; j++) {
			char type;
			size_t node;
			char sign;
			sscanf(str, "(%c %zu %c)", &type, &node, &sign);
			if (type == 'L') {
				goto parse_links;
			}
			while ((*str != ')') && (*str != '\n')) {
				str++;
			}
			if (sign == '-') {
				printf("Setting zero at %zu\n", j);
				SGA_BitArray_set_zero(presence_mask, i);
			}
			printf("j = %zu, size = %zu\n", j, nb_events_per_key_moment[i]);
			*access_nth_node(sg.events.events[i], j) = node;
			str += 2;
		}
	parse_links:
		*access_nb_node_events(sg.events.events[i]) = j;
		size_t k;
		for (k = j; k < nb_events_per_key_moment[i]; k++) {
			char type;
			size_t link;
			char sign;
			sscanf(str, "(%c %zu %c)", &type, &link, &sign);
			while ((*str != ')') && (*str != '\n')) {
				str++;
			}
			str += 2;
			*access_nth_link(sg.events.events[i], k - j) = link;
			if (sign == '-') {
				SGA_BitArray_set_zero(presence_mask, i);
			}
		}

		// If there was a suppression of the presence mask, we need to update the number of events

		printf("j = %zu, k = %zu\n", j, k);
		*access_nb_link_events(sg.events.events[i]) = k - j;

		printf("Number of node events: %zu, number of link events: %zu --",
			   *access_nb_node_events(sg.events.events[i]),
			   *access_nb_link_events(sg.events.events[i]));
		printf("\n Node events:");
		for (size_t j = 0; j < *access_nb_node_events(sg.events.events[i]); j++) {
			printf(" %zu", *access_nth_node(sg.events.events[i], j));
		}
		printf("\n Link events:");
		for (size_t j = 0; j < *access_nb_link_events(sg.events.events[i]); j++) {
			printf(" %zu", *access_nth_link(sg.events.events[i], j));
		}
		printf("\n");

		GO_TO_NEXT_LINE(str);
		printf("\n");
	}

	char* presence_mask_str = SGA_BitArray_to_string(presence_mask);
	printf("presence_mask: %s\n", presence_mask_str);
	free(presence_mask_str);

	// Parse the names
	if (named) {
		NEXT_HEADER([Names]);
		for (size_t i = 0; i < nb_nodes; i++) {
			size_t node;
			char name[256];
			nb_scanned = sscanf(str, "%zu=%255s\n", &node, name);
			EXPECTED_NB_SCANNED(2);
			printf("node: %zu, name: %s\n", node, name);
			sg.node_names.names[i] = strdup(name);
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

char* SGA_StreamGraph_to_string(StreamGraph* sg) {
	return NULL;
}