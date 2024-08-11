#include "../src/stream/link_stream.h"
#include "../src/interval.h"
#include "../src/stream.h"
#include "../src/units.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_links_at_time_80() {
	StreamGraph sg = StreamGraph_from_file("data/L.txt");
	init_events_table(&sg);
	Stream ls = LS_from(&sg);
	StreamFunctions funcs = LinkStream_stream_functions;

	LinkIdVector links = SGA_collect_link_ids(funcs.links_present_at_t(ls.stream_data, 80));
	LinkIdVector expected = LinkIdVector_new();
	String str = LinkIdVector_to_string(&links);
	printf("%s\n", str.data);
	String_destroy(str);
	LinkIdVector_destroy(links);
	LinkIdVector_destroy(expected);
	StreamGraph_destroy(sg);
	// Stream_destroy(ls);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"test_links_at_time_80", test_links_at_time_80},

		NULL
	};

	return test("Induced Graph", tests);
}