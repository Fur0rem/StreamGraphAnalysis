#include "../src/stream/full_stream_graph.h"
#include "../src/interval.h"
#include "../src/stream.h"
#include "../src/stream_functions.h"
#include "../src/units.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_links_at_time_80() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	Stream st = FullStreamGraph_from(&sg);
	StreamFunctions funcs = FullStreamGraph_stream_functions;
	LinksIterator links_present_at_80 = funcs.links_present_at_t(st.stream_data, 80);
	FOR_EACH_LINK(link, links_present_at_80) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"test_links_at_time_80", test_links_at_time_80},

		NULL
	};

	return test("Induced Graph", tests);
}