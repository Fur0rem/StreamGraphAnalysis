#ifndef SGA_PARSING_PARSE_STREAM_GRAPH_H
#define SGA_PARSING_PARSE_STREAM_GRAPH_H

#include "cursor.h"
#include "events.h"
#include "general.h"
#include "parse_weights.h"
#include "version.h"

#ifdef SGA_INTERNAL

typedef struct SGA_ParsedStreamGraph {
	SGA_GeneralHeader general_header;
	SGA_ParsedEventArrayList events;
	SGA_IntervalsSetBuilderArrayList node_presences;
	SGA_IntervalsSetBuilderArrayList link_presences;
	LinkIdMapHashset link_id_map;
	SGA_LinkIdArrayListArrayList neighbours_of_nodes;
	bool is_weighted;
	ParsedWeightFunction node_weights;
	ParsedWeightFunction link_weights;
} SGA_ParsedStreamGraph;

SGA_ParsedStreamGraph SGA_parse_stream_graph(const char* filename);

void SGA_ParsedStreamGraph_destroy(SGA_ParsedStreamGraph psg);

#endif // SGA_INTERNAL

#endif // SGA_PARSING_PARSE_STREAM_GRAPH_H