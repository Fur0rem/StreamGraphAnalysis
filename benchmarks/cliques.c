#include "../src/cliques.h"
#include "../src/stream.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "benchmark.h"

Stream st;

DONT_OPTIMISE void benchmark_maximal_cliques() {
	CliqueVector cliques = Stream_maximal_cliques(&st);
	CliqueVector_destroy(cliques);
	// CliqueVector cliques = maximal_cliques(&st);
	// CliqueVector cliques = maximal_cliques(&sg);
	// CliqueVector cliques = maximal_cliques(&v);
}

int main() {
	StreamGraph sg = StreamGraph_from_external("data/cliques.txt");
	st = FullStreamGraph_from(&sg);

	// CliqueVector cliques = maximal_cliques(&st);
	// CliqueVector cliques = maximal_cliques(&sg);

	/*LinksIterator it = funcs.links_set(&st);
	FOR_EACH_LINK(link_id, it) {
		Link link = sg.links.links[link_id];
		LinkVector_push(&v, link);
	}*/

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 10000);

	StreamGraph_destroy(sg);

	/*sg = StreamGraph_from_external("data/LS_90.txt");
	st = FullStreamGraph_from(&sg);

	LinkVector_destroy(v);
	v = LinkVector_new();
	for (size_t i = 0; i < sg.links.nb_links; i++) {
		Link link = sg.links.links[i];
		LinkVector_push(&v, link);
	}

	benchmark(benchmark_maximal_cliques, "maximal_cliques", 10);*/

	return 0;
}