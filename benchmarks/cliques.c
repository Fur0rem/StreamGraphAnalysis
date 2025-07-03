#include "../src/analysis/cliques.h"
#include "../src/stream.h"
#include "../src/streams.h"
#include "benchmark.h"

SGA_Stream st;

DONT_OPTIMISE void benchmark_maximal_cliques() {
	SGA_CliqueArrayList cliques = SGA_Stream_maximal_cliques(&st);
	SGA_CliqueArrayList_destroy(cliques);
}

int main() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/cliques.sga");
	st		   = SGA_FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 500);

	SGA_StreamGraph_destroy(sg);

	sg = SGA_StreamGraph_from_file("data/benchmarks/LS_90.sga");
	st = SGA_FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques LS_90", 10);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);

	sg = SGA_StreamGraph_from_file("data/benchmarks/primaryschool_3125_transformed.sga");
	st = SGA_FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques primaryschool", 10);

	sg = SGA_StreamGraph_from_file("data/benchmarks/facebooklike_1_transformed.sga");
	st = SGA_FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques facebooklike", 10);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);

	return 0;
}