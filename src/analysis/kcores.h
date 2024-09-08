#ifndef K_CORES_H
#define K_CORES_H

#include "../interval.h"
#include "../stream.h"
#include "../utils.h"
#include "../vector.h"
#include "stddef.h"

typedef struct {
	size_t degree;
	Interval time;
} DegreeInInterval;
DeclareVector(DegreeInInterval);
DeclareVectorDeriveRemove(DegreeInInterval);
DeclareVectorDeriveEquals(DegreeInInterval);
DeclareVectorDeriveToString(DegreeInInterval);

DegreeInIntervalVector Stream_evolution_of_node_degree(const Stream* stream, NodeId node_id);

typedef struct {
	NodeId node_id;
	IntervalVector presence;
} NodePresence;

DeclareVector(NodePresence);

typedef struct {
	NodePresenceVector nodes;
} KCore;

String KCore_to_string(const KCore* k_core);
bool KCore_equals(const KCore* a, const KCore* b);
void KCore_destroy(KCore k_core);

KCore Stream_k_cores(const Stream* stream, size_t k);
void KCore_clean_up(KCore* k_core);

#endif // K_CORES_H