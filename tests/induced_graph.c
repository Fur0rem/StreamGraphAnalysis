#include "../src/induced_graph.h"
#include "../src/stream_graph.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time_40() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_NODE_AT_TIME(&sg, node, 40) {
		printf("Node : %zu\n", node);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_60() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_NODE_AT_TIME(&sg, node, 60) {
		printf("Node : %zu\n", node);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_100() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_NODE_AT_TIME(&sg, node, 100) {
		printf("Node : %zu\n", node);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_30() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_LINK_AT_TIME(&sg, link, 30) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_60() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_LINK_AT_TIME(&sg, link, 60) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_100() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_LINK_AT_TIME(&sg, link, 100) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_40() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_LINK_AT_TIME(&sg, link, 40) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_25() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	FOR_EACH_LINK_AT_TIME(&sg, link, 25) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"nodes_at_time_40",	test_nodes_at_time_40 },
		&(Test){"nodes_at_time_60",	test_nodes_at_time_60 },
		&(Test){"nodes_at_time_100", test_nodes_at_time_100},
		&(Test){"links_at_time_30",	test_links_at_time_30 },
		&(Test){"links_at_time_60",	test_links_at_time_60 },
		&(Test){"links_at_time_100", test_links_at_time_100},
		&(Test){"links_at_time_40",	test_links_at_time_40 },
		&(Test){"links_at_time_25",	test_links_at_time_25 },
		NULL
	};

	return test("StreamGraph", tests);
}