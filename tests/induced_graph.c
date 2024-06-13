#include "../src/induced_graph.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_NODE_AT_TIME(&sg, node, 20) {
		printf("Node : %zu : %s\n", *node, sg.node_names[*node]);
		sleep(1);
	}
	SGA_StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_2() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_NODE_AT_TIME(&sg, node, 60) {
		printf("Node : %zu : %s\n", *node, sg.node_names[*node]);
		sleep(1);
	}
	SGA_StreamGraph_destroy(&sg);
	return true;
}

bool test_nodes_at_time_3() {
	StreamGraph sg = SGA_StreamGraph_from_file("tests/test_data/S.txt");
	FOR_EACH_NODE_AT_TIME(&sg, node, 100) {
		printf("Node : %zu : %s\n", *node, sg.node_names[*node]);
		sleep(1);
	}
	SGA_StreamGraph_destroy(&sg);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"nodes_at_time",	 test_nodes_at_time  },
		  &(Test){"nodes_at_time_2", test_nodes_at_time_2},
		&(Test){"nodes_at_time_3", test_nodes_at_time_3},
		  NULL
	  };

	return test("StreamGraph", tests);
}