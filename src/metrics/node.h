#ifndef METRICS_NODE_H
#define METRICS_NODE_H

#include "../stream_graph.h"
#include <stddef.h>

double degree_of_node(StreamGraph* sg, size_t node_id);
double contribution_of_node(StreamGraph* sg, size_t node_id);

#endif // METRICS_NODE_H