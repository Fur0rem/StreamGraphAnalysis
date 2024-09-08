#include "stream.h"

#include <stdbool.h>
#include <stddef.h>

#include "defaults.h"
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashset.h"
#include "interval.h"
#include "stream_graph/events_table.h"
#include "stream_graph/links_set.h"
#include "units.h"
#include "utils.h"
#include "vector.h"

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

#define NEXT_HEADER(section)                                                                                           \
	current_header = "" #section "";                                                                                   \
	(str)		   = get_to_header(str, current_header);

#define EXPECTED_NB_SCANNED(expected)                                                                                  \
	if (nb_scanned != (expected)) {                                                                                    \
		fprintf(stderr, TEXT_RED TEXT_BOLD "Could not parse the header " TEXT_RESET "%s\nError at line %d\n",          \
				current_header, __LINE__);                                                                             \
		fprintf(stderr, "Number of scanned elements: %d, expected: %d\n", nb_scanned, expected);                       \
		fprintf(stderr, "Current line:");                                                                              \
		PRINT_LINE(str);                                                                                               \
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
		char* end  = strchr(str, '\n');                                                                                \
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
StreamGraph StreamGraph_from_string(const char* str) {

	StreamGraph sg;
	int nb_scanned		 = -1;
	char* current_header = NULL;

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
	size_t nb_slices = (lifespan_end / SLICE_SIZE) + 1;

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
	sg.nodes	   = TemporalNodesSet_alloc(nb_nodes);
	sg.links	   = LinksSet_alloc(nb_links);

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
		sg.nodes.nodes[node].neighbours	   = (LinkId*)malloc(nb_neighbours * sizeof(LinkId));
	}

	NEXT_HEADER([[[NumberOfIntervals]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		// Parse the node
		size_t nb_intervals;
		nb_scanned = sscanf(str, "%zu\n", &nb_intervals);
		EXPECTED_NB_SCANNED(1);
		GO_TO_NEXT_LINE(str);
		// Allocate the intervals
		IntervalsSet presence		  = IntervalsSet_alloc(nb_intervals);
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
		IntervalsSet presence		  = IntervalsSet_alloc(nb_intervals);
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
			str++;
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
						fprintf(stderr, "Link %zu (from %zu to %zu) removed twice without being added\n", id,
								sg.links.links[id].nodes[0], sg.links.links[id].nodes[1]);
						PRINT_LINE(str);
						printf("line: %zu\n", i);
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

	NEXT_HEADER([EndOfStream]);

	free(key_moments);

	// for each thing pushed, check if it is even
	// TODO : better checking and error messages
	for (size_t i = 0; i < nb_nodes; i++) {
		if (nb_pushed_for_nodes[i] % 2 != 0) {
			fprintf(stderr, TEXT_RED TEXT_BOLD "Node %zu was pushed/removed %d times\n" TEXT_RESET, i,
					nb_pushed_for_nodes[i]);
		}
	}
	for (size_t i = 0; i < nb_links; i++) {
		if (nb_pushed_for_links[i] % 2 != 0) {
			fprintf(stderr, TEXT_RED TEXT_BOLD "Link %zu was pushed/removed %d times\n" TEXT_RESET, i,
					nb_pushed_for_links[i]);
		}
	}

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
	buffer[size]	 = '\0';
	buffer[size + 1] = '\0';
	fclose(file);

	// Parse the stream graph
	StreamGraph sg = StreamGraph_from_string(buffer);
	free(buffer);
	return sg;
}

size_t StreamGraph_lifespan_begin(StreamGraph* sg) {
	return KeyMomentsTable_first_moment(&sg->key_moments);
}

size_t StreamGraph_lifespan_end(StreamGraph* sg) {
	return KeyMomentsTable_last_moment(&sg->key_moments);
}

typedef struct {
	size_t moment;
	char sign;
	char letter; // TODO: use an enum instead
	union {
		size_t node;
		struct {
			size_t node1;
			size_t node2;
		};
	} id;
} EventTuple;

String EventTuple_to_string(EventTuple* tuple) {
	String str = String_with_capacity(50);
	switch (tuple->letter) {
		case 'N':
			String_append_formatted(&str, "(%zu %c %c %zu)", tuple->moment, tuple->sign, tuple->letter, tuple->id.node);
			break;
		case 'L':
			String_append_formatted(&str, "(%zu %c %c %zu %zu)", tuple->moment, tuple->sign, tuple->letter,
									tuple->id.node1, tuple->id.node2);
			break;
		default:
			fprintf(stderr, "Could not parse the letter %c\n", tuple->letter);
			exit(1);
	}
	return str;
}

bool EventTuple_equals(EventTuple tuple1, EventTuple tuple2) {
	if (tuple1.moment != tuple2.moment) {
		return false;
	}
	if (tuple1.sign != tuple2.sign) {
		return false;
	}
	if (tuple1.letter != tuple2.letter) {
		return false;
	}
	if (tuple1.letter == 'N') {
		return tuple1.id.node == tuple2.id.node;
	}
	else {
		return tuple1.id.node1 == tuple2.id.node1 && tuple1.id.node2 == tuple2.id.node2;
	}
}

DeclareVector(EventTuple);
DefineVector(EventTuple);
DeclareVector(size_tHashset);
DefineVector(size_tHashset);

DeclareVector(EventTupleVector);
DefineVector(EventTupleVector);
DefineVectorDeriveRemove(EventTuple, NO_FREE(EventTuple));
DefineVectorDeriveRemove(EventTupleVector, EventTupleVector_destroy);

size_t link_hash(Link key) {
	return key.nodes[0] + key.nodes[1];
}

typedef struct {
	size_t nodes[2];
	size_t nb_intervals;
} LinkInfo;

String LinkInfo_to_string(LinkInfo* info) {
	char* str = (char*)malloc(50);
	sprintf(str, "(%zu %zu) %zu", info->nodes[0], info->nodes[1], info->nb_intervals);
	return String_from_owned(str);
}

bool LinkInfo_equals(LinkInfo info1, LinkInfo info2) {
	return info1.nodes[0] == info2.nodes[0] && info1.nodes[1] == info2.nodes[1];
}

DeclareVector(LinkInfo);
DefineVector(LinkInfo);

typedef struct {
	size_t nodes[2];
	size_t id;
} LinkIdMap;

size_t LinkIdMap_hash(LinkIdMap* key) {
	return key->nodes[0] + key->nodes[1];
}

bool LinkIdMap_equals(LinkIdMap* map1, LinkIdMap* map2) {
	return map1->nodes[0] == map2->nodes[0] && map1->nodes[1] == map2->nodes[1];
}

DeclareVector(LinkIdMap);
DefineVector(LinkIdMap);
DeclareHashset(LinkIdMap);
DefineHashset(LinkIdMap);

DefineVectorDeriveRemove(size_tHashset, size_tHashset_destroy);
DefineVectorDeriveRemove(LinkInfo, NO_FREE(LinkInfo));

// Transforms an external format to an internal format
char* InternalFormat_from_External_str(const char* str) {
	char* current_header = "None";
	int nb_scanned;

	// Skip to general section
	str = get_to_header(str, "[General]");

	size_tHashsetVector node_neighbours = size_tHashsetVector_with_capacity(10);

	size_t lifespan_start;
	size_t lifespan_end;
	nb_scanned = sscanf(str, "Lifespan=(%zu %zu)\n", &lifespan_start, &lifespan_end);
	EXPECTED_NB_SCANNED(2);
	GO_TO_NEXT_LINE(str);

	// Parse the general section
	size_t scaling;
	nb_scanned = sscanf(str, "Scaling=%zu\n", &scaling);
	EXPECTED_NB_SCANNED(1);

	// Skip to events section
	str = get_to_header(str, "[Events]");

	EventTupleVectorVector events = EventTupleVectorVector_with_capacity(10);
	LinkInfoVector links		  = LinkInfoVector_with_capacity(10);
	LinkInfoVector nodes		  = LinkInfoVector_with_capacity(10);
	size_t biggest_node_id		  = 0;

	// Parse the events
	size_t nb_events			  = 0;
	size_t current_vec			  = 0;
	size_tVector number_of_slices = size_tVector_with_capacity(10);
	while (strncmp(str, "[EndOfStream]", 11) != 0) {
		// if the line is empty, skip it
		if (*str == '\n') {
			break;
		}
		size_t key_moment;
		char sign;
		char letter;
		size_t one, two;
		EventTuple tuple;
		nb_scanned = sscanf(str, "%zu %c %c", &key_moment, &sign, &letter);
		EXPECTED_NB_SCANNED(3);
		if (letter == 'N') {
			nb_scanned = sscanf(str, "%zu %c %c %zu", &key_moment, &sign, &letter, &one);
			EXPECTED_NB_SCANNED(4);
			tuple = (EventTuple){key_moment, sign, letter, .id.node = one};

			// push the neighbours
			if (one > biggest_node_id) {
				biggest_node_id = one;
			}
			if (biggest_node_id >= node_neighbours.size) {
				for (size_t i = node_neighbours.size; i <= biggest_node_id; i++) {
					size_tHashsetVector_push(&node_neighbours, size_tHashset_with_capacity(10));
				}
			}

			LinkInfo info = {
				.nodes = {one, one},
					 .nb_intervals = 1
			  };

			bool found = false;
			for (size_t i = 0; i < nodes.size; i++) {
				if (nodes.array[i].nodes[0] == info.nodes[0] && nodes.array[i].nodes[1] == info.nodes[1]) {
					nodes.array[i].nb_intervals++;
					found = true;
					break;
				}
			}
			if (!found) {
				LinkInfoVector_push(&nodes, info);
			}
		}
		else {
			nb_scanned = sscanf(str, "%zu %c %c %zu %zu", &key_moment, &sign, &letter, &one, &two);
			EXPECTED_NB_SCANNED(5);
			tuple = (EventTuple){
				key_moment,
				sign,
				letter,
				.id = {.node1 = one, .node2 = two},
			};
			// push the neighbours
			if (one > biggest_node_id) {
				biggest_node_id = one;
			}
			if (two > biggest_node_id) {
				biggest_node_id = two;
			}
			if (biggest_node_id >= node_neighbours.size) {
				for (size_t i = node_neighbours.size; i <= biggest_node_id; i++) {
					size_tHashsetVector_push(&node_neighbours, size_tHashset_with_capacity(10));
				}
			}
			size_tHashset_insert(&node_neighbours.array[one], two);
			size_tHashset_insert(&node_neighbours.array[two], one);

			LinkInfo info = {
				.nodes = {one, two},
					 .nb_intervals = 1
			  };

			bool found = false;
			for (size_t i = 0; i < links.size; i++) {
				if ((links.array[i].nodes[0] == info.nodes[0] && links.array[i].nodes[1] == info.nodes[1]) ||
					(links.array[i].nodes[0] == info.nodes[1] && links.array[i].nodes[1] == info.nodes[0])) {
					links.array[i].nb_intervals++;
					found = true;
					break;
				}
			}
			if (!found) {
				LinkInfoVector_push(&links, info);
			}
		}
		if (nb_events == 0) {
			EventTupleVector events_vec = EventTupleVector_with_capacity(10);
			EventTupleVector_push(&events_vec, tuple);
			EventTupleVectorVector_push(&events, events_vec);
		}
		else {
			if (key_moment == events.array[current_vec].array[0].moment) {
				EventTupleVector_push(&events.array[current_vec], tuple);
			}
			else {
				EventTupleVector events_vec = EventTupleVector_with_capacity(10);
				EventTupleVector_push(&events_vec, tuple);
				EventTupleVectorVector_push(&events, events_vec);
				current_vec++;
			}
		}
		nb_events++;

		GO_TO_NEXT_LINE(str);
	}

	for (size_t i = 0; i < events.size; i++) {
		size_t slice_id = events.array[i].array[0].moment / SLICE_SIZE;
		if (slice_id >= number_of_slices.size) {
			for (size_t j = number_of_slices.size; j <= slice_id; j++) {
				size_tVector_push(&number_of_slices, 0);
			}
		}
		number_of_slices.array[slice_id]++;
	}
	String out_str = String_from_duplicate("SGA Internal version 1.0.0\n\n");

	String_push_str(&out_str, "[General]\n");
	String_append_formatted(&out_str, "Lifespan=(%zu %zu)\n", lifespan_start, lifespan_end);
	String_append_formatted(&out_str, "Scaling=%zu\n\n", scaling);

	String_push_str(&out_str, "[Memory]\n");
	String_append_formatted(&out_str, "NumberOfNodes=%zu\n", biggest_node_id + 1);
	String_append_formatted(&out_str, "NumberOfLinks=%zu\n", LinkInfoVector_len(&links));
	String_append_formatted(&out_str, "NumberOfKeyMoments=%zu\n\n", EventTupleVectorVector_len(&events));

	String_push_str(&out_str, "[[Nodes]]\n");

	String_push_str(&out_str, "[[[NumberOfNeighbours]]]\n");
	for (size_t i = 0; i <= biggest_node_id; i++) {
		String_append_formatted(&out_str, "%zu\n", size_tHashset_nb_elems(&node_neighbours.array[i]));
	}
	String_push_str(&out_str, "[[[NumberOfIntervals]]]\n");
	for (size_t i = 0; i <= biggest_node_id; i++) {
		String_append_formatted(&out_str, "%zu\n", nodes.array[i].nb_intervals / 2);
	}

	String_push_str(&out_str, "[[Links]]\n");

	String_push_str(&out_str, "[[[NumberOfIntervals]]]\n");
	for (size_t i = 0; i < LinkInfoVector_len(&links); i++) {
		String_append_formatted(&out_str, "%zu\n", links.array[i].nb_intervals / 2);
	}
	String_push_str(&out_str, "[[[NumberOfSlices]]]\n");
	for (size_t i = 0; i < number_of_slices.size; i++) {
		String_append_formatted(&out_str, "%zu\n", number_of_slices.array[i]);
	}

	String_push_str(&out_str, "[Data]\n");
	String_push_str(&out_str, "[[Neighbours]]\n");

	String_push_str(&out_str, "[[[NodesToLinks]]]\n");

	for (size_t i = 0; i < nodes.size; i++) {
		String_push_str(&out_str, "(");
		size_tHashset neighs = node_neighbours.array[i];
		for (size_t j = 0; j < neighs.capacity; j++) {
			for (size_t k = 0; k < neighs.buckets[j].size; k++) {
				size_t neighbour = neighs.buckets[j].array[k];
				// Find the id of the link with the neighbour
				size_t link_id = SIZE_MAX;
				for (size_t l = 0; l < links.size; l++) {
					if ((links.array[l].nodes[0] == i && links.array[l].nodes[1] == neighbour) ||
						(links.array[l].nodes[0] == neighbour && links.array[l].nodes[1] == i)) {
						link_id = l;
						break;
					}
				}
				if (link_id == SIZE_MAX) {
					fprintf(stderr, "Could not find link with nodes %zu and %zu\n", i, neighbour);
					exit(1);
				}
				String_append_formatted(&out_str, "%zu ", link_id);
			}
		}
		String_pop(&out_str);
		String_push_str(&out_str, ")\n");
	}

	String_push_str(&out_str, "[[[LinksToNodes]]]\n");
	for (size_t i = 0; i < links.size; i++) {
		String_append_formatted(&out_str, "(%zu %zu)\n", links.array[i].nodes[0], links.array[i].nodes[1]);
	}

	String_push_str(&out_str, "[[Events]]\n");

	size_t* link_id_map = (size_t*)malloc((biggest_node_id + 1) * (biggest_node_id + 1) * sizeof(size_t));
	for (size_t i = 0; i < links.size; i++) {
		link_id_map[links.array[i].nodes[0] * (biggest_node_id + 1) + links.array[i].nodes[1]] = i;
		link_id_map[links.array[i].nodes[1] * (biggest_node_id + 1) + links.array[i].nodes[0]] = i;
	}

	// printf("parsing events 2\n");
	for (size_t i = 0; i < events.size; i++) {
		// sprintf(buffer, "%zu=(", events.array[i].array[0].moment);
		String_append_formatted(&out_str, "%zu=(", events.array[i].array[0].moment);
		for (size_t j = 0; j < events.array[i].size; j++) {
			if (events.array[i].array[j].letter == 'L') {
				EventTuple e = events.array[i].array[j];
				size_t idx1	 = e.id.node1;
				size_t idx2	 = e.id.node2;
				// printf("idx1 : %zu, idx2 : %zu, idx2 - idx1 : %zu\n", idx1, idx2, idx2 - idx1);
				size_t link_id = link_id_map[idx1 * (biggest_node_id + 1) + idx2];
				// printf("link_id: %zu\n", link_id);
				// sprintf(tuple_str, "(%c %c %zu) ", events.array[i].array[j].sign, events.array[i].array[j].letter,
				// 		link_id);
				String_append_formatted(&out_str, "(%c %c %zu) ", events.array[i].array[j].sign,
										events.array[i].array[j].letter, link_id);
			}
			else {
				// sprintf(tuple_str, "(%c %c %zu) ", events.array[i].array[j].sign, events.array[i].array[j].letter,
				// 		events.array[i].array[j].id.node);
				String_append_formatted(&out_str, "(%c %c %zu) ", events.array[i].array[j].sign,
										events.array[i].array[j].letter, events.array[i].array[j].id.node);
			}
			// strcat(buffer, tuple_str);
		}
		// charVector_append(&vec, buffer, strlen(buffer));
		// charVector_pop_last(&vec);
		// charVector_append(&vec, APPEND_CONST(")\n"));
		String_pop(&out_str);
		String_push_str(&out_str, ")\n");
	}
	size_t sum_events = 0;
	for (size_t i = 0; i < events.size; i++) {
		sum_events += events.array[i].size;
	}

	free(link_id_map);
	String_push_str(&out_str, "[EndOfStream]\n");
	String_push_str(&out_str, "\0");

	size_tHashsetVector_destroy(node_neighbours);
	EventTupleVectorVector_destroy(events);
	LinkInfoVector_destroy(links);
	LinkInfoVector_destroy(nodes);
	size_tVector_destroy(number_of_slices);

	return out_str.data;
}

// TODO: WHY ISN'T THIS IN NODES_SET.C ??
String TemporalNode_to_string(StreamGraph* sg, size_t node_idx) {

	TemporalNode* node = &sg->nodes.nodes[node_idx];

	String str = String_from_duplicate("Node ");
	String_append_formatted(&str, "%zu {\n", node_idx);
	String_push_str(&str, "\tIntervals=[\n");

	// Append the first interval
	String_push_str(&str, "\t\t");
	String interval_str = Interval_to_string(&node->presence.intervals[0]);
	String_concat_consume(&str, &interval_str);

	// Append the other intervals
	for (size_t i = 1; i < node->presence.nb_intervals; i++) {
		String_push_str(&str, " U ");
		interval_str = Interval_to_string(&node->presence.intervals[i]);
		String_concat_consume(&str, &interval_str);
	}
	String_push_str(&str, "\n\t]\n");
	String_push_str(&str, "\tNeighbours={\n\t\t");
	for (size_t i = 0; i < node->nb_neighbours; i++) {
		// get the names of the neighbours through the links
		size_t link_idx		 = node->neighbours[i];
		size_t node1		 = sg->links.links[link_idx].nodes[0];
		size_t node2		 = sg->links.links[link_idx].nodes[1];
		size_t neighbour_idx = (node1 == node_idx) ? node2 : node1;
		String_append_formatted(&str, "%zu, ", neighbour_idx);
	}
	String_push_str(&str, "\n\t}\n\n}\n");

	return str;
}

// TODO: SWITCH TO STRINGS GOD DAMN IT I HATE PAST ME
String StreamGraph_to_string(StreamGraph* sg) {
	String str = String_from_duplicate("StreamGraph {\n");
	String_append_formatted(&str, "\tLifespan=[%zu %zu[\n", StreamGraph_lifespan_begin(sg),
							StreamGraph_lifespan_end(sg));

	// Nodes
	String_push_str(&str, "\tNodes=[\n");
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		String node_str = TemporalNode_to_string(sg, i);
		String_concat_consume(&str, &node_str);
	}
	String_push_str(&str, "\t]\n");

	// Links
	String_push_str(&str, "\tLinks=[\n");
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		String link_str = Link_to_string(&sg->links.links[i]);
		String_concat_consume(&str, &link_str);
	}
	String_push_str(&str, "\t]\n");

	String_push_str(&str, "}\n");

	return str;
}

void StreamGraph_destroy(StreamGraph sg) {
	for (size_t i = 0; i < sg.nodes.nb_nodes; i++) {
		free(sg.nodes.nodes[i].neighbours);
		free(sg.nodes.nodes[i].presence.intervals);
	}
	free(sg.nodes.nodes);
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		free(sg.links.links[i].presence.intervals);
	}
	free(sg.links.links);
	KeyMomentsTable_destroy(sg.key_moments);

	// Free the events if they were initialized
}

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
	printf("initializing events table\n");
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
			size_t start	  = KeyMomentsTable_find_time_index(&sg->key_moments, interval.start);
			size_t end		  = KeyMomentsTable_find_time_index(&sg->key_moments, interval.end);
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
			size_t start	  = KeyMomentsTable_find_time_index(&sg->key_moments, interval.start);
			size_t end		  = KeyMomentsTable_find_time_index(&sg->key_moments, interval.end);
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

