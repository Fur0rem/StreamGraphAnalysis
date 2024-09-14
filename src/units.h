#ifndef UNITS_H
#define UNITS_H

#include "defaults.h"
#include <stddef.h>

typedef size_t NodeId;
DeclareArrayList(NodeId);
DeclareArrayListDeriveEquals(NodeId);
DeclareArrayListDeriveRemove(NodeId);
DeclareArrayListDeriveToString(NodeId);

typedef size_t LinkId;
DeclareArrayList(LinkId);
DeclareArrayListDeriveEquals(LinkId);
DeclareArrayListDeriveRemove(LinkId);
DeclareArrayListDeriveToString(LinkId);

typedef size_t TimeId;
DeclareArrayList(TimeId);
DeclareArrayListDeriveEquals(TimeId);
DeclareArrayListDeriveRemove(TimeId);
DeclareArrayListDeriveToString(TimeId);
DeclareArrayListDeriveOrdered(TimeId);

#endif // UNITS_H