#include "../src/induced_graph.h"
#include "../src/stream_graph.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time_40() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	NodesIterator nodes_present_at_40 = get_nodes_present_at_t(&sg, 40);
	FOR_EACH_NODE(nodes_present_at_40, node) {
		printf("Node : %zu\n", node);
	}
	events_destroy(&sg);

	StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_60() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	NodesIterator nodes_present_at_60 = get_nodes_present_at_t(&sg, 60);
	FOR_EACH_NODE(nodes_present_at_60, node) {
		printf("Node : %zu\n", node);
	}
	events_destroy(&sg);

	StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_100() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	NodesIterator nodes_present_at_100 = get_nodes_present_at_t(&sg, 100);
	FOR_EACH_NODE(nodes_present_at_100, node) {
		printf("Node : %zu\n", node);
	}
	events_destroy(&sg);

	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_30() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_30 = get_links_present_at_t(&sg, 30);
	FOR_EACH_LINK(links_present_at_30, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);

	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_60() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_60 = get_links_present_at_t(&sg, 60);
	FOR_EACH_LINK(links_present_at_60, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_100() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_100 = get_links_present_at_t(&sg, 100);
	FOR_EACH_LINK(links_present_at_100, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_40() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_40 = get_links_present_at_t(&sg, 40);
	FOR_EACH_LINK(links_present_at_40, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_25() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_25 = get_links_present_at_t(&sg, 25);
	FOR_EACH_LINK(links_present_at_25, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_74() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_74 = get_links_present_at_t(&sg, 74);
	FOR_EACH_LINK(links_present_at_74, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_75() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_75 = get_links_present_at_t(&sg, 75);
	FOR_EACH_LINK(links_present_at_75, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_76() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_76 = get_links_present_at_t(&sg, 76);
	FOR_EACH_LINK(links_present_at_76, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_80() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_80 = get_links_present_at_t(&sg, 80);
	FOR_EACH_LINK(links_present_at_80, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_90() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_90 = get_links_present_at_t(&sg, 90);
	FOR_EACH_LINK(links_present_at_90, link) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
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
		&(Test){"links_at_time_40",	test_links_at_time_40 },
		&(Test){"links_at_time_25",	test_links_at_time_25 },
		&(Test){"links_at_time_74",	test_links_at_time_74 },
		&(Test){"links_at_time_75",	test_links_at_time_75 },
		&(Test){"links_at_time_76",	test_links_at_time_76 },
		&(Test){"links_at_time_80",	test_links_at_time_80 },
		&(Test){"links_at_time_90",	test_links_at_time_90 },
		&(Test){"links_at_time_100", test_links_at_time_100},

		NULL
	};

	return test("StreamGraph", tests);
}