StreamGraph StreamGraph_from_external(const char* filename) {
	// printf("calling from_external\n");
	// read the file
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Error: could not open file %s\n", filename);
		exit(1);
	}

	// Read the file by seeking the end
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	rewind(file);

	// Read the file into a buffer
	char* buffer = (char*)malloc(file_size * sizeof(char) + 1);
	size_t read	 = fread(buffer, sizeof(char), file_size, file);
	if (read != file_size) {
		fprintf(stderr, "Error: could not read the file %s\n", filename);
		exit(1);
	}
	buffer[file_size] = '\0';

	// Close the file
	fclose(file);
	// printf("file read\n");

	// turn the external format into a stream graph
	char* internal_format = InternalFormat_from_External_str(buffer);
	// printf("internal_format done\n");

	StreamGraph sg = StreamGraph_from_string(internal_format);
	// printf("streamgraph done\n");
	free(internal_format);
	free(buffer);

	return sg;
}

void init_cache(Stream* stream) {
	stream->cache.cardinalOfW.present = false;
	stream->cache.cardinalOfT.present = false;
	stream->cache.cardinalOfE.present = false;
	stream->cache.cardinalOfV.present = false;
}

void reset_cache(Stream* stream) {
	stream->cache.cardinalOfW.present = false;
	stream->cache.cardinalOfT.present = false;
	stream->cache.cardinalOfE.present = false;
	stream->cache.cardinalOfV.present = false;
}
