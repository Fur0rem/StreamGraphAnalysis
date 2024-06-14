#ifndef METRICS_GENERIC_H
#define METRICS_GENERIC_H

#include <stddef.h>

typedef struct {
	enum {
		FULL_STREAM_GRAPH,
		LINK_STREAM,
	} type;
	void* stream;
} stream_t;

typedef struct {
	// takes in a void* and returns a double
	size_t (*cardinalOfW)(void*);
	// takes in a void* and returns a double
	size_t (*cardinalOfT)(void*);
	// takes in a void* and returns a size_t
	size_t (*cardinalOfV)(void*);
	// takes in a void* and returns a size_t
	size_t (*scaling)(void*);
} BaseGenericFunctions;

typedef struct {
	// takes in a void* and returns a double
	double (*coverage)(void*);
	// takes in a void* and returns a double
	double (*coverage_stream)(void*);
	// takes in a void* and returns a double
	double (*node_duration)(void*);
} HijackedGenericFunctions;

double coverage_stream(stream_t* stream);
double node_duration_stream(stream_t* stream);

#endif // METRICS_GENERIC_H