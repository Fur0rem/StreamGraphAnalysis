/**
 * @file tests/full_stream_graph.c
 * @brief Tests regarding FullStreamGraph
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_links_at_time_80() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");
	// init_events_table(&sg);
	SGA_Stream stream   = SGA_FullStreamGraph_from(&sg);
	StreamFunctions fns = FullStreamGraph_stream_functions;

	SGA_LinksIterator links_present_at_80 = fns.links_present_at_t(stream.stream_data, 80);
	SGA_FOR_EACH_LINK(link, links_present_at_80) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	// events_destroy(&sg);
	SGA_FullStreamGraph_destroy(stream);
	SGA_StreamGraph_destroy(sg);
	return true;
}

int main() {
	Test* tests[] = {
	    TEST(test_links_at_time_80),
	    NULL,
	};

	return test("Induced Graph", tests);
}