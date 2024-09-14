#include "../src/analysis/walks.h"
#include "../src/stream.h"
#include "../src/stream_wrappers.h"
#include "benchmark.h"

Stream stream;

DONT_OPTIMISE void robustness_by_length() {
	Stream_robustness_by_length(&stream);
}

DONT_OPTIMISE void robustness_by_duration() {
	Stream_robustness_by_duration(&stream);
}

DONT_OPTIMISE void fastest() {
	MetricsFunctions fns = FullStreamGraph_metrics_functions;
	size_t nb_nodes		 = fns.cardinalOfV(&stream);
	for (size_t i = 0; i < nb_nodes; i++) {
		for (size_t j = 0; j < nb_nodes; j++) {
			if (i == j) {
				continue;
			}
			printf("Optimal walks between %zu and %zu\n", i, j);
			WalkInfoArrayList optimal_walks = optimal_walks_between_two_nodes(&stream, i, j, Stream_fastest_walk);
			WalkInfoArrayList_destroy(optimal_walks);
		}
	}
}

int main() {
	StreamGraph sg;
	sg	   = StreamGraph_from_external("data/S_external.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length Figure_8", 1000);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/S_concat_L.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length S_concat_L", 1000);
	benchmark(robustness_by_duration, "robustness_by_duration S_concat_L", 1000);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/LS_90.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(robustness_by_duration, "robustness_by_duration LS_90", 1);
	benchmark(fastest, "fastest LS_90", 1);
	benchmark(robustness_by_length, "robustness_by_length LS_90", 1);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	return 0;
}