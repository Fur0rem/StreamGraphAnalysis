#include "units.h"
#include "generic_data_structures/vector.h"
#include "utils.h"

DEFAULT_EQUALS(NodeId);

DefineVector(NodeId);
DefineVectorDeriveEquals(NodeId);
DefineVectorDeriveRemove(NodeId, NO_FREE(NodeId));
DEFAULT_TO_STRING(NodeId, "%zu");
DefineVectorDeriveToString(NodeId);

DEFAULT_EQUALS(LinkId);

DefineVector(LinkId);
DefineVectorDeriveEquals(LinkId);
DefineVectorDeriveRemove(LinkId, NO_FREE(LinkId));
DEFAULT_TO_STRING(LinkId, "%zu");
DefineVectorDeriveToString(LinkId);

DEFAULT_EQUALS(TimeId);
DEFAULT_COMPARE(TimeId);
DefineVector(TimeId);
DefineVectorDeriveEquals(TimeId);
DefineVectorDeriveRemove(TimeId, NO_FREE(TimeId));
DefineVectorDeriveOrdered(TimeId);
DEFAULT_TO_STRING(TimeId, "%zu");
DefineVectorDeriveToString(TimeId);