#include "links_set.h"
#include "../utils.h"

LinksSet LinksSet_alloc(size_t nb_links) {
	LinksSet set;
	set.nb_links = nb_links;
	set.links = MALLOC(nb_links * sizeof(Link));
	return set;
}

char* Link_to_string(Link* link) {
	char* str = MALLOC(9999);
	snprintf(str, 9999, "(%lu - %lu), Intervals : [", link->nodes[0], link->nodes[1]);
	for (size_t i = 0; i < link->presence.nb_intervals; i++) {
		snprintf(str + strlen(str), 9999 - strlen(str), "[%lu, %lu[ U ", link->presence.intervals[i].start);
	}
	return str;
}

bool Link_equals(Link a, Link b) {
	return a.nodes[0] == b.nodes[0] && a.nodes[1] == b.nodes[1];
}