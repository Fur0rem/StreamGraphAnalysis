#define SGA_INTERNAL

#include "../src/stream_data_access/induced_graph.h"
#include "../src/streams.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time_40() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodesIterator nodes_present_at_40 = SGA_StreamGraph_nodes_present_at(&sg, 40);
	SGA_FOR_EACH_NODE(node, nodes_present_at_40) {
		printf("Node : %zu\n", node);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_nodes_at_time_60() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_NodesIterator nodes_present_at_60 = SGA_StreamGraph_nodes_present_at(&sg, 60);
	SGA_FOR_EACH_NODE(node, nodes_present_at_60) {
		printf("Node : %zu\n", node);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_30() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_30 = SGA_StreamGraph_links_present_at(&sg, 30);
	SGA_FOR_EACH_LINK(link, links_present_at_30) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}

	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_60() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_60 = SGA_StreamGraph_links_present_at(&sg, 60);
	SGA_FOR_EACH_LINK(link, links_present_at_60) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_40() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_40 = SGA_StreamGraph_links_present_at(&sg, 40);
	SGA_FOR_EACH_LINK(link, links_present_at_40) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_25() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_25 = SGA_StreamGraph_links_present_at(&sg, 25);
	SGA_FOR_EACH_LINK(link, links_present_at_25) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_74() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_74 = SGA_StreamGraph_links_present_at(&sg, 74);
	SGA_FOR_EACH_LINK(link, links_present_at_74) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_75() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_75 = SGA_StreamGraph_links_present_at(&sg, 75);
	SGA_FOR_EACH_LINK(link, links_present_at_75) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_76() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_76 = SGA_StreamGraph_links_present_at(&sg, 76);
	SGA_FOR_EACH_LINK(link, links_present_at_76) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_80() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_80 = SGA_StreamGraph_links_present_at(&sg, 80);
	SGA_FOR_EACH_LINK(link, links_present_at_80) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_links_at_time_90() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_file("data/S.txt");
	SGA_LinksIterator links_present_at_90 = SGA_StreamGraph_links_present_at(&sg, 90);
	SGA_FOR_EACH_LINK(link, links_present_at_90) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	SGA_StreamGraph_destroy(sg);
	// TODO
	return true;
}

bool test_s_concat_l() {
	SGA_StreamGraph sg		      = SGA_StreamGraph_from_external("data/S_concat_L.txt");
	SGA_LinksIterator links_present_at_90 = SGA_StreamGraph_links_present_at(&sg, 90);
	SGA_FOR_EACH_LINK(link, links_present_at_90) {
		printf("Link : %zu (%zu - %zu)\n", link, sg.links.links[link].nodes[0], sg.links.links[link].nodes[1]);
	}
	// TODO
	return true;
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