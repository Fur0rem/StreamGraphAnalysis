#define SGA_INTERNAL

#include "links_set.h"
#include "../utils.h"

LinksSet LinksSet_alloc(size_t nb_links) {
	LinksSet set = {
	    .nb_links = nb_links,
	    .links    = MALLOC(nb_links * sizeof(SGA_Link)),
	};
	return set;
}

String SGA_Link_to_string(const SGA_Link* link) {
	String str = String_with_capacity(100);
	String_append_formatted(&str, "(%lu - %lu), Presence : ", link->nodes[0], link->nodes[1]);
	String_concat_consume(&str, SGA_IntervalsSet_to_string(&link->presence));
	return str;
}

bool SGA_Link_equals(const SGA_Link* a, const SGA_Link* b) {
	return a->nodes[0] == b->nodes[0] && a->nodes[1] == b->nodes[1];
}

DefineArrayList(SGA_Link);
DefineArrayListDeriveEquals(SGA_Link);
DefineArrayListDeriveToString(SGA_Link);
NO_FREE(SGA_Link);
DefineArrayListDeriveRemove(SGA_Link);

SGA_NodeId SGA_Link_get_other_node(const SGA_Link* link, SGA_NodeId node_id) {
	if (link->nodes[0] == node_id) {
		return link->nodes[1];
	}
	else {
		return link->nodes[0];
	}
}