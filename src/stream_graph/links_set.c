#include "links_set.h"
#include "../utils.h"

LinksSet LinksSet_alloc(size_t nb_links) {
	LinksSet set = {
		.nb_links = nb_links,
		.links	  = MALLOC(nb_links * sizeof(Link)),
	};
	return set;
}

String Link_to_string(const Link* link) {
	String str = String_with_capacity(100);
	String_append_formatted(&str, "(%lu - %lu), Intervals : [", link->nodes[0], link->nodes[1]);

	for (size_t i = 0; i < link->presence.nb_intervals; i++) {
		String interval_str = Interval_to_string(&link->presence.intervals[i]);
		String_concat_consume(&str, interval_str);
		String_push_str(&str, " U ");
	}

	String_pop_n(&str, 3);
	String_push_str(&str, "]\n");
	return str;
}

bool Link_equals(const Link* a, const Link* b) {
	return a->nodes[0] == b->nodes[0] && a->nodes[1] == b->nodes[1];
}

DefineArrayList(Link);
DefineArrayListDeriveEquals(Link);
DefineArrayListDeriveToString(Link);
NO_FREE(Link);
DefineArrayListDeriveRemove(Link);

NodeId Link_get_other_node(const Link* link, NodeId node_id) {
	if (link->nodes[0] == node_id) {
		return link->nodes[1];
	}
	else {
		return link->nodes[0];
	}
}