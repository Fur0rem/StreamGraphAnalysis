#ifndef METRICS_H
#define METRICS_H

#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include <stddef.h>

typedef struct {
	size_t (*cardinalOfW)(void*);
	size_t (*cardinalOfT)(void*);
	size_t (*cardinalOfV)(void*);
	double (*coverage)(void*);
	double (*node_duration)(void*);
} MetricsFunctions;

double coverage_stream(Stream stream);
double node_duration_stream(Stream stream);
size_t cardinalOfW(NodesIterator nodes);
size_t cardinalOfT(Stream stream);

#endif // METRICS_H