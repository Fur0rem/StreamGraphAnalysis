#include "units.h"
#include "generic_data_structures/arraylist.h"
#include "utils.h"

DEFAULT_EQUALS(SGA_NodeId);
NO_FREE(SGA_NodeId);
DefineArrayList(SGA_NodeId);
DefineArrayListDeriveEquals(SGA_NodeId);
DefineArrayListDeriveRemove(SGA_NodeId);
DEFAULT_TO_STRING(SGA_NodeId, "%zu");
DefineArrayListDeriveToString(SGA_NodeId);

DEFAULT_EQUALS(SGA_LinkId);
NO_FREE(SGA_LinkId);
DefineArrayList(SGA_LinkId);
DefineArrayListDeriveEquals(SGA_LinkId);
DefineArrayListDeriveRemove(SGA_LinkId);
DEFAULT_TO_STRING(SGA_LinkId, "%zu");
DefineArrayListDeriveToString(SGA_LinkId);

NO_FREE(SGA_TimeId);
DEFAULT_EQUALS(SGA_TimeId);
DEFAULT_COMPARE(SGA_TimeId);
DefineArrayList(SGA_TimeId);
DefineArrayListDeriveEquals(SGA_TimeId);
DefineArrayListDeriveRemove(SGA_TimeId);
DefineArrayListDeriveOrdered(SGA_TimeId);
DEFAULT_TO_STRING(SGA_TimeId, "%zu");
DefineArrayListDeriveToString(SGA_TimeId);

NO_FREE(SGA_Time);
DEFAULT_EQUALS(SGA_Time);
DEFAULT_COMPARE(SGA_Time);
DefineArrayList(SGA_Time);
DefineArrayListDeriveEquals(SGA_Time);
DefineArrayListDeriveRemove(SGA_Time);
DefineArrayListDeriveOrdered(SGA_Time);
DEFAULT_TO_STRING(SGA_Time, "%zu");
DefineArrayListDeriveToString(SGA_Time);