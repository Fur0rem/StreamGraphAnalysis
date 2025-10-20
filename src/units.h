#ifndef UNITS_H
#define UNITS_H

#include "defaults.h"
#include "generic_data_structures/arraylist.h"
#include <stddef.h>

typedef size_t SGA_NodeId; ///< Used to index nodes in a StreamGraph.
DeclareArrayList(SGA_NodeId);
DeclareArrayListDeriveEquals(SGA_NodeId);
DeclareArrayListDeriveRemove(SGA_NodeId);
DeclareArrayListDeriveToString(SGA_NodeId);
DeclareArrayListDeriveOrdered(SGA_NodeId);
size_t SGA_NodeId_max();

typedef size_t SGA_LinkId; ///< Used to index links in a StreamGraph.
DeclareArrayList(SGA_LinkId);
DeclareArrayListDeriveEquals(SGA_LinkId);
DeclareArrayListDeriveRemove(SGA_LinkId);
DeclareArrayListDeriveToString(SGA_LinkId);
DeclareArrayListDeriveOrdered(SGA_LinkId);
DeclareArrayList(SGA_LinkIdArrayList);
DeclareArrayListDeriveEquals(SGA_LinkIdArrayList);
DeclareArrayListDeriveRemove(SGA_LinkIdArrayList);
DeclareArrayListDeriveToString(SGA_LinkIdArrayList);
size_t SGA_LinkId_max();

typedef size_t SGA_TimeId; ///< Used to index times in a StreamGraph.
DeclareArrayList(SGA_TimeId);
DeclareArrayListDeriveEquals(SGA_TimeId);
DeclareArrayListDeriveRemove(SGA_TimeId);
DeclareArrayListDeriveToString(SGA_TimeId);
DeclareArrayListDeriveOrdered(SGA_TimeId);

typedef size_t SGA_Time; ///< Used to represent a time instant
DeclareArrayList(SGA_Time);
DeclareArrayListDeriveEquals(SGA_Time);
DeclareArrayListDeriveRemove(SGA_Time);
DeclareArrayListDeriveToString(SGA_Time);
DeclareArrayListDeriveOrdered(SGA_Time);

size_t SGA_Time_max();

typedef double SGA_Weight; ///< Used to represent a weight in a weighted stream graph.
DeclareArrayList(SGA_Weight);
DeclareArrayListDeriveEquals(SGA_Weight);
DeclareArrayListDeriveRemove(SGA_Weight);
DeclareArrayListDeriveToString(SGA_Weight);
DeclareArrayListDeriveOrdered(SGA_Weight);

#include "generic_data_structures/arraylist.h"
#include "generic_data_structures/hashset.h"

/**
 * @brief Either a node ID or a Link composed of both of its nodes
 */
typedef union SGA_NodeOrLink {
	struct {
		SGA_NodeId nodes[2];
	} link;
	SGA_NodeId node;
} SGA_NodeOrLink;

#endif // UNITS_H