#include "units.h"
#include "generic_data_structures/arraylist.h"
#include "utils.h"

DEFAULT_EQUALS(SGA_NodeId);
NO_FREE(SGA_NodeId);
DEFAULT_COMPARE(SGA_NodeId);
DEFAULT_TO_STRING(SGA_NodeId, "%zu");
DefineArrayList(SGA_NodeId);
DefineArrayListDeriveEquals(SGA_NodeId);
DefineArrayListDeriveRemove(SGA_NodeId);
DefineArrayListDeriveToString(SGA_NodeId);
DefineArrayListDeriveOrdered(SGA_NodeId);

DEFAULT_EQUALS(SGA_LinkId);
NO_FREE(SGA_LinkId);
DEFAULT_COMPARE(SGA_LinkId);
DEFAULT_TO_STRING(SGA_LinkId, "%zu");
DefineArrayList(SGA_LinkId);
DefineArrayListDeriveEquals(SGA_LinkId);
DefineArrayListDeriveRemove(SGA_LinkId);
DefineArrayListDeriveToString(SGA_LinkId);
DefineArrayListDeriveOrdered(SGA_LinkId);

NO_FREE(SGA_TimeId);
DEFAULT_EQUALS(SGA_TimeId);
DEFAULT_COMPARE(SGA_TimeId);
DEFAULT_TO_STRING(SGA_TimeId, "%zu");
DefineArrayList(SGA_TimeId);
DefineArrayListDeriveEquals(SGA_TimeId);
DefineArrayListDeriveRemove(SGA_TimeId);
DefineArrayListDeriveOrdered(SGA_TimeId);
DefineArrayListDeriveToString(SGA_TimeId);

NO_FREE(SGA_Time);
DEFAULT_EQUALS(SGA_Time);
DEFAULT_COMPARE(SGA_Time);
DEFAULT_TO_STRING(SGA_Time, "%zu");
DefineArrayList(SGA_Time);
DefineArrayListDeriveEquals(SGA_Time);
DefineArrayListDeriveRemove(SGA_Time);
DefineArrayListDeriveOrdered(SGA_Time);
DefineArrayListDeriveToString(SGA_Time);