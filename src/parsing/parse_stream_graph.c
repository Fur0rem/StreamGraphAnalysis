#define SGA_INTERNAL

#include "parse_stream_graph.h"
#include "cursor.h"
#include "events.h"
#include "general.h"
#include "parse_weights.h"
#include "version.h"

#define CHECK_FAIL(err_msg) SGA_ParsingResult_crash_if_fail(&cursor, &result, (err_msg), NULL)

SGA_ParsedStreamGraph SGA_parse_stream_graph(const char* filename) {
	String str		 = String_from_file(filename);
	SGA_ParsingCursor cursor = SGA_ParsingCursor_begin(str.data, filename);

	SGA_Version version = SGA_parse_version(&cursor);
	// No difference in parsing for different versions yet so we don't use it

	SGA_ParsingResult result = SGA_ParsingCursor_move_to_next_line(&cursor, SGA_CODE_HERE);
	CHECK_FAIL("Failed to move to next line after version!\n");

	SGA_ParsedStreamGraph stream_graph = {
	    .node_presences	 = SGA_IntervalsSetBuilderArrayList_new(),
	    .link_presences	 = SGA_IntervalsSetBuilderArrayList_new(),
	    .link_id_map	 = LinkIdMapHashset_new(),
	    .neighbours_of_nodes = SGA_LinkIdArrayListArrayList_new(),
	};

	result = SGA_ParsingCursor_set_current_header(&cursor, 0, "[general]", SGA_CODE_HERE);
	CHECK_FAIL("Failed to find [general] header!\n");

	SGA_ParsingCursor_move_to_next_line(&cursor, SGA_CODE_HERE);
	stream_graph.general_header = SGA_parse_general_header(&cursor);

	SGA_ParsingCursor_set_current_header(&cursor, 0, "[events]", SGA_CODE_HERE);
	SGA_ParsingCursor_move_to_next_line(&cursor, SGA_CODE_HERE);
	stream_graph.events = SGA_parse_events(&cursor,
					       &stream_graph.node_presences,
					       &stream_graph.link_presences,
					       &stream_graph.link_id_map,
					       &stream_graph.neighbours_of_nodes);

	if (SGA_ParsingCursor_contains(&cursor, "[weights]")) {
		result = SGA_ParsingCursor_set_current_header(&cursor, 2, "[[nodes]]", SGA_CODE_HERE);
		CHECK_FAIL("Failed to find [[nodes]] sub-header when parsing weights!\n");
		result = SGA_ParsingCursor_move_to_next_line(&cursor, SGA_CODE_HERE);
		CHECK_FAIL("Reached end of file prematurely!\n");
		stream_graph.node_weights = SGA_parse_weight_function(&cursor, true);

		result = SGA_ParsingCursor_set_current_header(&cursor, 2, "[[links]]", SGA_CODE_HERE);
		CHECK_FAIL("Failed to find [[links]] sub-header when parsing weights!\n");
		result = SGA_ParsingCursor_move_to_next_line(&cursor, SGA_CODE_HERE);
		CHECK_FAIL("Reached end of file prematurely!\n");
		stream_graph.link_weights = SGA_parse_weight_function(&cursor, false);
	}

	result = SGA_ParsingCursor_set_current_header(&cursor, 1, "[end]", SGA_CODE_HERE);
	CHECK_FAIL("Stream graph was not closed! Please add [end] to signify the end of it.");

	String_destroy(str);
	return stream_graph;
}

void SGA_ParsedStreamGraph_destroy(SGA_ParsedStreamGraph psg) {
	SGA_ParsedEventArrayList_destroy(psg.events);
	SGA_IntervalsSetBuilderArrayList_destroy(psg.node_presences);
	SGA_IntervalsSetBuilderArrayList_destroy(psg.link_presences);
	LinkIdMapHashset_destroy(psg.link_id_map);
	SGA_LinkIdArrayListArrayList_destroy(psg.neighbours_of_nodes);
	if (psg.general_header.is_weighted) {
		ParsedWeightFunction_destroy(psg.node_weights);
		ParsedWeightFunction_destroy(psg.link_weights);
	}
}