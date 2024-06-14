#include "../src/induced_graph.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_NODE_AT_TIME(&sg, node, 40) {
		printf("Node : %zu\n", *node);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_2() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_NODE_AT_TIME(&sg, node, 60) {
		printf("Node : %zu\n", *node);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_3() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_NODE_AT_TIME(&sg, node, 100) {
		printf("Node : %zu\n", *node);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_LINK_AT_TIME(&sg, link, 40) {
		printf("Link : %zu (%zu - %zu)\n", *link, sg.links.links[*link].nodes[0], sg.links.links[*link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_2() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_LINK_AT_TIME(&sg, link, 60) {
		printf("Link : %zu (%zu - %zu)\n", *link, sg.links.links[*link].nodes[0], sg.links.links[*link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_3() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_LINK_AT_TIME(&sg, link, 100) {
		printf("Link : %zu (%zu - %zu)\n", *link, sg.links.links[*link].nodes[0], sg.links.links[*link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_4() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_LINK_AT_TIME(&sg, link, 40) {
		printf("Link : %zu (%zu - %zu)\n", *link, sg.links.links[*link].nodes[0], sg.links.links[*link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

bool test_links_at_time_inbetween() {
	StreamGraph sg = StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_LINK_AT_TIME(&sg, link, 25) {
		printf("Link : %zu (%zu - %zu)\n", *link, sg.links.links[*link].nodes[0], sg.links.links[*link].nodes[1]);
	}
	StreamGraph_destroy(&sg);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"nodes_at_time",			 test_nodes_at_time		   },
		&(Test){"nodes_at_time_2",		   test_nodes_at_time_2		   },
		&(Test){"nodes_at_time_3",		   test_nodes_at_time_3		   },
		&(Test){"links_at_time",			 test_links_at_time		   },
		&(Test){"links_at_time_2",		   test_links_at_time_2		   },
		&(Test){"links_at_time_3",		   test_links_at_time_3		   },
		&(Test){"links_at_time_4",		   test_links_at_time_4		   },
		&(Test){"links_at_time_inbetween", test_links_at_time_inbetween},
		NULL
	};

	return test("StreamGraph", tests);
}