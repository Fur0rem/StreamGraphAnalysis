#include "../src/interval.h"
#include "../src/stream.h"
#include "../src/stream_functions.h"
#include "../src/stream_wrappers.h"
#include "../src/units.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_links_at_time_80() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	Stream stream	    = FullStreamGraph_from(&sg);
	StreamFunctions fns = FullStreamGraph_stream_functions;

	LinksIterator links_present_at_80 = fns.links_present_at_t(stream.stream_data, 80);
	FOR_EACH_LINK(link, links_present_at_80) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);
	return true;
}

int main() {
	Test* tests[] = {
	    TEST(test_links_at_time_80),
	    NULL,
	};

	return test("Induced Graph", tests);
}