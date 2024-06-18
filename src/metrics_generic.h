#ifndef METRICS_GENERIC_H
#define METRICS_GENERIC_H

#include "interval.h"
#include "stream_graph.h"
#include <stddef.h>

typedef struct {
	/*// takes in a void* and returns a double
	size_t (*cardinalOfW)(void*);
	// takes in a void* and returns a double
	size_t (*cardinalOfT)(void*);
	// takes in a void* and returns a size_t
	size_t (*cardinalOfV)(void*);
	// takes in a void* and returns a size_t
	size_t (*scaling)(void*);*/
	NodesIterator (*nodes_set)(void*);
	LinksIterator (*links_set)(void*);
	Interval (*lifespan)(void*);
	size_t (*scaling)(void*);

	NodesIterator (*nodes_present_at_t)(void*, TimeId);
	LinksIterator (*links_present_at_t)(void*, TimeId);

	TimesIterator (*times_node_present)(void*, NodeId);
	TimesIterator (*times_link_present)(void*, LinkId);
} BaseGenericFunctions;

typedef struct {
	/*// takes in a void* and returns a double
	double (*coverage)(void*);
	// takes in a void* and returns a double
	double (*coverage_stream)(void*);
	// takes in a void* and returns a double
	double (*node_duration)(void*);*/

	size_t (*cardinalOfW)(void*);
	size_t (*cardinalOfT)(void*);
	size_t (*cardinalOfV)(void*);

	double (*coverage)(void*);
	double (*node_duration)(void*);
} HijackedGenericFunctions;

double coverage_stream(stream_t stream);
double node_duration_stream(stream_t stream);
size_t cardinalOfW(NodesIterator nodes);
size_t cardinalOfT(TimesIterator times);

#endif // METRICS_GENERIC_H