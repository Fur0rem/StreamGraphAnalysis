#ifndef METRICS_LINK_H
#define METRICS_LINK_H

#include "../stream_graph.h"
#include <stddef.h>

double contribution_of_link(StreamGraph* sg, size_t link_id);

#endif // METRICS_LINK_H