#undef SGA_INTERNAL // TODO just don't define it
#include "../StreamGraphAnalysis.h"
#include <stdlib.h>

int main() {
	// SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/S_external.txt");
	// SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	char* content	   = read_file("../data/S_external.txt");
	char* to_internal  = SGA_InternalFormat_from_External_str(content);
	SGA_StreamGraph sg = SGA_StreamGraph_from_string(to_internal);

	SGA_Stream st = SGA_FullStreamGraph_from(&sg);

	printf("------ Basic information ------\n");
	printf("Temporal cardinal of nodes: %lu\n", SGA_Stream_temporal_cardinal_of_node_set(&st));
	printf("Distinct cardinal of nodes: %lu\n", SGA_Stream_distinct_cardinal_of_node_set(&st));
	printf("Temporal cardinal of links: %lu\n", SGA_Stream_temporal_cardinal_of_link_set(&st));
	printf("Distinct cardinal of links: %lu\n", SGA_Stream_distinct_cardinal_of_link_set(&st));
	printf("Duration: %lu\n", SGA_Stream_duration(&st));

	printf("\n------ Stream metrics ------\n");
	printf("Coverage: %f\n", SGA_Stream_coverage(&st));
	printf("Number of nodes: %f\n", SGA_Stream_number_of_nodes(&st));
	printf("Node duration: %f\n", SGA_Stream_node_duration(&st));
	printf("Link duration: %f\n", SGA_Stream_link_duration(&st));
	printf("Uniformity: %f\n", SGA_Stream_uniformity(&st));
	printf("Compactness: %f\n", SGA_Stream_compactness(&st));
	printf("Density: %f\n", SGA_Stream_density(&st));
	printf("Average node degree: %f\n", SGA_Stream_average_node_degree(&st));
	printf("Node clustering coefficient: %f\n", SGA_Stream_node_clustering_coeff(&st));
	printf("Transitivity ratio: %f\n", SGA_Stream_transitivity_ratio(&st));

	printf("\n------ Node and link metrics ------\n");
	SGA_NodesIterator nodes_set = SGA_Stream_nodes_set(&st);
	SGA_FOR_EACH_NODE(node, nodes_set) {
		printf("Node %lu | Contribution: %f | Density: %f | Degree: %f | Clustering coefficient: %f\n",
		       node,
		       SGA_Stream_contribution_of_node(&st, node),
		       SGA_Stream_density_of_node(&st, node),
		       SGA_Stream_degree_of_node(&st, node),
		       SGA_Stream_clustering_coeff_of_node(&st, node));
	}

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}