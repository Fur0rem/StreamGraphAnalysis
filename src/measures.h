#ifndef SGA_MEASURES_H
#define SGA_MEASURES_H

#include "measures/link.h"
#include "measures/node.h"
#include "stream_graph.h"
#include <stddef.h>

size_t SGA_size_of_lifespan(StreamGraph* sg);
double SGA_coverage(StreamGraph* sg);
size_t SGA_number_of_nodes(StreamGraph* sg);
double SGA_number_of_temporal_nodes(StreamGraph* sg);
double SGA_number_of_links(StreamGraph* sg);
double SGA_time_units(StreamGraph* sg);
double SGA_node_duration(StreamGraph* sg);
double SGA_link_duration(StreamGraph* sg);
double SGA_uniformity(StreamGraph* sg);
double SGA_density(StreamGraph* sg);
double SGA_compactness(StreamGraph* sg);
double SGA_average_node_degree(StreamGraph* sg);

#endif // SGA_MEASURES_H