#include "measures.h"
#include "interval.h"
#include "stream_graph.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>

size_t SGA_size_of_lifespan(StreamGraph* sg) {
	return Interval_size(Interval_from(SGA_StreamGraph_lifespan_begin(sg), SGA_StreamGraph_lifespan_end(sg)));
}

size_t SGA_NodesSet_size(StreamGraph* sg) {
	size_t size = 0;
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		size += IntervalsSet_size(sg->nodes.nodes[i].presence);
	}
	return size;
}

size_t SGA_LinksSet_size(StreamGraph* sg) {
	size_t size = 0;
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		size += IntervalsSet_size(sg->links.links[i].presence);
	}
	return size;
}

size_t SGA_number_of_nodes(StreamGraph* sg) {
	return sg->nodes.nb_nodes;
}

double SGA_number_of_temporal_nodes(StreamGraph* sg) {
	return (double)SGA_NodesSet_size(sg) / (double)SGA_size_of_lifespan(sg);
}

double SGA_coverage(StreamGraph* sg) {
	size_t size_of_nodes = SGA_NodesSet_size(sg);
	size_t max_possible = sg->nodes.nb_nodes * SGA_size_of_lifespan(sg);
	return (double)size_of_nodes / (double)max_possible;
}

double SGA_number_of_links(StreamGraph* sg) {
	return (double)SGA_LinksSet_size(sg) / (double)SGA_size_of_lifespan(sg);
}

double SGA_node_duration(StreamGraph* sg) {
	return ((double)SGA_NodesSet_size(sg) / (double)SGA_number_of_nodes(sg)) / (double)sg->scaling;
}

size_t SGA_size_unord_pairs_set_itself(size_t n) {
	return n * (n - 1) / 2;
}

double SGA_link_duration(StreamGraph* sg) {
	return ((double)SGA_LinksSet_size(sg) / (double)SGA_size_unord_pairs_set_itself(SGA_number_of_nodes(sg))) /
		   (double)sg->scaling;
}

double SGA_contribution_of_node(StreamGraph* sg, size_t node_id) {
	size_t size = IntervalsSet_size(sg->nodes.nodes[node_id].presence);
	return (double)size / (double)SGA_size_of_lifespan(sg);
}

double SGA_contribution_of_link(StreamGraph* sg, size_t link_id) {
	size_t size = IntervalsSet_size(sg->links.links[link_id].presence);
	return (double)size / (double)SGA_size_of_lifespan(sg);
}

double SGA_uniformity(StreamGraph* sg) {
	size_t sum_union = 0;
	size_t sum_intersection = 0;
	for (size_t u = 0; u < sg->nodes.nb_nodes; u++) {
		for (size_t v = u + 1; v < sg->nodes.nb_nodes; v++) {
			IntervalsSet intersection_uv =
				IntervalsSet_intersection(sg->nodes.nodes[u].presence, sg->nodes.nodes[v].presence);
			size_t size_intersection = IntervalsSet_size(intersection_uv);

			IntervalsSet union_uv = IntervalsSet_union(sg->nodes.nodes[u].presence, sg->nodes.nodes[v].presence);
			size_t size_union = IntervalsSet_size(union_uv);

			IntervalsSet_destroy(union_uv);
			IntervalsSet_destroy(intersection_uv);

			sum_union += size_union;
			sum_intersection += size_intersection;
		}
	}

	return (double)sum_intersection / (double)sum_union;
}

double SGA_density(StreamGraph* sg) {
	size_t sum_link_durations = 0;
	size_t sum_duration_inter_nodes = 0;
	for (size_t i = 0; i < sg->links.nb_links; i++) {
		sum_link_durations += IntervalsSet_size(sg->links.links[i].presence);
	}

	for (size_t u = 0; u < sg->nodes.nb_nodes; u++) {
		for (size_t v = u + 1; v < sg->nodes.nb_nodes; v++) {
			IntervalsSet intersection_uv =
				IntervalsSet_intersection(sg->nodes.nodes[u].presence, sg->nodes.nodes[v].presence);
			size_t size_intersection = IntervalsSet_size(intersection_uv);

			sum_duration_inter_nodes += size_intersection;

			IntervalsSet_destroy(intersection_uv);
		}
	}

	return (double)sum_link_durations / (double)sum_duration_inter_nodes;
}

DEFAULT_MIN_MAX(size_t)

double SGA_compactness(StreamGraph* sg) {
	size_t sum_node_duration = 0;
	size_t first_node_appearance = SGA_StreamGraph_lifespan_end(sg);
	size_t last_node_disappearance = SGA_StreamGraph_lifespan_begin(sg);
	size_t number_of_nodes_with_presence = 0;
	for (size_t i = 0; i < sg->nodes.nb_nodes; i++) {
		size_t size = IntervalsSet_size(sg->nodes.nodes[i].presence);
		if (size > 0) {
			number_of_nodes_with_presence++;
			sum_node_duration += size;

			first_node_appearance =
				size_t_min(first_node_appearance, SGA_TemporalNode_first_appearance(&sg->nodes.nodes[i]));
			last_node_disappearance =
				size_t_max(last_node_disappearance, SGA_TemporalNode_last_disappearance(&sg->nodes.nodes[i]));
		}
	}

	printf("sum_node_duration: %zu\n", sum_node_duration);
	printf("first_node_appearance: %zu\n", first_node_appearance);
	printf("last_node_disappearance: %zu\n", last_node_disappearance);
	printf("number_of_nodes_with_presence: %zu\n", number_of_nodes_with_presence);

	return (double)sum_node_duration /
		   (double)((last_node_disappearance - first_node_appearance) * number_of_nodes_with_presence);
}