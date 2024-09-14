#include "../src/analysis/cliques.h"
#include "../src/stream.h"
#include "../src/stream_wrappers.h"
#include "benchmark.h"

Stream st;

DONT_OPTIMISE void benchmark_maximal_cliques() {
	CliqueArrayList cliques = Stream_maximal_cliques(&st);
	CliqueArrayList_destroy(cliques);
	// CliqueArrayList cliques = maximal_cliques(&st);
	// CliqueArrayList cliques = maximal_cliques(&sg);
	// CliqueArrayList cliques = maximal_cliques(&v);
}

int main() {
	StreamGraph sg = StreamGraph_from_external("data/cliques.txt");
	st			   = FullStreamGraph_from(&sg);

	// CliqueArrayList cliques = maximal_cliques(&st);
	// CliqueArrayList cliques = maximal_cliques(&sg);

	/*LinksIterator it = funcs.links_set(&st);
	FOR_EACH_LINK(link_id, it) {
		Link link = sg.links.links[link_id];
		LinkArrayList_push(&v, link);
	}*/

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 10000);

	StreamGraph_destroy(sg);

	sg = StreamGraph_from_external("data/LS_90.txt");
	st = FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques LS_90", 10);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);

	/*sg = StreamGraph_from_external("data/LS_90.txt");
	st = FullStreamGraph_from(&sg);

	LinkArrayList_destroy(v);
	v = LinkArrayList_new();
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		Link link = sg.links.links[i];
		LinkArrayList_push(&v, link);
	}

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 10);*/

	sg = StreamGraph_from_external("data/facebooklike_1_transformed.txt");
	st = FullStreamGraph_from(&sg);

	benchmark(benchmark_maximal_cliques, "maximal_cliques facebooklike", 10);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);

	return 0;
}