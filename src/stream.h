#ifndef STREAM_H
#define STREAM_H

#include "bit_array.h"
#include "interval.h"
#include "stream_graph/events_table.h"
#include "stream_graph/key_moments_table.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	KeyMomentsTable key_moments;
	TemporalNodesSet nodes;
	LinksSet links;
	EventsTable events;
	Interval lifespan;
	size_t scaling;
} StreamGraph;

StreamGraph StreamGraph_from_string(const char* str);
StreamGraph StreamGraph_from_file(const char* filename);
String StreamGraph_to_string(StreamGraph* sg);
void StreamGraph_destroy(StreamGraph sg);
// size_t StreamGraph_lifespan_begin(StreamGraph* sg);
// size_t StreamGraph_lifespan_end(StreamGraph* sg);
Interval StreamGraph_lifespan(StreamGraph* sg);
void init_events_table(StreamGraph* sg);
void events_destroy(StreamGraph* sg);
char* InternalFormat_from_External_str(const char* str);
StreamGraph StreamGraph_from_external(const char* filename);

typedef struct {
	bool present;
	size_t data;
} OptionalSizeT;

typedef struct {
	OptionalSizeT cardinalOfW;
	OptionalSizeT cardinalOfT;
	OptionalSizeT cardinalOfE;
	OptionalSizeT cardinalOfV;
} InformationCache;

// TRICK: StreamData is a blanket struct that serves as a placeholder for the union of all stream types
// (FullStreamGraph, LinkStream, ect...).
// Otherwise a circular dependency would be created since these files need stream.h.
// And if we used a void* pointer, we would lose type safety.
typedef struct {
} StreamData;

typedef struct {
	enum {
		FULL_STREAM_GRAPH,
		LINK_STREAM,
		CHUNK_STREAM,
		CHUNK_STREAM_SMALL,
		SNAPSHOT_STREAM,
	} type;
	StreamData* stream_data;
	InformationCache cache;
} Stream;

void init_cache(Stream* stream);

#define FETCH_CACHE(stream, field)                                                                                     \
	if ((stream)->cache.field.present) {                                                                               \
		return (stream)->cache.field.data;                                                                             \
	}

#define UPDATE_CACHE(stream, field, value)                                                                             \
	(stream)->cache.field.present = true;                                                                              \
	(stream)->cache.field.data	  = value;

void reset_cache(Stream* stream);

#endif // STREAM_H