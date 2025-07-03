/**
 * @file benchmarks/metrics.c
 * @brief Benchmarks for metrics
 */

#include <stdio.h>
#define SGA_INTERNAL
#include "../StreamGraphAnalysis.h"
#include "benchmark.h"

SGA_Stream stream;

DONT_OPTIMISE void number_of_links() {
	SGA_Stream_number_of_links(&stream);
	reset_cache(&stream);
}

DONT_OPTIMISE void coverage() {
	SGA_Stream_coverage(&stream);
	reset_cache(&stream);
}

DONT_OPTIMISE void uniformity() {
	SGA_Stream_node_duration(&stream);
	reset_cache(&stream);
}

DONT_OPTIMISE void link_times_present() {
	StreamFunctions fns  = STREAM_FUNCS(fns, &stream);
	SGA_TimesIterator it = fns.times_link_present(stream.stream_data, 1);
	size_t sum	     = 0;
	SGA_FOR_EACH_TIME(interval, it) {
		sum += interval.end - interval.start;
	}
}

char* file_name;

DONT_OPTIMISE void transitivity_ratio() {
	SGA_Stream_transitivity_ratio(&stream);
}

DONT_OPTIMISE void clustering_coeff() {
	SGA_Stream_node_clustering_coeff(&stream);
}

DONT_OPTIMISE void density_at_instant() {
	SGA_TimesIterator key_instants = SGA_Stream_key_instants(&stream);
	SGA_FOR_EACH_TIME(time, key_instants) {
		SGA_Stream_density_at_instant(&stream, time.start);
	}
}

#define BENCHMARK_METRIC(name, metric, nb_repeats)                                                                                         \
	stream = stream1;                                                                                                                  \
	benchmark(metric, name " S_concat_L", nb_repeats);                                                                                 \
	stream = stream2;                                                                                                                  \
	benchmark(metric, name " LS_90", nb_repeats);                                                                                      \
	stream = stream3;                                                                                                                  \
	benchmark(metric, name " primaryschool", nb_repeats);                                                                              \
	stream = stream4;                                                                                                                  \
	benchmark(metric, name " facebooklike", 1);                                                                                        \
	printf("\n");

int main() {
	SGA_StreamGraph sg1 = SGA_StreamGraph_from_file("data/tests/S_concat_L.sga");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/benchmarks/LS_90.sga");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	SGA_StreamGraph sg3 = SGA_StreamGraph_from_file("data/benchmarks/primaryschool_3125_transformed.sga");
	SGA_Stream stream3  = SGA_FullStreamGraph_from(&sg3);

	SGA_StreamGraph sg4 = SGA_StreamGraph_from_file("data/benchmarks/facebooklike_1_transformed.sga");
	SGA_Stream stream4  = SGA_FullStreamGraph_from(&sg4);

	// BENCHMARK_METRIC("number_of_links", number_of_links, 1000);
	// BENCHMARK_METRIC("coverage", coverage, 10);
	// BENCHMARK_METRIC("uniformity", uniformity, 10);
	// BENCHMARK_METRIC("transitivity_ratio", transitivity_ratio, 10);
	// BENCHMARK_METRIC("clustering_coeff", clustering_coeff, 10);
	BENCHMARK_METRIC("density_at_instant", density_at_instant, 3);
	// BENCHMARK_METRIC("link_times_present", link_times_present, 2000);

	SGA_StreamGraph_destroy(sg1);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream2);
	SGA_StreamGraph_destroy(sg3);
	SGA_FullStreamGraph_destroy(stream3);
	SGA_StreamGraph_destroy(sg4);
	SGA_FullStreamGraph_destroy(stream4);

	return 0;
}