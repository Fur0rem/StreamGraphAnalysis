/**
 * @file src/stream.c
 * @brief Implementation of the functions to create StreamGraph's and manipulate generic Stream's.
 */

#define SGA_INTERNAL

#include "units.h"
#include <stdarg.h>

#include "stream.h"

#include "stream_graph/key_instants_table.h"

#include <stdbool.h>
#include <stddef.h>

#include "defaults.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generic_data_structures/arraylist.h"
#include "generic_data_structures/hashset.h"
#include "interval.h"
#include "stream_functions.h"
#include "stream_graph/events_table.h"
#include "stream_graph/links_set.h"
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

#define NEXT_HEADER(section)                                                                                                               \
	current_header_hierarchy[0] = "" #section "";                                                                                      \
	current_header_hierarchy[1] = NULL;                                                                                                \
	current_header_hierarchy[2] = NULL;                                                                                                \
	(str)			    = get_to_header(str, current_header_hierarchy[0]);

#define NEXT_SUB_HEADER(subsection)                                                                                                        \
	current_header_hierarchy[1] = "" #subsection "";                                                                                   \
	current_header_hierarchy[2] = NULL;                                                                                                \
	(str)			    = get_to_header(str, current_header_hierarchy[1]);

#define NEXT_SUB_SUB_HEADER(subsubsection)                                                                                                 \
	current_header_hierarchy[2] = "" #subsubsection "";                                                                                \
	(str)			    = get_to_header(str, current_header_hierarchy[2]);

#define EXPECTED_NB_SCANNED(expected)                                                                                                      \
	if (nb_scanned != (expected)) {                                                                                                    \
		fprintf(stderr,                                                                                                            \
			TEXT_RED TEXT_BOLD "Could not parse the header " TEXT_RESET "%s\nError at line %d\n",                              \
			current_header,                                                                                                    \
			__LINE__);                                                                                                         \
		fprintf(stderr, "Number of scanned elements: %d, expected: %d\n", nb_scanned, expected);                                   \
		fprintf(stderr, "Current line:");                                                                                          \
		PRINT_LINE(str);                                                                                                           \
		exit(1);                                                                                                                   \
	}

#define GO_TO_NEXT_LINE(str)                                                                                                               \
	if (*(str) != '\0') {                                                                                                              \
		(str) = strchr((str), '\n');                                                                                               \
		if ((str) == NULL) {                                                                                                       \
			fprintf(stderr,                                                                                                    \
				TEXT_BOLD TEXT_RED "End of file was reached prematurely while trying "                                     \
						   "to find next line \n" TEXT_RESET);                                                     \
			exit(1);                                                                                                           \
		}                                                                                                                          \
		(str)++;                                                                                                                   \
	}                                                                                                                                  \
	else {                                                                                                                             \
		fprintf(stderr, "End of file was reached prematurely while trying to find next line \n");                                  \
		exit(1);                                                                                                                   \
	}

#define PRINT_LINE_ERR(str, column_of_err)                                                                                                 \
	{                                                                                                                                  \
		const char* begin = (str);                                                                                                 \
		while (*begin != '\n') {                                                                                                   \
			begin--;                                                                                                           \
		}                                                                                                                          \
		begin++;                                                                                                                   \
		char* end  = strchr(begin, '\n');                                                                                          \
		char* line = (char*)MALLOC(end - begin + 1);                                                                               \
		strncpy(line, begin, end - begin);                                                                                         \
		line[end - begin] = '\0';                                                                                                  \
		fprintf(stderr, "%s\n", line);                                                                                             \
		free(line);                                                                                                                \
		/* Print some red squiggly lines under it */                                                                               \
		for (size_t i = 1; i < (column_of_err); i++) {                                                                             \
			fprintf(stderr, " ");                                                                                              \
		}                                                                                                                          \
		fprintf(stderr, TEXT_RED "^" TEXT_RESET "\n");                                                                             \
	}

#define NEXT_TUPLE(str)                                                                                                                    \
	{                                                                                                                                  \
		if (*(str) == '\0') {                                                                                                      \
			fprintf(stderr,                                                                                                    \
				TEXT_RED TEXT_BOLD "End of file was reached prematurely while trying "                                     \
						   "to find next tuple \n" TEXT_RESET);                                                    \
			exit(1);                                                                                                           \
		}                                                                                                                          \
                                                                                                                                           \
		(str)++;                                                                                                                   \
		while ((*(str) != '(') && (*(str) != '\n') && (*(str) != '\0')) {                                                          \
			(str)++;                                                                                                           \
		}                                                                                                                          \
	}

/**
 * @brief Checks if a condition is true and prints an error message if it is not, along with some context information
 * @param[in] err_cond The condition to check
 * @param[in] err_cond_str The string representation of the condition
 * @param[in] where_it_stopped The position where the parsing stopped
 * @param[in] full_str The full string that was parsed
 * @param[in] src_code_file_name The name of the source code file
 * @param[in] src_code_line_number The line number in the source code
 * @param[in] current_header_hierarchy The hierarchy of the current header
 * @param[in] error_format The format of the error message
 * @param[in] ... The arguments to the format string
 */
void check_parse_error(bool err_cond, const char* err_cond_str, const char* where_it_stopped, const char* full_str,
		       const char* src_code_file_name, int src_code_line_number, const char* const current_header_hierarchy[3],
		       const char* error_format, ...) {

	if UNLIKELY (err_cond) {
		// Print the error message
		fprintf(stderr, TEXT_BOLD TEXT_RED "Error" TEXT_RESET " while parsing : ");
		va_list args;
		va_start(args, error_format);
		vfprintf(stderr, error_format, args);
		va_end(args);

		// Print the context of the file
		// Calculate the line and column
		size_t line	= 1;
		size_t column	= 1;
		const char* ptr = full_str;
		while (ptr != where_it_stopped) {
			if (*ptr == '\n') {
				line++;
				column = 1;
			}
			else {
				column++;
			}
			ptr++;
		}
		fprintf(stderr, "\nError in stream file line %zu, column %zu\nContents of faulty line:\n", line, column);
		// PRINT_LINE(stderr, where_it_stopped);
		PRINT_LINE_ERR(where_it_stopped, column);

		// Print additional information and source code reference
		fprintf(stderr,
			"Backtrace:\n\tCondition failed: %s\n\tSource code reference: %s:%d\n",
			err_cond_str,
			src_code_file_name,
			src_code_line_number);
		fprintf(stderr, "\tIn headers ");
		for (size_t i = 0; i < 3; i++) {
			if (current_header_hierarchy[i] == NULL) {
				break;
			}
			fprintf(stderr, " %s ", current_header_hierarchy[i]);
		}
		exit(1);
	}
}

/**
 * @brief Expects a character in a string and moves the pointer to the next character. Panics if the character is not found
 * @param[in, out] str The string to parse, gets modified to point to the next character after the expected character
 * @param[in] character The character to expect
 */
#define EXPECT_AND_MOVE(str, character)                                                                                                    \
	CHECK_PARSE_ERROR((str)[0] != (character), "Expected '%c', got '%c'\n", character, (str)[0]);                                      \
	(str) += 1;

