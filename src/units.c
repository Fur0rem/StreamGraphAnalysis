#include "units.h"
#include "utils.h"

bool NodeId_equals(NodeId* a, NodeId* b) {
	return *a == *b;
}

DefineVector(NodeId);
DefineVectorDeriveEquals(NodeId);
DefineVectorDeriveRemove(NodeId, NO_FREE(NodeId));
DEFAULT_TO_STRING(NodeId, "%zu");
DefineVectorDeriveToString(NodeId);

bool LinkId_equals(LinkId* a, LinkId* b) {
	return *a == *b;
}

DefineVector(LinkId);
DefineVectorDeriveEquals(LinkId);
DefineVectorDeriveRemove(LinkId, NO_FREE(LinkId));
DEFAULT_TO_STRING(LinkId, "%zu");
DefineVectorDeriveToString(LinkId);

bool TimeId_equals(TimeId* a, TimeId* b) {
	return *a == *b;
}

DefineVector(TimeId);
DefineVectorDeriveEquals(TimeId);
DefineVectorDeriveRemove(TimeId, NO_FREE(TimeId));
DEFAULT_TO_STRING(TimeId, "%zu");
DefineVectorDeriveToString(TimeId);