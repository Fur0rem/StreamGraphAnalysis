#include "units.h"
#include "generic_data_structures/arraylist.h"
#include "utils.h"

DEFAULT_EQUALS(NodeId);
NO_FREE(NodeId);
DefineArrayList(NodeId);
DefineArrayListDeriveEquals(NodeId);
DefineArrayListDeriveRemove(NodeId);
DEFAULT_TO_STRING(NodeId, "%zu");
DefineArrayListDeriveToString(NodeId);

DEFAULT_EQUALS(LinkId);

NO_FREE(LinkId);
DefineArrayList(LinkId);
DefineArrayListDeriveEquals(LinkId);
DefineArrayListDeriveRemove(LinkId);
DEFAULT_TO_STRING(LinkId, "%zu");
DefineArrayListDeriveToString(LinkId);

// TODO: rename to instant
NO_FREE(TimeId);
DEFAULT_EQUALS(TimeId);
DEFAULT_COMPARE(TimeId);
DefineArrayList(TimeId);
DefineArrayListDeriveEquals(TimeId);
DefineArrayListDeriveRemove(TimeId);
DefineArrayListDeriveOrdered(TimeId);
DEFAULT_TO_STRING(TimeId, "%zu");
DefineArrayListDeriveToString(TimeId);