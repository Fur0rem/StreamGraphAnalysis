#ifndef UNITS_H
#define UNITS_H

#include "defaults.h"
#include <stddef.h>

typedef size_t SGA_NodeId; ///< Used to index nodes in a StreamGraph.
DeclareArrayList(SGA_NodeId);
DeclareArrayListDeriveEquals(SGA_NodeId);
DeclareArrayListDeriveRemove(SGA_NodeId);
DeclareArrayListDeriveToString(SGA_NodeId);

typedef size_t SGA_LinkId; ///< Used to index links in a StreamGraph.
DeclareArrayList(SGA_LinkId);
DeclareArrayListDeriveEquals(SGA_LinkId);
DeclareArrayListDeriveRemove(SGA_LinkId);
DeclareArrayListDeriveToString(SGA_LinkId);

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

#endif // UNITS_H