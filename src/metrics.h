#ifndef METRICS_H
#define METRICS_H

#include "metrics/link.h"
#include "metrics/node.h"
#include "stream_graph.h"
#include <stddef.h>

size_t size_of_lifespan(StreamGraph* sg);
double coverage(StreamGraph* sg);
size_t number_of_nodes(StreamGraph* sg);
double number_of_temporal_nodes(StreamGraph* sg);
double number_of_links(StreamGraph* sg);
double time_units(StreamGraph* sg);
double node_duration(StreamGraph* sg);
double link_duration(StreamGraph* sg);
double uniformity(StreamGraph* sg);
double density(StreamGraph* sg);
double compactness(StreamGraph* sg);
double average_node_degree(StreamGraph* sg);

#endif // METRICS_H