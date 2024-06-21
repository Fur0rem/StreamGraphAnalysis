#include "links_set.h"
#include "../utils.h"

LinksSet LinksSet_alloc(size_t nb_links) {
	LinksSet set;
	set.nb_links = nb_links;
	set.links = MALLOC(nb_links * sizeof(Link));
	return set;
}

char* Link_to_string(Link* link) {
	char* str = MALLOC(64);
	snprintf(str, 64, "(%lu, %lu)", link->nodes[0], link->nodes[1]);
	return str;
}

bool Link_equals(Link a, Link b) {
	return a.nodes[0] == b.nodes[0] && a.nodes[1] == b.nodes[1];
}