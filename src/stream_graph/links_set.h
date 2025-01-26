#ifndef STREAM_GRAPH_LINKS_SET_H
#define STREAM_GRAPH_LINKS_SET_H

#include "../interval.h"
#include "../units.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	SGA_IntervalsSet presence;
	SGA_NodeId nodes[2];
} SGA_Link;

typedef struct {
	SGA_LinkId nb_links;
	SGA_Link* links;
} LinksSet;

#ifdef SGA_INTERNAL
LinksSet LinksSet_alloc(size_t nb_links);
#endif // SGA_INTERNAL

DeclareToString(SGA_Link);
DeclareEquals(SGA_Link);

DeclareArrayList(SGA_Link);
DeclareArrayListDeriveEquals(SGA_Link);
DeclareArrayListDeriveToString(SGA_Link);
DeclareArrayListDeriveRemove(SGA_Link);

SGA_NodeId SGA_Link_get_other_node(const SGA_Link* link, SGA_NodeId node_id);

#endif // STREAM_GRAPH_LINKS_SET_H