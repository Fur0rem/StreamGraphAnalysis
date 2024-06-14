#include "link.h"
#include "../metrics.h"

double contribution_of_link(StreamGraph* sg, size_t link_id) {
	size_t size = IntervalsSet_size(sg->links.links[link_id].presence);
	return (double)size / (double)size_of_lifespan(sg);
}