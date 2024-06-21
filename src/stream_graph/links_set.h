#ifndef STREAM_GRAPH_LINKS_SET_H
#define STREAM_GRAPH_LINKS_SET_H

#include "../interval.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	IntervalsSet presence;
	NodeId nodes[2];
} Link;

typedef struct {
	size_t nb_links;
	Link* links;
} LinksSet;

LinksSet LinksSet_alloc(size_t nb_links);
char* Link_to_string(Link* link);
bool Link_equals(Link a, Link b);

#endif // STREAM_GRAPH_LINKS_SET_H