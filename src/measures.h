#ifndef SGA_MEASURES_H
#define SGA_MEASURES_H

#include "stream_graph.h"

size_t SGA_size_of_lifespan(StreamGraph* sg);
double SGA_coverage(StreamGraph* sg);
double SGA_number_of_nodes(StreamGraph* sg);

#endif // SGA_MEASURES_H