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
[General]
Lifespan=(0 100)
[Memory]
NumberOfNodes=4
NumberOfLinks=4
KeyMoments=13
[[Nodes]]
[[[Neighbours]]]
1=2
2=2
3=2
4=1
[[[Intervals]]]
1=1
2=2
3=1
4=1
[[Links]]
[[[Intervals]]]
1=2
2=1
3=1
4=1
[Values]
KeyMoments=(0 10 20 30 40 45 50 60 70 75 80 90 100)
[[Nodes]]
[[[Neighbours]]]
1=(0 2)
2=(1 3)
3=(2 3)
4=(1)
[[[Intervals]]]
1=((0 13))
2=((0 5) (6 13))
3=((4 12))
4=((1 4))
[[Links]]
[[[Nodes]]]
1=(0 1)
2=(1 3)
3=(0 2)
4=(1 2)
[[[Intervals]]]
1=((1 4) (8 11))
2=((2 4))
3=((5 10))
4=((7 12))
[Names]
1=a
2=b
3=c
4=d
*/

void SGA_init_key_moments(StreamGraph* sg, size_t* key_moments) {
}

char* get_to_header(const char* str, const char* header) {
	char* str2 = strstr(str, header);
	if (str2 == NULL) {
		fprintf(stderr, "Could not find header %s\n", header);
		exit(1);
	}
	str2 = strchr(str, '\n') + 1;
	return str2;
}

#define NEXT_HEADER(section)                                                                       \
	current_header = "" #section "\n";                                                             \
	(str) = get_to_header(str, current_header);

#define EXPECTED_NB_SCANNED(expected)                                                              \
	if (nb_scanned != (expected)) {                                                                \
		fprintf(stderr,                                                                            \
				TEXT_RED TEXT_BOLD "Could not parse the header " TEXT_RESET                        \
								   "%sError at line %d\n",                                         \
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
	GO_TO_NEXT_LINE(str);
	GO_TO_NEXT_LINE(str);
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
	GO_TO_NEXT_LINE(str);
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

	NEXT_HEADER([Values]);
	// Parse key moments
	// KeyMoments=(0 10 20 30 40 45 50 60 70 75 80 90 100)
	size_t* key_moments = (size_t*)malloc(nb_key_moments * sizeof(size_t));
	nb_scanned = sscanf(str, "KeyMoments=(");
	EXPECTED_NB_SCANNED(0);
	str += strlen("KeyMoments=(");
	for (size_t i = 0; i < nb_key_moments; i++) {
		nb_scanned = sscanf(str, "%zu", &key_moments[i]);
		printf("key_moments[%zu]: %zu\n", i, key_moments[i]);
		EXPECTED_NB_SCANNED(1);
		// Move to the next number
		while (*str != ' ' && *str != ')') {
			str++;
		}
		str++;
	}
	GO_TO_NEXT_LINE(str);

	// Parse the nodes
	NEXT_HEADER([[Nodes]]);
	NEXT_HEADER([[[Neighbours]]]);
	for (size_t i = 0; i < nb_nodes; i++) {
		// Parse the node
		size_t node;
		size_t nb_neighbors;
		nb_scanned = sscanf(str, "%zu=(", &node);
		EXPECTED_NB_SCANNED(1);
		str += strlen("=(") + 1;
		for (size_t j = 0; j < sg.nodes.nodes[i].nb_neighbors; j++) {
			nb_scanned = sscanf(str, "%zu", &sg.nodes.nodes[i].neighbors[j]);
			EXPECTED_NB_SCANNED(1);
			printf("sg.nodes.nodes[%zu].neighbors[%zu]: %zu\n", i, j,
				   sg.nodes.nodes[i].neighbors[j]);
			// Move to the next number
			while (*str != ' ' && *str != ')') {
				str++;
			}
			str++;
		}
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([[[Intervals]]]);
	for (size_t i = 0; i < nb_nodes; i++) {
		PRINT_LINE(str);
		// Parse the node
		size_t node;
		nb_scanned = sscanf(str, "%zu=(", &node);
		EXPECTED_NB_SCANNED(1);
		str += strlen("=(") + 1;
		for (size_t j = 0; j < sg.nodes.nodes[i].nb_intervals; j++) {
			nb_scanned = sscanf(str, "(%zu %zu)", &sg.nodes.nodes[i].present_at[j].start,
								&sg.nodes.nodes[i].present_at[j].end);
			EXPECTED_NB_SCANNED(2);
			printf("sg.nodes.nodes[%zu].present_at[%zu].start: %zu, end: %zu\n", i, j,
				   sg.nodes.nodes[i].present_at[j].start, sg.nodes.nodes[i].present_at[j].end);

			// Move to the next number
			while (*str != ')') {
				str++;
			}
			str++;
			if (*str == ' ') {
				str++;
			}
		}
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([[Links]]);
	NEXT_HEADER([[[Nodes]]]);
	for (size_t i = 0; i < nb_links; i++) {
		// Parse the edge
		size_t link;
		nb_scanned = sscanf(str, "%zu=(", &link);
		EXPECTED_NB_SCANNED(1);
		str += strlen("=(") + 1;
		for (size_t j = 0; j < 2; j++) {
			nb_scanned = sscanf(str, "%zu", &sg.links.links[i].nodes[j]);
			EXPECTED_NB_SCANNED(1);
			printf("sg.links.links[%zu].nodes[%zu]: %zu\n", i, j, sg.links.links[i].nodes[j]);
			// Move to the next number
			while (*str != ' ' && *str != ')') {
				str++;
			}
			str++;
		}
		GO_TO_NEXT_LINE(str);
	}

	NEXT_HEADER([[[Intervals]]]);
	for (size_t i = 0; i < nb_links; i++) {
		PRINT_LINE(str);
		// Parse the edge
		size_t link;
		nb_scanned = sscanf(str, "%zu=(", &link);
		EXPECTED_NB_SCANNED(1);
		str += strlen("=(") + 1;
		for (size_t j = 0; j < sg.links.links[i].nb_intervals; j++) {
			nb_scanned = sscanf(str, "(%zu %zu)", &sg.links.links[i].present_at[j].start,
								&sg.links.links[i].present_at[j].end);
			EXPECTED_NB_SCANNED(2);
			printf("sg.links.links[%zu].present_at[%zu].start: %zu, end: %zu\n", i, j,
				   sg.links.links[i].present_at[j].start, sg.links.links[i].present_at[j].end);

			// Move to the next number
			while (*str != ')') {
				str++;
			}
			str++;
			if (*str == ' ') {
				str++;
			}
		}
		GO_TO_NEXT_LINE(str);
	}

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

	SGA_init_key_moments(&sg, key_moments);

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