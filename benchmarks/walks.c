/**
 * @file benchmarks/walks.c
 * @brief Benchmarks for walks
 */

#define SGA_INTERNAL
#include "../StreamGraphAnalysis.h"
#include "benchmark.h"

SGA_Stream stream;

DONT_OPTIMISE void robustness_by_length() {
	Stream_robustness_by_length(&stream);
}

DONT_OPTIMISE void robustness_by_duration() {
	Stream_robustness_by_duration(&stream);
}

DONT_OPTIMISE void fastest() {
	MetricsFunctions fns = FullStreamGraph_metrics_functions;
	size_t nb_nodes	     = fns.distinct_cardinal_of_node_set(&stream);
	for (size_t i = 0; i < nb_nodes; i++) {
		for (size_t j = 0; j < nb_nodes; j++) {
			if (i == j) {
				continue;
			}
			printf("Optimal walks between %zu and %zu\n", i, j);
			WalkInfoArrayList optimal_walks = optimal_walks_between_two_nodes(&stream, i, j, SGA_fastest_walk);
			WalkInfoArrayList_destroy(optimal_walks);
		}
	}
}

int main() {
	SGA_StreamGraph sg;
	sg     = SGA_StreamGraph_from_external("data/S_external.txt");
	stream = SGA_FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length Figure_8", 1000);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(stream);

	sg     = SGA_StreamGraph_from_external("data/S_concat_L.txt");
	stream = SGA_FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length S_concat_L", 1000);
	benchmark(robustness_by_duration, "robustness_by_duration S_concat_L", 1000);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(stream);

	sg     = SGA_StreamGraph_from_external("data/LS_90.txt");
	stream = SGA_FullStreamGraph_from(&sg);
	benchmark(robustness_by_duration, "robustness_by_duration LS_90", 1);
	benchmark(fastest, "fastest LS_90", 1);
	benchmark(robustness_by_length, "robustness_by_length LS_90", 1);
	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(stream);

	return 0;
}