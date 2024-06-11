#include "links_set.h"
#include "../utils.h"

LinksSet SGA_LinksSet_alloc(size_t nb_links) {
	LinksSet set;
	set.nb_links = nb_links;
	set.links = MALLOC(nb_links * sizeof(Link));
	return set;
}