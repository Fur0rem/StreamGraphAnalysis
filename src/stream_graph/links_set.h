#ifndef SGA_STREAM_GRAPH_LINKS_SET_H
#define SGA_STREAM_GRAPH_LINKS_SET_H

#include "../interval.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	size_t nb_intervals;
	Interval* present_at;
	NodeId nodes[2];
} Link;

typedef struct {
	size_t nb_links;
	Link* links;
} LinksSet;

#endif // SGA_STREAM_GRAPH_LINKS_SET_H