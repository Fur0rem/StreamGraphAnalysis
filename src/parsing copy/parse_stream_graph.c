#ifndef SGA_PARSING_PARSE_STREAM_GRAPH_C
#define SGA_PARSING_PARSE_STREAM_GRAPH_C

#include "cursor.h"
#include "events.c"
#include "general.c"
#include "version.c"

typedef struct SGA_ParsedStreamGraph {
	SGA_GeneralHeader general_header;
	SGA_ParsedEventArrayList events;
	SGA_IntervalsSetBuilderArrayList node_presences;
	SGA_IntervalsSetBuilderArrayList link_presences;
	LinkIdMapHashset link_id_map;
	SGA_LinkIdArrayListArrayList neighbours_of_nodes;
} SGA_ParsedStreamGraph;

SGA_ParsedStreamGraph SGA_parse_stream_graph(const char* str) {
	SGA_ParsingCursor cursor = SGA_ParsingCursor_begin(str);

	SGA_ParsedStreamGraph stream_graph = {
	    .events		 = SGA_ParsedEventArrayList_new(),
	    .node_presences	 = SGA_IntervalsSetBuilderArrayList_new(),
	    .link_presences	 = SGA_IntervalsSetBuilderArrayList_new(),
	    .link_id_map	 = LinkIdMapHashset_new(),
	    .neighbours_of_nodes = SGA_LinkIdArrayListArrayList_new(),
	};

	SGA_ParsingCursor_move_to_next(&cursor, "[general]");
	SGA_ParsingCursor_move_to_next_line(&cursor);
	stream_graph.general_header = SGA_parse_general_header(&cursor);

	SGA_ParsingCursor_move_to_next(&cursor, "[events]");
	SGA_ParsingCursor_move_to_next_line(&cursor);
	stream_graph.events = SGA_parse_events(&cursor,
					       &stream_graph.node_presences,
					       &stream_graph.link_presences,
					       &stream_graph.link_id_map,
					       &stream_graph.neighbours_of_nodes);

	return stream_graph;
}

#endif // SGA_PARSING_PARSE_STREAM_GRAPH_C