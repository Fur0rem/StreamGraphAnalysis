#include "../src/analysis/cliques.h"
#include "../src/stream.h"
#include "../src/streams.h"
#include "benchmark.h"

SGA_Stream st;

DONT_OPTIMISE void benchmark_maximal_cliques() {
	SGA_CliqueArrayList cliques = SGA_Stream_maximal_cliques(&st);
	SGA_CliqueArrayList_destroy(cliques);
	// CliqueArrayList cliques = maximal_cliques(&st);
	// CliqueArrayList cliques = maximal_cliques(&sg);
	// CliqueArrayList cliques = maximal_cliques(&v);
}

int main() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/cliques.sga");
	st		   = SGA_FullStreamGraph_from(&sg);

	// CliqueArrayList cliques = maximal_cliques(&st);
	// CliqueArrayList cliques = maximal_cliques(&sg);

	/*LinksIterator it = funcs.links_set(&st);
	FOR_EACH_LINK(link_id, it) {
		Link link = sg.links.links[link_id];
		LinkArrayList_push(&v, link);
	}*/

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 10000);

	SGA_StreamGraph_destroy(sg);

	sg = SGA_StreamGraph_from_file("data/benchmarks/LS_90.sga");
	st = SGA_FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques LS_90", 10);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);

	/*sg = SGA_StreamGraph_from_file("data/benchmarks/LS_90.sga");
	st = SGA_FullStreamGraph_from(&sg);

	LinkArrayList_destroy(v);
	v = LinkArrayList_new();
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		Link link = sg.links.links[i];
		LinkArrayList_push(&v, link);
	}

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 10);*/

	sg = SGA_StreamGraph_from_file("data/benchmarks/facebooklike_1_transformed.sga");
	st = SGA_FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques facebooklike", 10);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);

	return 0;
}