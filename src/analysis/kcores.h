#ifndef K_CORES_H
#define K_CORES_H

#include "../generic_data_structures/vector.h"
#include "../interval.h"
#include "../stream.h"
#include "../utils.h"
#include "stddef.h"

typedef struct {
	NodeId node_id;
	IntervalVector presence;
} NodePresence;

DeclareVector(NodePresence);

typedef struct {
	NodePresenceVector nodes;
} KCore;

DeclareToString(KCore);
DeclareEquals(KCore);
DeclareDestroy(KCore);

KCore Stream_k_cores(const Stream* stream, size_t k);
void KCore_clean_up(KCore* k_core);

#endif // K_CORES_H