/**
 * @brief Expects either character1 or character2 in a string and moves the pointer to the next character. Panics if neither character is
 * found
 * @param[in, out] str The string to parse, gets modified to point to the next character after the expected character
 * @param[in] character1 The first character to expect
 * @param[in] character2 The second character to expect
 */
#define EXPECT_EITHER_AND_MOVE(str, character1, character2)                                                                                \
	CHECK_PARSE_ERROR(                                                                                                                 \
	    (str)[0] != (character1) && (str)[0] != (character2), "Expected %c or %c, got %c\n", character1, character2, (str)[0]);        \
	(str) += 1;

/**
 * @brief Expects a sequence of characters in a string and moves the pointer to the next character. Panics if the sequence is not found
 * @param[in, out] str The string to parse, gets modified to point to the next character after the sequence
 * @param sequence The sequence to expect
 */
#define EXPECT_SEQUENCE_AND_MOVE(str, sequence)                                                                                            \
	if (strncmp(str, sequence, strlen(sequence)) != 0) {                                                                               \
		char* next_space = str;                                                                                                    \
		while (*next_space != ' ' && *next_space != '\n') {                                                                        \
			next_space++;                                                                                                      \
		}                                                                                                                          \
		char* trimmed = (char*)MALLOC(next_space - str + 1);                                                                       \
		strncpy(trimmed, str, next_space - str);                                                                                   \
		trimmed[next_space - str] = '\0';                                                                                          \
		CHECK_PARSE_ERROR(true, "Expected %s, got %s", sequence, trimmed);                                                         \
	}                                                                                                                                  \
	(str) += strlen(sequence);

void init_events_table(SGA_StreamGraph* sg);

/**
 * @brief A wrapper macro around check_parse_error that automatically fills some of the arguments
 * @param err_cond The condition to check
 * @param where_it_stopped The position where the parsing stopped
 * @param full_str The full string that was parsed
 * @param error_format The format of the error message
 * @param ... The arguments to the format string
 */
