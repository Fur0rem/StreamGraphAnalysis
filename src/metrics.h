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
	double (*density)(void*);
} MetricsFunctions;

double Stream_coverage(Stream stream);
double Stream_node_duration(Stream stream);
size_t cardinalOfW(Stream stream);
size_t cardinalOfT(Stream stream);

double Stream_contribution_of_node(Stream stream, NodeId node_id);
double Stream_contribution_of_link(Stream stream, LinkId link_id);
double Stream_number_of_nodes(Stream stream);
double Stream_number_of_links(Stream stream);
double Stream_node_contribution_at_time(Stream stream, TimeId time_id);
// size_t size_set_unordered_pairs_itself(size_t n);
double Stream_link_contribution_at_time(Stream stream, TimeId time_id);
double Stream_link_duration(Stream stream);
double Stream_uniformity(Stream stream);
double Stream_density(Stream stream);
double Stream_density_of_node(Stream stream, NodeId node_id); // TODO
double Stream_density_of_link(Stream stream, LinkId link_id);
double Stream_density_of_time(Stream stream, TimeId time_id); // TODO

#endif // METRICS_H