#include "../src/stream_data_access/induced_graph.h"
#include "../src/stream_wrappers.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time_40() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	NodesIterator nodes_present_at_40 = StreamGraph_nodes_present_at(&sg, 40);
	FOR_EACH_NODE(node, nodes_present_at_40) {
		printf("Node : %zu\n", node);
	}
	events_destroy(&sg);

	StreamGraph_destroy(sg);
	return true;
}

bool test_nodes_at_time_60() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	NodesIterator nodes_present_at_60 = StreamGraph_nodes_present_at(&sg, 60);
	FOR_EACH_NODE(node, nodes_present_at_60) {
		printf("Node : %zu\n", node);
	}
	events_destroy(&sg);

	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_30() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_30 = StreamGraph_links_present_at(&sg, 30);
	FOR_EACH_LINK(link, links_present_at_30) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);

	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_60() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_60 = StreamGraph_links_present_at(&sg, 60);
	FOR_EACH_LINK(link, links_present_at_60) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_40() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_40 = StreamGraph_links_present_at(&sg, 40);
	FOR_EACH_LINK(link, links_present_at_40) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_25() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_25 = StreamGraph_links_present_at(&sg, 25);
	FOR_EACH_LINK(link, links_present_at_25) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_74() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_74 = StreamGraph_links_present_at(&sg, 74);
	FOR_EACH_LINK(link, links_present_at_74) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_75() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_75 = StreamGraph_links_present_at(&sg, 75);
	FOR_EACH_LINK(link, links_present_at_75) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_76() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_76 = StreamGraph_links_present_at(&sg, 76);
	FOR_EACH_LINK(link, links_present_at_76) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_80() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_80 = StreamGraph_links_present_at(&sg, 80);
	FOR_EACH_LINK(link, links_present_at_80) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_links_at_time_90() {
	StreamGraph sg = StreamGraph_from_file("data/S.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_90 = StreamGraph_links_present_at(&sg, 90);
	FOR_EACH_LINK(link, links_present_at_90) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
	StreamGraph_destroy(sg);
	return true;
}

bool test_s_concat_l() {
	StreamGraph sg = StreamGraph_from_external("data/S_concat_L.txt");
	init_events_table(&sg);
	LinksIterator links_present_at_90 = StreamGraph_links_present_at(&sg, 90);
	FOR_EACH_LINK(link, links_present_at_90) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	events_destroy(&sg);
}

int main() {
	Test* tests[] = {&(Test){"nodes_at_time_40", test_nodes_at_time_40},
					 &(Test){"nodes_at_time_60", test_nodes_at_time_60},
					 &(Test){"links_at_time_30", test_links_at_time_30},
					 &(Test){"links_at_time_60", test_links_at_time_60},
					 &(Test){"links_at_time_40", test_links_at_time_40},
					 &(Test){"links_at_time_25", test_links_at_time_25},
					 &(Test){"links_at_time_74", test_links_at_time_74},
					 &(Test){"links_at_time_75", test_links_at_time_75},
					 &(Test){"links_at_time_76", test_links_at_time_76},
					 &(Test){"links_at_time_80", test_links_at_time_80},
					 &(Test){"links_at_time_90", test_links_at_time_90},

					 // TODO: tests which are expected to crash
					 // &(Test){"test_links_at_time_100", test_links_at_time_100},

					 NULL};

	return test("StreamGraph", tests);
}