#define CHECK_PARSE_ERROR(err_cond, error_format, ...)                                                                                     \
	check_parse_error(                                                                                                                 \
	    err_cond, #err_cond, str, format->data, __FILE__, __LINE__, current_header_hierarchy, error_format, ##__VA_ARGS__);

/**
 * @brief Tries to parse a number in a string and moves the pointer to the character after the number. Panics if the number is not found
 * @param[in, out] str The string to parse, gets modified to point to the next character after the number
 * @param[in] ... The arguments to the format string for additional information in case of an error
 */
#define PARSE_NUMBER_AND_MOVE(str, ...)                                                                                                    \
	({                                                                                                                                 \
		char* new_ptr;                                                                                                             \
		long number = strtol(str, &new_ptr, 10);                                                                                   \
		if (str == new_ptr) {                                                                                                      \
			char* next_space = str;                                                                                            \
			while (*next_space != ' ' && *next_space != '\n') {                                                                \
				next_space++;                                                                                              \
			}                                                                                                                  \
			char* trimmed = (char*)MALLOC(next_space - str + 1);                                                               \
			strncpy(trimmed, str, next_space - str);                                                                           \
			trimmed[next_space - str] = '\0';                                                                                  \
			CHECK_PARSE_ERROR((str) == new_ptr, "Could not parse the " #__VA_ARGS__ " : Expected a number, got %s", trimmed);  \
		}                                                                                                                          \
		(str) = new_ptr;                                                                                                           \
		number;                                                                                                                    \
	})

SGA_StreamGraph SGA_StreamGraph_from_internal_format_v_1_0_0(const String* format) {
	SGA_StreamGraph sg;
	const char* current_header_hierarchy[3] = {
	    NULL,
	    NULL,
	    NULL,
	};
	char* str = format->data;

	// Skip first line (version control)

	// Parse the General section
	NEXT_HEADER([General]);

	// Parse the lifespan
	EXPECT_SEQUENCE_AND_MOVE(str, "Lifespan=(");
	size_t lifespan_start = PARSE_NUMBER_AND_MOVE(str, "start of the lifespan");
	EXPECT_AND_MOVE(str, ' ');
	size_t lifespan_end = PARSE_NUMBER_AND_MOVE(str, "end of the lifespan");
	EXPECT_SEQUENCE_AND_MOVE(str, ")\n");
	sg.lifespan = SGA_Interval_from(lifespan_start, lifespan_end);

	// Parse the time scale
	EXPECT_SEQUENCE_AND_MOVE(str, "TimeScale=");
	sg.time_scale = PARSE_NUMBER_AND_MOVE(str, "time scale");

	// Parse the Memory section
	NEXT_HEADER([Memory]);

	// Count how many slices are needed
	size_t nb_slices = (lifespan_end / SLICE_SIZE) + 1;

	// Parse the number of nodes
	EXPECT_SEQUENCE_AND_MOVE(str, "NumberOfNodes=");
	size_t nb_nodes = PARSE_NUMBER_AND_MOVE(str, "number of nodes");

	EXPECT_AND_MOVE(str, '\n');

	// Parse the number of links
	EXPECT_SEQUENCE_AND_MOVE(str, "NumberOfLinks=");
	size_t nb_links = PARSE_NUMBER_AND_MOVE(str, "number of links");

	EXPECT_AND_MOVE(str, '\n');

	// Parse the number of key instants
	EXPECT_SEQUENCE_AND_MOVE(str, "NumberOfKeyInstants=");
	size_t nb_key_instants = PARSE_NUMBER_AND_MOVE(str, "number of key instants");

	EXPECT_AND_MOVE(str, '\n');

	// Allocate the stream graph
	size_t* key_instants = MALLOC(nb_key_instants * sizeof(size_t));
	sg.key_instants	     = KeyInstantsTable_alloc(nb_slices);
	sg.events.nb_events  = nb_key_instants;
	sg.nodes	     = NodesSet_alloc(nb_nodes);
	sg.links	     = LinksSet_alloc(nb_links);

	// Parse the memory needed for the nodes
	NEXT_SUB_HEADER([[Nodes]]);

	// Parse the number of neighbours for each node
	NEXT_SUB_SUB_HEADER([[[NumberOfNeighbours]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		// Parse the number of neighbours
		size_t nb_neighbours = PARSE_NUMBER_AND_MOVE(str, "number of neighbours for node %zu", node);

		// Allocate the neighbours
		sg.nodes.nodes[node].nb_neighbours = nb_neighbours;
		sg.nodes.nodes[node].neighbours	   = MALLOC(nb_neighbours * sizeof(SGA_LinkId));

		// Next line
		EXPECT_AND_MOVE(str, '\n');
	}

	// Parse the number of intervals for each node
	NEXT_SUB_SUB_HEADER([[[NumberOfIntervals]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		// Parse the number of intervals
		size_t nb_intervals = PARSE_NUMBER_AND_MOVE(str, "number of presence intervals for node %zu", node);

		// Allocate the intervals
		sg.nodes.nodes[node].presence = SGA_IntervalsSet_alloc(nb_intervals);

		// Next line
		EXPECT_AND_MOVE(str, '\n');
	}

	// Parse the memory needed for the links
	NEXT_SUB_HEADER([[Links]]);

	// Parse the number of intervals for each link
	NEXT_SUB_SUB_HEADER([[[NumberOfIntervals]]]);
	for (size_t link = 0; link < nb_links; link++) {
		// Parse the number of intervals
		size_t nb_intervals = PARSE_NUMBER_AND_MOVE(str, "number of presence intervals for link %zu", link);

		// Allocate the intervals
		sg.links.links[link].presence = SGA_IntervalsSet_alloc(nb_intervals);

		// Next line
		EXPECT_AND_MOVE(str, '\n');
	}

	// Parse the key instants
	NEXT_SUB_SUB_HEADER([[[NumberOfKeyInstantsPerSlice]]]);

	// Parse the number of key instants in each slice
	for (size_t i = 0; i < nb_slices; i++) {
		// Parse the number of key instants in the slice
		size_t instants_in_slice = PARSE_NUMBER_AND_MOVE(str, "number of key instants in the slice %zu", i);

		// Allocate the slice
		KeyInstantsTable_alloc_slice(&sg.key_instants, i, instants_in_slice);

		// Next slice
		EXPECT_AND_MOVE(str, '\n');
	}

	// Parse the Data section
	NEXT_HEADER([Data]);

	// Parse the neighbours
	NEXT_SUB_HEADER([[Neighbours]]);

	// Parse the neighbouring links for each node
	NEXT_SUB_SUB_HEADER([[[NodesToLinks]]]);
	for (size_t node = 0; node < nb_nodes; node++) {
		// Find beginning of list of neighbouring links
		str = strchr(str, '(') + 1;

		// Parse the neighbouring links
		for (size_t j = 0; j < sg.nodes.nodes[node].nb_neighbours; j++) {
			// Get the link id
			size_t link = PARSE_NUMBER_AND_MOVE(str, "link id of the %zu-th neighbour of node %zu", j, node);

			// Store the link id
			sg.nodes.nodes[node].neighbours[j] = link;

			// Next link
			while ((*str != '(') && (*str != ' ') && (*str != '\n')) {
				str++;
			}
			str++;
		}
	}

	// Parse the two nodes each link connects
	NEXT_SUB_SUB_HEADER([[[LinksToNodes]]]);

	// Parse the two nodes each link connects
	for (size_t link = 0; link < nb_links; link++) {
		// Parse the tuple
		EXPECT_AND_MOVE(str, '(');
		size_t node1 = PARSE_NUMBER_AND_MOVE(str, "first node of link %zu", link);
		EXPECT_AND_MOVE(str, ' ');
		size_t node2 = PARSE_NUMBER_AND_MOVE(str, "second node of link %zu", link);

		// Store the nodes
		sg.links.links[link].nodes[0] = node1;
		sg.links.links[link].nodes[1] = node2;

		// End of tuple and go to next line
		EXPECT_SEQUENCE_AND_MOVE(str, ")\n");
	}

	// Parse the events
	NEXT_SUB_HEADER([[Events]]);
	size_t last_event_parsed = lifespan_start;

	// Arrays to keep track how many tiems a node/link was pushed and removed
	// OPTIMISATION: Both arrays are allocated in one calloc call to prevent two separate allocations and better cache locality
	int* buffer		 = calloc(nb_nodes + nb_links, sizeof(int));
	int* nb_pushed_for_nodes = buffer;
	int* nb_pushed_for_links = buffer + nb_nodes;

	// Parse all the lists of events
	for (size_t i = 0; i < nb_key_instants; i++) {
		size_t key_instant = PARSE_NUMBER_AND_MOVE(str, "%zu-th key instant", i);
		CHECK_PARSE_ERROR((key_instant <= last_event_parsed) && (key_instant != lifespan_start),
				  "Key instants are not sorted in increasing order (last event: %zu >= current event: %zu)\n",
				  last_event_parsed,
				  key_instant);

		// Push the key instant in the table
		KeyInstantsTable_push_in_order(&sg.key_instants, key_instant);
		last_event_parsed = key_instant;

		// Parse the list of events
		EXPECT_SEQUENCE_AND_MOVE(str, "=(");
		while (*str != '\n') {
			// Parse the next event
			EXPECT_AND_MOVE(str, '(');

			// Get whether its an appearance or disappearance
			char sign = *str;
			EXPECT_EITHER_AND_MOVE(str, '+', '-');

			EXPECT_AND_MOVE(str, ' ');

			// Get whether its a node or a link
			char letter = *str;
			EXPECT_EITHER_AND_MOVE(str, 'N', 'L');

			EXPECT_AND_MOVE(str, ' ');

			// Get the id of the node or link
			size_t id = PARSE_NUMBER_AND_MOVE(str, "id of the %s", letter == 'N' ? "node" : "link");
			// Add the node or link to the stream graph
			// Check if the id is in bounds and if the operation is valid (i.e. not two appearances or two disappearances)
			if (letter == 'N') {
				CHECK_PARSE_ERROR(id >= nb_nodes, "Node id %zu is out of bounds (%zu nodes announced)\n", id, nb_nodes);
				if (sign == '+') {
					CHECK_PARSE_ERROR(
					    nb_pushed_for_nodes[id] % 2 != 0, "Node %zu was pushed twice without being removed\n", id);
					sg.nodes.nodes[id].presence.intervals[nb_pushed_for_nodes[id] / 2].start = key_instant;
				}
				else {
					CHECK_PARSE_ERROR(
					    nb_pushed_for_nodes[id] % 2 == 0, "Node %zu was removed twice without being added\n", id);
					sg.nodes.nodes[id].presence.intervals[nb_pushed_for_nodes[id] / 2].end = key_instant;
				}
				nb_pushed_for_nodes[id]++;
			}
			else {
				CHECK_PARSE_ERROR(id >= nb_links, "Link id %zu is out of bounds (%zu links announced)\n", id, nb_links);
				if (sign == '+') {
					CHECK_PARSE_ERROR(
					    nb_pushed_for_links[id] % 2 != 0, "Link %zu was pushed twice without being removed\n", id);
					sg.links.links[id].presence.intervals[nb_pushed_for_links[id] / 2].start = key_instant;
				}
				else {
					CHECK_PARSE_ERROR(
					    nb_pushed_for_links[id] % 2 == 0, "Link %zu was removed twice without being added\n", id);
					sg.links.links[id].presence.intervals[nb_pushed_for_links[id] / 2].end = key_instant;
				}
				nb_pushed_for_links[id]++;
			}

			// Go to the next event
			str += 2;
		}

		// Go to the next line
		EXPECT_AND_MOVE(str, '\n');
	}

	// Check if each appearance has a corresponding disappearance
	for (SGA_NodeId n = 0; n < nb_nodes; n++) {
		CHECK_PARSE_ERROR(nb_pushed_for_nodes[n] % 2 != 0,
				  "Node %zu doesn't have a balanced number of appearances and disappearances (%zu <- Should be even), "
				  "perhaps you forgot to make it disappear at the end of the stream?\n",
				  n,
				  nb_pushed_for_nodes[n]);
	}
	for (SGA_LinkId l = 0; l < nb_links; l++) {
		CHECK_PARSE_ERROR(nb_pushed_for_links[l] % 2 != 0,
				  "Link %zu doesn't have a balanced number of appearances and disappearances (%zu <- Should be even), "
				  "perhaps you forgot to make it disappear at the end of the stream?\n",
				  l,
				  nb_pushed_for_links[l]);
	}

	// Close and clean up
	NEXT_HEADER([EndOfStream]);
	free(buffer);
	free(key_instants);

	init_events_table(&sg);

	return sg;
}

SGA_StreamGraph SGA_StreamGraph_from_file(const char* filename) {
	String file_content = String_from_file(filename);
	SGA_StreamGraph sg  = SGA_StreamGraph_from_string(&file_content);
	String_destroy(file_content);
	return sg;
}

size_t StreamGraph_lifespan_begin(SGA_StreamGraph* sg) {
	return KeyInstantsTable_first_instant(&sg->key_instants);
}

size_t StreamGraph_lifespan_end(SGA_StreamGraph* sg) {
	return KeyInstantsTable_last_instant(&sg->key_instants);
}

/**
 * @brief A single event, signifies the appearance of disappearance of a certain link or node
 */
typedef struct {
	size_t instant; ///< The instant at which the event occurs
	char sign;	///< '+' for appearance, '-' for disappearance
	char letter;	///< 'N' for node, 'L' for link
	union {
		size_t node; ///< The id of the node
		struct {
			size_t node1; ///< The id of the first node of the link
			size_t node2; ///< The id of the second node of the link
		} link;		      ///< A link is represented by two nodes
	} id;			      ///< The id of the node or link
} EventTuple;

String EventTuple_to_string(EventTuple* tuple) {
	String str = String_with_capacity(50);
	switch (tuple->letter) {
		case 'N': {
			String_append_formatted(&str, "(%zu %c %c %zu)", tuple->instant, tuple->sign, tuple->letter, tuple->id.node);
			break;
		}
		case 'L': {
			String_append_formatted(&str,
						"(%zu %c %c %zu %zu)",
						tuple->instant,
						tuple->sign,
						tuple->letter,
						tuple->id.link.node1,
						tuple->id.link.node2);
			break;
		}
		default: {
			UNREACHABLE_CODE;
		}
	}
	return str;
}

DeclareArrayList(EventTuple);
DefineArrayList(EventTuple);
DeclareArrayList(size_tHashset);
DefineArrayList(size_tHashset);

DeclareArrayList(EventTupleArrayList);
DefineArrayList(EventTupleArrayList);
NO_FREE(EventTuple);
DefineArrayListDeriveRemove(EventTuple);
DefineArrayListDeriveRemove(EventTupleArrayList);

size_t link_hash(SGA_Link key) {
	return key.nodes[0] + key.nodes[1];
}

typedef struct {
	size_t nodes[2];
	size_t nb_intervals;
} LinkInfo;

String LinkInfo_to_string(LinkInfo* info) {
	char* str = (char*)MALLOC(50);
	sprintf(str, "(%zu %zu) %zu", info->nodes[0], info->nodes[1], info->nb_intervals);
	return String_from_owned(str);
}

bool LinkInfo_equals(LinkInfo info1, LinkInfo info2) {
	return info1.nodes[0] == info2.nodes[0] && info1.nodes[1] == info2.nodes[1];
}

DeclareArrayList(LinkInfo);
DefineArrayList(LinkInfo);

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

DeclareArrayList(LinkIdMap);
DefineArrayList(LinkIdMap);
DeclareHashset(LinkIdMap);

String LinkIdMap_to_string(LinkIdMap* map) {
	char* str = (char*)MALLOC(50);
	sprintf(str, "(%zu %zu) %zu", map->nodes[0], map->nodes[1], map->id);
	return String_from_owned(str);
}

DefineHashset(LinkIdMap);

DefineArrayListDeriveRemove(size_tHashset);
NO_FREE(LinkInfo);
DefineArrayListDeriveRemove(LinkInfo);

size_t nb_characters_needed(size_t number) {
	const size_t BASE = 10;

	size_t nb_chars = 1;
	while (number >= BASE) {
		number /= BASE;
		nb_chars++;
	}

	if (nb_chars == 1) {
		return 2;
	}
	return nb_chars;
}

// An estimation of the size of the output string for a stream internal format (slightly overestimated most of
// the time)
size_t estimate_internal_format_v_1_0_0_size(size_t nb_nodes, size_t nb_links, SGA_Time last_event, size_t nb_events,
					     size_tArrayList nb_events_per_slice) {
	// Most of these are magic functions with some trial and error on a few examples
	// I had between ~2% and ~30% of error on the size estimation
	// The additions with random 2's, 3's, 4's are to account for the space, newline, and parenthesis

	// The headers + the number of events in each slice * the number of slices
	size_t headers_size =
	    strlen("[General]\nLifespan=(%zu "
		   "%zu)\nTimeScale=%zu\n\n[Memory]\nNumberOfNodes=%zu\nNumberOfLinks=%zu\nNumberOfKeyInstants=%"
		   "zu\n\n[["
		   "Nodes]"
		   "]\n[[[NumberOfNeighbours]]]\n[[[NumberOfIntervals]]]\n[[Links]]\n[[[NumberOfIntervals]]]\n[[["
		   "NumberOfKeyInstantsPerSlice]]]\n[Data]\n[[Neighbours]]\n[[[NodesToLinks]]]\n[[[LinksToNodes]]]\n[[Events]]"
		   "\n") +
	    100 + (nb_events_per_slice.length * (nb_characters_needed(nb_events / nb_events_per_slice.length)));

	// The number of neighbours for each node
	size_t neighbours_per_node_prediction = (nb_links + nb_links / 2) / nb_nodes;

	// The number of intervals for each node and link (appearance + disappearance) = 1 interval
	size_t number_of_intervals = ((nb_characters_needed(nb_events / 8) + 1) * (nb_nodes + nb_links)) / 2;

	// The number of neighbours for each node to links and links to nodes
	size_t neighbours_nodes_to_links_size = ((nb_characters_needed(nb_links) + 2) * neighbours_per_node_prediction + 3) * nb_nodes;
	size_t neighbours_links_to_nodes_size =
	    (4 + (((nb_characters_needed(nb_nodes) - 1) * 2) + nb_characters_needed(nb_links) - 1)) * nb_links;

	// The size of the events with (sign letter link_id) or (sign letter node_id)
	size_t chars_per_event	      = (1 + nb_characters_needed((nb_nodes > nb_links) ? nb_nodes : nb_links)) + strlen(" (X X X)") + 2;
	size_t events_size_prediction = ((nb_characters_needed(last_event) - 1 + chars_per_event) * nb_events);
	size_t number_of_neighbours_prediction = (nb_characters_needed(neighbours_per_node_prediction) + 1) * nb_nodes;

	// Sum of everything
	size_t size_prediction = headers_size + (number_of_neighbours_prediction) + number_of_intervals + neighbours_nodes_to_links_size +
				 neighbours_links_to_nodes_size + events_size_prediction;

	return size_prediction;
}

/**
 * @brief Structure for parsing the external format of a stream graph
 */
// TODO: Better name
typedef struct {
	SGA_Interval lifespan;			///< The lifespan of the stream graph
	size_t time_scale;			///< The time scale of the stream graph
	LinkInfoArrayList links;		///< The links of the stream graph
	LinkInfoArrayList nodes;		///< The nodes of the stream graph
	size_tHashsetArrayList node_neighbours; ///< The neighbours of the nodes
	EventTupleArrayListArrayList events;	///< The events of the stream graph
	size_tArrayList nb_events_per_slice;	///< The number of events per slice
} ParsedStreamGraph;

/**
 * @brief Parse the external format of a stream graph
 * @param[in] format The external format of the stream graph
 * @return The parsed stream graph
 */
ParsedStreamGraph StreamGraph_parse_from_external_format_v_1_0_0(const String* format) {

	// Parse the external format
	const char* current_header_hierarchy[3] = {
	    NULL,
	    NULL,
	    NULL,
	};
	char* str = format->data;

	// Parse the General section
	NEXT_HEADER([General]);

	// Parse the lifespan
	EXPECT_SEQUENCE_AND_MOVE(str, "Lifespan=(");
	size_t lifespan_start = PARSE_NUMBER_AND_MOVE(str, "start of the lifespan");
	EXPECT_AND_MOVE(str, ' ');
	size_t lifespan_end = PARSE_NUMBER_AND_MOVE(str, "end of the lifespan");
	EXPECT_SEQUENCE_AND_MOVE(str, ")\n");

	// Parse the time scale
	EXPECT_SEQUENCE_AND_MOVE(str, "TimeScale=");
	size_t time_scale = PARSE_NUMBER_AND_MOVE(str, "time scale");
	EXPECT_AND_MOVE(str, '\n');

	// Parse the Events section
	NEXT_HEADER([Events]);

	// Keep track of the events parsed
	EventTupleArrayListArrayList events    = EventTupleArrayListArrayList_new();
	size_tHashsetArrayList node_neighbours = size_tHashsetArrayList_new();
	LinkInfoArrayList links		       = LinkInfoArrayList_new();
	LinkInfoArrayList nodes		       = LinkInfoArrayList_new();
	size_t biggest_node_id		       = 0;
	size_t nb_events		       = 0;
	size_t current_vec		       = 0;
	size_tArrayList nb_events_per_slice    = size_tArrayList_new();

	// To check if the events are in order
	size_t last_event_parsed = lifespan_start;

	// Parse the events while the end of the stream is not reached
	const char* END_OF_STREAM = "[EndOfStream]";
	while (strncmp(str, END_OF_STREAM, strlen(END_OF_STREAM)) != 0) {

		// If the line is empty, skip it
		if (*str == '\n') {
			str++;
			continue;
		}

		// Parse the event 4-tuple (instant, sign, letter, id)
		EventTuple tuple;

		// Instant
		size_t key_instant = PARSE_NUMBER_AND_MOVE(str, "key instant");
		CHECK_PARSE_ERROR(key_instant < lifespan_start || key_instant > lifespan_end,
				  "Event %zu is out of bounds of the lifespan of the stream [%zu, %zu)\n",
				  key_instant,
				  lifespan_start,
				  lifespan_end);
		CHECK_PARSE_ERROR(key_instant < last_event_parsed,
				  "Events are not sorted in increasing order (last event: %zu > current event: %zu)\n",
				  last_event_parsed,
				  key_instant);

		// printf("Parsed Key instant: %zu\n", key_instant);

		EXPECT_AND_MOVE(str, ' ');

		// Sign
		char sign = *str;
		CHECK_PARSE_ERROR(sign != '+' && sign != '-', "Could not parse the sign %c", sign);
		str++;

		EXPECT_AND_MOVE(str, ' ');

		// Letter
		char letter = *str;
		CHECK_PARSE_ERROR(letter != 'N' && letter != 'L', "Could not parse the letter %c", letter);
		str++;

		EXPECT_AND_MOVE(str, ' ');

		// IDs
		// One node id for a node
		if (letter == 'N') {
			size_t node_id = PARSE_NUMBER_AND_MOVE(str, "node id");

			tuple = (EventTuple){
			    .instant = key_instant,
			    .sign    = sign,
			    .letter  = letter,
			    .id.node = node_id,
			};

			// Initialise the rest of the structures that keep track of nodes in the array if you are the new biggest node id
			if (node_id > biggest_node_id) {
				biggest_node_id = node_id;
				for (size_t i = nodes.length; i <= biggest_node_id; i++) {
					LinkInfo info = {
					    .nodes	  = {i, i},
					    .nb_intervals = 0,
					};
					LinkInfoArrayList_push(&nodes, info);
				}
				for (size_t i = node_neighbours.length; i <= biggest_node_id; i++) {
					size_tHashsetArrayList_push(&node_neighbours, size_tHashset_new());
				}
			}

			LinkInfo info = {
			    .nodes	  = {node_id, node_id},
			    .nb_intervals = 1,
			};

			// Increment the number of intervals for the node
			bool found = false;
			for (size_t i = 0; i < nodes.length; i++) {
				if (LinkInfo_equals(nodes.array[i], info)) {
					nodes.array[i].nb_intervals++;
					found = true;
					break;
				}
			}
			if (!found) {
				LinkInfoArrayList_push(&nodes, info);
			}
		}
		else { // letter == 'L'
			// Parse the two node ids for a link
			size_t node1 = PARSE_NUMBER_AND_MOVE(str, "first node");
			EXPECT_AND_MOVE(str, ' ');
			size_t node2 = PARSE_NUMBER_AND_MOVE(str, "second node");

			CHECK_PARSE_ERROR(node1 == node2, "Nodes are the same");

			tuple = (EventTuple){
			    .instant	   = key_instant,
			    .sign	   = sign,
			    .letter	   = letter,
			    .id.link.node1 = node1,
			    .id.link.node2 = node2,
			};

			// Initialise the rest of the structures that keep track of nodes in the array if you are the new biggest node id
			if (node1 > biggest_node_id) {
				biggest_node_id = node1;
			}
			if (node2 > biggest_node_id) {
				biggest_node_id = node2;
			}
			for (size_t i = node_neighbours.length; i <= biggest_node_id; i++) {
				size_tHashsetArrayList_push(&node_neighbours, size_tHashset_new());
			}
			size_tHashset_insert(&node_neighbours.array[node1], node2);
			size_tHashset_insert(&node_neighbours.array[node2], node1);

			LinkInfo info = {
			    .nodes	  = {node1, node2},
			    .nb_intervals = 1,
			};

			// Increment the number of intervals for the link
			bool found = false;
			for (size_t i = 0; i < links.length; i++) {
				if (LinkInfo_equals(links.array[i], info)) {
					links.array[i].nb_intervals++;
					found = true;
					break;
				}
			}
			if (!found) {
				LinkInfoArrayList_push(&links, info);
			}
		}

		// If the time instant is a new one, move on to next event and create a new list
		// Otherwise, add the event to the current list
		if (nb_events == 0) {
			EventTupleArrayList events_vec = EventTupleArrayList_new();
			EventTupleArrayList_push(&events_vec, tuple);
			EventTupleArrayListArrayList_push(&events, events_vec);
		}
		else {
			// If the key instant is the same as the previous event, add it to the current list
			if (key_instant == events.array[current_vec].array[0].instant) {
				EventTupleArrayList_push(&events.array[current_vec], tuple);
			}
			else {
				// If the key instant is different, create a new list of events
				EventTupleArrayList events_vec = EventTupleArrayList_new();
				EventTupleArrayList_push(&events_vec, tuple);
				EventTupleArrayListArrayList_push(&events, events_vec);
				current_vec++;
			}
		}

		// Move to next event
		nb_events++;
		last_event_parsed = key_instant;
		EXPECT_AND_MOVE(str, '\n');
	}

	// printf("Parsed %zu events\n", nb_events);

	// Push the lifespan end event if it is not the last event
	// printf("Lifespan end: %zu\n", lifespan_end);
	// printf("events length: %zu\n", events.length);
	size_t last_event = events.array[events.length - 1].array[0].instant;
	if (last_event != lifespan_end) {
		last_event	= lifespan_end;
		size_t slice_id = last_event / SLICE_SIZE;
		if (slice_id >= nb_events_per_slice.length) {
			for (size_t j = nb_events_per_slice.length; j <= slice_id; j++) {
				size_tArrayList_push(&nb_events_per_slice, 0);
			}
		}
		nb_events_per_slice.array[slice_id]++;
	}

	// Update the number of events per slice
	for (size_t i = 0; i < events.length; i++) {
		size_t slice_id = events.array[i].array[0].instant / SLICE_SIZE;
		if (slice_id >= nb_events_per_slice.length) {
			for (size_t j = nb_events_per_slice.length; j <= slice_id; j++) {
				size_tArrayList_push(&nb_events_per_slice, 0);
			}
		}
		nb_events_per_slice.array[slice_id]++;
	}

	// If the last event is not the lifespan end, remove the last event
	// printf("Last event: %zu, lifespan end: %zu\n", last_event, lifespan_end);
	// if (last_event != lifespan_end) {
	// 	printf("Last event is not the lifespan end, removing it\n");
	// 	// Remove the last event from the last slice
	// 	if (nb_events_per_slice.length > 0) {
	// 		nb_events_per_slice.array[nb_events_per_slice.length - 1]--;
	// 	}
	// }

	// Create the parsed stream graph
	ParsedStreamGraph parsed = {
	    .lifespan		 = SGA_Interval_from(lifespan_start, lifespan_end),
	    .time_scale		 = time_scale,
	    .links		 = links,
	    .nodes		 = nodes,
	    .node_neighbours	 = node_neighbours,
	    .events		 = events,
	    .nb_events_per_slice = nb_events_per_slice,
	};

	return parsed;
}

String internal_format_v_1_0_0_from_parsed(ParsedStreamGraph parsed) {
	// String to store the internal format
	size_t size_prediction = estimate_internal_format_v_1_0_0_size(parsed.nodes.length,
								       parsed.links.length,
								       parsed.events.array[parsed.events.length - 1].array[0].instant,
								       parsed.events.length,
								       parsed.nb_events_per_slice);
	String str	       = String_with_capacity(size_prediction);

	// Write the internal format

	String_push_str(&str, "SGA Internal version 1.0.0\n");

	// Write the General section
	String_push_str(&str, "\n\n[General]\n");
	String_append_formatted(&str, "Lifespan=(%zu %zu)\n", parsed.lifespan.start, parsed.lifespan.end);
	String_append_formatted(&str, "TimeScale=%zu\n", parsed.time_scale);

	// Write the Memory section
	String_push_str(&str, "\n\n[Memory]\n");
	String_append_formatted(&str, "NumberOfNodes=%zu\n", parsed.nodes.length);
	String_append_formatted(&str, "NumberOfLinks=%zu\n", parsed.links.length);
	String_append_formatted(&str, "NumberOfKeyInstants=%zu\n", parsed.events.length);

	// Write the Nodes subsection
	String_push_str(&str, "\n[[Nodes]]\n");

	// Write the number of neighbours
	String_push_str(&str, "[[[NumberOfNeighbours]]]\n");
	for (size_t i = 0; i < parsed.nodes.length; i++) {
		String_append_formatted(&str, "%zu\n", size_tHashset_nb_elems(&parsed.node_neighbours.array[i]));
	}
	String_push_str(&str, "[[[NumberOfIntervals]]]\n");
	for (size_t i = 0; i < parsed.nodes.length; i++) {
		String_append_formatted(&str, "%zu\n", parsed.nodes.array[i].nb_intervals / 2);
	}

	// Write the Links subsection
	String_push_str(&str, "\n[[Links]]\n");

	// Write the number of intervals
	String_push_str(&str, "[[[NumberOfIntervals]]]\n");
	for (size_t i = 0; i < parsed.links.length; i++) {
		String_append_formatted(&str, "%zu\n", parsed.links.array[i].nb_intervals / 2);
	}

	String_push_str(&str, "[[[NumberOfKeyInstantsPerSlice]]]\n");
	// If the last key instant is not the lifespan end, it's not a real event.
	// Therefore decrease by 1 the number of events in the last slice
	if (parsed.events.array[parsed.events.length - 1].array[0].instant != parsed.lifespan.end) {
		parsed.nb_events_per_slice.array[parsed.nb_events_per_slice.length - 1]--;
	}

	for (size_t i = 0; i < parsed.nb_events_per_slice.length; i++) {
		String_append_formatted(&str, "%zu\n", parsed.nb_events_per_slice.array[i]);
	}

	// Write the Data section
	String_push_str(&str, "\n\n[Data]\n");

	// Write the Neighbours subsection
	String_push_str(&str, "\n[[Neighbours]]\n");

	// Write the NodesToLinks subsubsection
	String_push_str(&str, "[[[NodesToLinks]]]\n");
	for (size_t i = 0; i < parsed.nodes.length; i++) {
		String_push_str(&str, "(");
		size_tHashset neighbours = parsed.node_neighbours.array[i];
		FOR_EACH_ELEM(size_t, neighbour, neighbours, {
			// Find the id of the link with the neighbour
			size_t link_id = SIZE_MAX;
			for (size_t j = 0; j < parsed.links.length; j++) {
				if ((parsed.links.array[j].nodes[0] == i && parsed.links.array[j].nodes[1] == neighbour) ||
				    (parsed.links.array[j].nodes[0] == neighbour && parsed.links.array[j].nodes[1] == i)) {
					link_id = j;
					break;
				}
			}
			DEV_ASSERT(link_id != SIZE_MAX);
			String_append_formatted(&str, "%zu ", link_id);
		})
		if (str.data[str.size - 1] != '(') {
			String_pop(&str);
		}
		String_push_str(&str, ")\n");
	}

	// Write the LinksToNodes subsubsection
	String_push_str(&str, "[[[LinksToNodes]]]\n");
	for (size_t i = 0; i < parsed.links.length; i++) {
		String_append_formatted(&str, "(%zu %zu)\n", parsed.links.array[i].nodes[0], parsed.links.array[i].nodes[1]);
	}

	// Write the Events subsection
	String_push_str(&str, "\n[[Events]]\n");

	// Create a map to find the link id from the two node ids
	// OPTIMISATION: It's a 2d symmetric matrix, so we store only the upper triangle as a 1d array
	size_t* link_id_map = MALLOC((parsed.nodes.length) * (parsed.nodes.length) * sizeof(size_t));
	for (size_t i = 0; i < parsed.links.length; i++) {
		link_id_map[(parsed.links.array[i].nodes[0] * (parsed.nodes.length)) + parsed.links.array[i].nodes[1]] = i;
		link_id_map[(parsed.links.array[i].nodes[1] * (parsed.nodes.length)) + parsed.links.array[i].nodes[0]] = i;
	}

	// Write the events
	for (size_t i = 0; i < parsed.events.length; i++) {
		String_append_formatted(&str, "%zu=(", parsed.events.array[i].array[0].instant);
		for (size_t j = 0; j < parsed.events.array[i].length; j++) {
			if (parsed.events.array[i].array[j].letter == 'L') {
				EventTuple event = parsed.events.array[i].array[j];
				size_t idx1	 = event.id.link.node1;
				size_t idx2	 = event.id.link.node2;
				size_t link_id	 = link_id_map[(idx1 * (parsed.nodes.length)) + idx2];
				String_append_formatted(&str, "(%c %c %zu) ", event.sign, event.letter, link_id);
			}
			else {
				EventTuple event = parsed.events.array[i].array[j];
				String_append_formatted(&str, "(%c %c %zu) ", event.sign, event.letter, event.id.node);
			}
		}
		String_pop(&str);
		String_push_str(&str, ")\n");
	}

	String_push_str(&str, "\n\n[EndOfStream]\0");

	// Clean up
	free(link_id_map);
	size_tHashsetArrayList_destroy(parsed.node_neighbours);
	EventTupleArrayListArrayList_destroy(parsed.events);
	LinkInfoArrayList_destroy(parsed.links);
	LinkInfoArrayList_destroy(parsed.nodes);
	size_tArrayList_destroy(parsed.nb_events_per_slice);

	return str;
}

String SGA_external_v_1_0_0_to_internal_v_1_0_0_format(const String* format) {
	ParsedStreamGraph parsed = StreamGraph_parse_from_external_format_v_1_0_0(format);
	String internal_format	 = internal_format_v_1_0_0_from_parsed(parsed);
	return internal_format;
}

// TODO: WHY ISN'T THIS IN NODES_SET.C ??
// TODO: it's because i need the neighbours and all of that...
String SGA_Node_to_string(SGA_StreamGraph* sg, size_t node_idx) {

	SGA_Node* node = &sg->nodes.nodes[node_idx];

	String str = String_from_duplicate("Node ");
	String_append_formatted(&str, "%zu {\n", node_idx);
	String_push_str(&str, "\tPresence : ");

	String presence_str = SGA_IntervalsSet_to_string(&node->presence);
	String_concat_consume(&str, presence_str);

	String_push_str(&str, "\n\tNeighbours : [\n\t\t");
	for (size_t i = 0; i < node->nb_neighbours; i++) {
		// get the names of the neighbours through the links
		size_t link_idx	     = node->neighbours[i];
		size_t node1	     = sg->links.links[link_idx].nodes[0];
		size_t node2	     = sg->links.links[link_idx].nodes[1];
		size_t neighbour_idx = (node1 == node_idx) ? node2 : node1;
		String_append_formatted(&str, "%zu, ", neighbour_idx);
	}
	String_push_str(&str, "\n\t]\n\n}");

	return str;
}

String SGA_StreamGraph_to_string(SGA_StreamGraph* sg) {
	String str = String_from_duplicate("StreamGraph {\n");
	String_append_formatted(&str, "\tLifespan=[%zu %zu[\n", StreamGraph_lifespan_begin(sg), StreamGraph_lifespan_end(sg));

	// SGA_Nodes
	String_push_str(&str, "\tNodes=[\n");
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		String node_str = SGA_Node_to_string(sg, i);
		String_push(&node_str, '\n');
		String_concat_consume(&str, node_str);
	}
	String_push_str(&str, "\t]\n");

	// Links
	String_push_str(&str, "\tLinks=[\n");
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		String link_str = SGA_Link_to_string(&sg->links.links[i]);
		String_push(&link_str, '\n');
		String_concat_consume(&str, link_str);
	}
	String_push_str(&str, "\t]\n");

	String_push_str(&str, "}\n");

	return str;
}

void events_destroy(SGA_StreamGraph* sg);

void SGA_StreamGraph_destroy(SGA_StreamGraph sg) {
	for (size_t i = 0; i < sg.nodes.nb_nodes; i++) {
		free(sg.nodes.nodes[i].neighbours);
		free(sg.nodes.nodes[i].presence.intervals);
	}
	free(sg.nodes.nodes);
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		free(sg.links.links[i].presence.intervals);
	}
	free(sg.links.links);
	KeyInstantsTable_destroy(sg.key_instants);
	events_destroy(&sg);
}

void events_table_write(SGA_StreamGraph* sg, size_tArrayList* node_events, size_tArrayList* link_events) {
	// Realloc the arraylists into the events
	sg->events.node_events.events = MALLOC(sizeof(Event) * sg->events.nb_events);
	sg->events.link_events.events = MALLOC(sizeof(Event) * sg->events.nb_events);

	// Write the events
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		Event node_event;
		node_event.nb_info = node_events[i].length;
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
		link_event.nb_info = link_events[i].length;
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
void init_events_table(SGA_StreamGraph* sg) {
	// printf("initializing events table\n");
	// only the number of events is known and node and link presence intervals are known

	// Find the index of the last time a node appears
	size_t last_node_addition = 0;
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		// Skip nodes that never appear
		if (sg->nodes.nodes[i].presence.nb_intervals == 0) {
			continue;
		}

		size_t last_node_time = SGA_IntervalsSet_last(&sg->nodes.nodes[i].presence).start;
		if (last_node_time > last_node_addition) {
			last_node_addition = last_node_time;
		}
	}

	size_t last_link_addition = 0;
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		// Skip links that never appear
		if (sg->links.links[i].presence.nb_intervals == 0) {
			continue;
		}
		size_t last_link_time = SGA_IntervalsSet_last(&sg->links.links[i].presence).start;
		if (last_link_time > last_link_addition) {
			last_link_addition = last_link_time;
		}
	}

	size_t index_of_last_node_addition = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, last_node_addition);
	size_t index_of_last_link_addition = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, last_link_addition);

	// printf("last_node_addition: %zu\n", last_node_addition);
	// printf("last_link_addition: %zu\n", last_link_addition);
	// printf("index_of_last_node_addition: %zu\n", index_of_last_node_addition);
	// printf("index_of_last_link_addition: %zu\n", index_of_last_link_addition);

	sg->events.node_events.disappearance_index = index_of_last_node_addition;
	sg->events.link_events.disappearance_index = index_of_last_link_addition;

	// Allocate the accumulator for the events
	// printf("nb_events: %zu\n", sg->events.nb_events);
	size_tArrayList* node_events = MALLOC(sizeof(size_tArrayList) * sg->events.nb_events);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		node_events[i] = size_tArrayList_new();
	}

	// Fill the events
	sg->events.node_events.presence_mask = BitArray_n_ones(sg->events.node_events.disappearance_index);

	// For each node
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		SGA_Node* node = &sg->nodes.nodes[i];
		// For each interval
		for (size_t j = 0; j < node->presence.nb_intervals; j++) {
			SGA_Interval interval = node->presence.intervals[j];
			size_t start	      = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.start);
			size_t end	      = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.end);
			// Invalidate the bit of the presence mask
			if (end < sg->events.node_events.disappearance_index) {
				// printf("invalidating %zu\n", end);
				BitArray_set_zero(sg->events.node_events.presence_mask, end - 1);
			}
			// Push the creation
			size_tArrayList_push(&node_events[start], i);

			// Push the deletion if
			if (end >= sg->events.node_events.disappearance_index) {
				size_tArrayList_push(&node_events[end], i);
			}
		}
	}

	// Do the same for links
	sg->events.link_events.presence_mask = BitArray_n_ones(sg->events.link_events.disappearance_index + 1);

	size_tArrayList* link_events = MALLOC(sizeof(size_tArrayList) * sg->events.nb_events);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		link_events[i] = size_tArrayList_new();
	}

	for (size_t i = 0; i < sg->links.nb_links; i++) {
		SGA_Link* link = &sg->links.links[i];
		for (size_t j = 0; j < link->presence.nb_intervals; j++) {
			SGA_Interval interval = link->presence.intervals[j];
			size_t start	      = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.start);
			size_t end	      = KeyInstantsTable_find_time_index_if_pushed(&sg->key_instants, interval.end);
			if (end < sg->events.link_events.disappearance_index) {
				BitArray_set_zero(sg->events.link_events.presence_mask, end - 1);
			}
			size_tArrayList_push(&link_events[start], i);
			if (end > sg->events.link_events.disappearance_index) {
				size_tArrayList_push(&link_events[end], i);
			}
		}
	}

	for (SGA_TimeId i = 1; i < sg->events.node_events.disappearance_index; i++) {
		if (BitArray_is_zero(sg->events.node_events.presence_mask, i - 1)) {
			for (int j = i - 2; j >= 0; j--) {
				if ((j == 0) || (BitArray_is_one(sg->events.node_events.presence_mask, j - 1))) {
					for (size_t k = 0; k < node_events[j].length; k++) {
						if (SGA_IntervalsSet_contains_sorted(
							sg->nodes.nodes[node_events[j].array[k]].presence,
							KeyInstantsTable_nth_key_instant(&sg->key_instants, i))) {
							size_tArrayList_push(&node_events[i], node_events[j].array[k]);
						}
					}
				}
				else {
					break;
				}
			}
		}
	}

	// Do the same for links
	for (size_t i = 1; i < sg->events.link_events.disappearance_index; i++) {
		if (BitArray_is_zero(sg->events.link_events.presence_mask, i - 1)) {
			for (int j = i - 2; j >= 0; j--) {
				if ((j == 0) || (BitArray_is_one(sg->events.link_events.presence_mask, j - 1))) {
					for (size_t k = 0; k < link_events[j].length; k++) {

						// FIXME: WHY DO I HAVE TO COMPILE TWICE TO HAVE THE PERFORMANCE BOOST?
						if (SGA_IntervalsSet_contains_sorted(
							sg->links.links[link_events[j].array[k]].presence,
							KeyInstantsTable_nth_key_instant(&sg->key_instants, i))) {
							size_tArrayList_push(&link_events[i], link_events[j].array[k]);
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

	// Write the events
	events_table_write(sg, node_events, link_events);
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		size_tArrayList_destroy(node_events[i]);
		size_tArrayList_destroy(link_events[i]);
	}
	free(node_events);
	free(link_events);
}

void print_events_table(SGA_StreamGraph* sg) {
	for (size_t i = 0; i < sg->events.nb_events; i++) {
		printf("Event %zu\n", i);
		printf("Node events: ");
		for (size_t j = 0; j < sg->events.node_events.events[i].nb_info; j++) {
			printf("%zu ", sg->events.node_events.events[i].events[j]);
		}
		printf("\n");
		printf("Link events: ");
		for (size_t j = 0; j < sg->events.link_events.events[i].nb_info; j++) {
			printf("%zu ", sg->events.link_events.events[i].events[j]);
		}
		printf("\n");
	}
}

void events_destroy(SGA_StreamGraph* sg) {
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

SGA_StreamGraph SGA_StreamGraph_from_external_format_v_1_0_0(const String* external_format) {
	String internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(external_format);
	SGA_StreamGraph sg     = SGA_StreamGraph_from_string(&internal_format);
	String_destroy(internal_format);
	return sg;
}

void init_cache(SGA_Stream* stream) {
	stream->cache.temporal_cardinal_of_node_set.present = false;
	stream->cache.duration.present			    = false;
	stream->cache.temporal_cardinal_of_link_set.present = false;
	stream->cache.distinct_cardinal_of_node_set.present = false;
}

void reset_cache(SGA_Stream* stream) {
	stream->cache.temporal_cardinal_of_node_set.present = false;
	stream->cache.duration.present			    = false;
	stream->cache.temporal_cardinal_of_link_set.present = false;
	stream->cache.distinct_cardinal_of_node_set.present = false;
}

SGA_Interval SGA_StreamGraph_lifespan(SGA_StreamGraph* sg) {
	return sg->lifespan;
}

size_t SGA_StreamGraph_time_scale(SGA_StreamGraph* sg) {
	return sg->time_scale;
}

SGA_StreamGraph SGA_StreamGraph_from_string(const String* sg_as_str) {
	// Read the header
	char* str = sg_as_str->data;
	char format[20];
	int major, minor, patch;
	sscanf(str, "SGA %19s format version %d.%d.%d\n", format, &major, &minor, &patch);

	// Call the appropriate function
	if (strcmp(format, "Internal") == 0) {
		// No modifications of format for now
		return SGA_StreamGraph_from_internal_format_v_1_0_0(sg_as_str);
	}
	else if (strcmp(format, "External") == 0) {
		// No modifications of format for now
		return SGA_StreamGraph_from_external_format_v_1_0_0(sg_as_str);
	}
	else {
		fprintf(stderr, "Unknown format %s\n", format);
		exit(1);
	}
}
