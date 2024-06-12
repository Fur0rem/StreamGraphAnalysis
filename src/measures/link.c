#include "link.h"
#include "../measures.h"

double SGA_contribution_of_link(StreamGraph* sg, size_t link_id) {
	size_t size = IntervalsSet_size(sg->links.links[link_id].presence);
	return (double)size / (double)SGA_size_of_lifespan(sg);
}