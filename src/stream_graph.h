#ifndef SGA_STREAM_GRAPH_H
#define SGA_STREAM_GRAPH_H

#include "bit_array.h"
#include "interval.h"
#include "units.h"
#include <stddef.h>
#include <stdint.h>

#include "stream_graph/events_table.h"
#include "stream_graph/key_moments_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"

typedef const char** NodeNameTable;

typedef struct {
	KeyMomentsTable key_moments;
	TemporalNodesSet nodes;
	LinksSet links;
	EventsTable events;
	NodeNameTable node_names;
	size_t scaling;
} StreamGraph;

StreamGraph SGA_StreamGraph_from_string(const char* str);
StreamGraph SGA_StreamGraph_from_file(const char* filename);
char* SGA_StreamGraph_to_string(StreamGraph* sg);
void SGA_StreamGraph_destroy(StreamGraph* sg);
size_t SGA_StreamGraph_lifespan_begin(StreamGraph* sg);
size_t SGA_StreamGraph_lifespan_end(StreamGraph* sg);

#endif // SGA_STREAM_GRAPH_H