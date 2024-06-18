#ifndef STREAM_GRAPH_H
#define STREAM_GRAPH_H

#include "bit_array.h"
#include "interval.h"
#include "units.h"
#include <stddef.h>
#include <stdint.h>

#include "stream_graph/events_table.h"
#include "stream_graph/key_moments_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"

typedef struct {
	KeyMomentsTable key_moments;
	TemporalNodesSet nodes;
	LinksSet links;
	EventsTable events;
	size_t scaling;
} StreamGraph;

typedef struct {
	enum {
		FULL_STREAM_GRAPH,
		LINK_STREAM,
	} type;
	void* stream;
} stream_t;

StreamGraph StreamGraph_from_string(const char* str);
StreamGraph StreamGraph_from_file(const char* filename);
char* StreamGraph_to_string(StreamGraph* sg);
void StreamGraph_destroy(StreamGraph* sg);
size_t StreamGraph_lifespan_begin(StreamGraph* sg);
size_t StreamGraph_lifespan_end(StreamGraph* sg);
void init_events_table(StreamGraph* sg);
void events_destroy(StreamGraph* sg);

typedef struct {
	stream_t stream_graph;
	void* iterator_data;
	size_t (*next)(void*);
	void (*destroy)(void*);
} NodesIterator;

typedef struct {
	stream_t stream_graph;
	void* iterator_data;
	size_t (*next)(void*);
	void (*destroy)(void*);
} LinksIterator;

typedef struct {
	stream_t stream_graph;
	void* iterator_data;
	Interval (*next)(void*);
	void (*destroy)(void*);
} TimesIterator;

#endif // STREAM_GRAPH_H