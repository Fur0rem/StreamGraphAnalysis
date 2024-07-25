#ifndef STREAM_H
#define STREAM_H

#include <stdbool.h>
#include <stddef.h>

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
	} type;
	StreamData* stream_data;
	InformationCache cache;
} Stream;

void init_cache(Stream* stream);

#define FETCH_CACHE(stream, field)                                                                                     \
	if ((stream)->cache.field.present) {                                                                               \
		printf("Cache hit for %s\n", #field);                                                                          \
		return (stream)->cache.field.data;                                                                             \
	}                                                                                                                  \
	printf("Cache miss for %s\n", #field);

#define UPDATE_CACHE(stream, field, value)                                                                             \
	(stream)->cache.field.present = true;                                                                              \
	(stream)->cache.field.data = value;

void reset_cache(Stream* stream);

#endif // STREAM_H