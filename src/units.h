#ifndef UNITS_H
#define UNITS_H

#include "defaults.h"
#include <stddef.h>

typedef size_t NodeId;
DeclareVector(NodeId);
DeclareVectorDeriveEquals(NodeId);
DeclareVectorDeriveRemove(NodeId);
DeclareVectorDeriveToString(NodeId);

typedef size_t LinkId;
DeclareVector(LinkId);
DeclareVectorDeriveEquals(LinkId);
DeclareVectorDeriveRemove(LinkId);
DeclareVectorDeriveToString(LinkId);

typedef size_t TimeId;
DeclareVector(TimeId);
DeclareVectorDeriveEquals(TimeId);
DeclareVectorDeriveRemove(TimeId);
DeclareVectorDeriveToString(TimeId);

#endif // UNITS_H