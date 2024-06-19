#ifndef STREAM_H
#define STREAM_H

#include <stddef.h>

typedef struct {
	bool present;
	size_t data;
} OptionalSizeT;

typedef struct {
	OptionalSizeT cardinalOfW;
	OptionalSizeT cardinalOfT;
	OptionalSizeT cardinalOfV;
} InformationCache;

typedef struct {
	enum {
		FULL_STREAM_GRAPH,
		LINK_STREAM,
	} type;
	void* stream;
	InformationCache cache;
} Stream;

#endif // STREAM_H