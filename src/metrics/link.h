#ifndef SGA_METRICS_LINK_H
#define SGA_METRICS_LINK_H

#include "../stream_graph.h"
#include <stddef.h>

double SGA_contribution_of_link(StreamGraph* sg, size_t link_id);

#endif // SGA_METRICS_LINK_H