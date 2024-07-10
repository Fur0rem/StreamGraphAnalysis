#include "../src/cliques.h"
#include "../src/induced_graph.h"
#include "../src/stream.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream_graph.h"
#include "../src/units.h"

#include "test.h"
#include <stdio.h>

// TODO : change that temporary shit because linking is hell
bool test_full_stream_graph() {
	/*StreamGraph sg = StreamGraph_from_file("tests/test_data/internal_cliques.txt");
	Stream* st = FSG_From(&sg);
	printf("Loaded graph\n");
	char* str = StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);

	init_events_table(&sg);

	StreamFunctions funcs = FullStreamGraph_stream_functions;

	printf("Links at time 5\n");

	printf("v1\n");
	LinksIterator links_it = get_links_present_at_t(&sg, 5);
	FOR_EACH_LINK(link_id, links_it) {
		printf("Link %zu\n", link_id);
	}

	FullStreamGraph fsg = (FullStreamGraph){
		.underlying_stream_graph = &sg,
	};
	Stream st0 = (Stream){
		.type = FULL_STREAM_GRAPH,
		.stream = &fsg,
	};

	printf("v2\n");
	LinksIterator links_iter = funcs.links_present_at_t(&st0, 5);
	FOR_EACH_LINK(link_id, links_iter) {
		printf("Link %zu\n", link_id);
	}*/

	return true;
}

// TODO : fix issues with streams so that we dont have to build the vector of links
bool test_maximal_cliques() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/internal_cliques.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");
	char* str = StreamGraph_to_string(&sg);
	printf("%s\n", str);
	free(str);

	init_events_table(&sg);

	// CliqueVector cliques = maximal_cliques(&st);
	// CliqueVector cliques = maximal_cliques(&sg);

	StreamFunctions funcs = FullStreamGraph_stream_functions;
	LinkVector v = LinkVector_new();
	/*LinksIterator it = funcs.links_set(&st);
	FOR_EACH_LINK(link_id, it) {
		Link link = sg.links.links[link_id];
		LinkVector_push(&v, link);
	}*/

	for (size_t i = 0; i < sg.links.nb_links; i++) {
		Link link = sg.links.links[i];
		LinkVector_push(&v, link);
	}

	CliqueVector cliques = maximal_cliques(v);

	char* cliques_str = CliqueVector_to_string(&cliques);
	printf("%s\n", cliques_str);
	free(cliques_str);

	StreamGraph_destroy(sg);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"full_stream_graph", test_full_stream_graph},
		  &(Test){"maximal_cliques",	 test_maximal_cliques	 },

		NULL
	};

	return test("Cliques", tests);
}