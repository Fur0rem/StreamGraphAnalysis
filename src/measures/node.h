#ifndef SGA_MEASURES_NODE_H
#define SGA_MEASURES_NODE_H

#include "../stream_graph.h"
#include <stddef.h>

double SGA_degree_of_node(StreamGraph* sg, size_t node_id);
double SGA_contribution_of_node(StreamGraph* sg, size_t node_id);

#endif // SGA_MEASURES_NODE_H