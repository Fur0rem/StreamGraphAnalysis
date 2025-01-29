#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <unistd.h>

bool test_nodes_at_time_0() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_0 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 0));
	SGA_NodeIdArrayList expected	       = SGA_NodeIdArrayList_from(2, 0, 1);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_0);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_0, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_0);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_time_5() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_5 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 5));
	SGA_NodeIdArrayList expected	       = SGA_NodeIdArrayList_from(2, 0, 1);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_5);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_5, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_5);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_time_10() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_10 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 10));
	SGA_NodeIdArrayList expected		= SGA_NodeIdArrayList_from(3, 0, 1, 3);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_10);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_10, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_10);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_time_40() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_40 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 40));
	SGA_NodeIdArrayList expected		= SGA_NodeIdArrayList_from(2, 0, 2);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_40);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_40, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_40);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_time_60() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_60 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 60));
	SGA_NodeIdArrayList expected		= SGA_NodeIdArrayList_from(3, 0, 1, 2);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_60);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_60, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_60);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_time_85() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_85 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 85));
	SGA_NodeIdArrayList expected		= SGA_NodeIdArrayList_from(3, 0, 1, 2);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_85);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_85, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_85);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_90() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_90 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 90));
	SGA_NodeIdArrayList expected		= SGA_NodeIdArrayList_from(2, 0, 1);

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_90);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_90, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_90);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_nodes_at_100() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_NodeIdArrayList nodes_present_at_100 = SGA_collect_node_ids(SGA_StreamGraph_nodes_present_at(&sg, 100));
	SGA_NodeIdArrayList expected		 = SGA_NodeIdArrayList_empty();

	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_100);
	SGA_NodeIdArrayList_sort_unstable(&expected);
	bool success = SGA_NodeIdArrayList_equals(&nodes_present_at_100, &expected);

	SGA_NodeIdArrayList_destroy(nodes_present_at_100);
	SGA_NodeIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_25() {
	SGA_StreamGraph sg			= SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_LinkIdArrayList links_present_at_25 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 25));
	SGA_LinkIdArrayList expected =
	    SGA_LinkIdArrayList_from(2, SGA_StreamGraph_link_between_nodes(&sg, 0, 1), SGA_StreamGraph_link_between_nodes(&sg, 1, 3));

	SGA_LinkIdArrayList_sort_unstable(&links_present_at_25);
	SGA_LinkIdArrayList_sort_unstable(&expected);
	bool success = SGA_LinkIdArrayList_equals(&links_present_at_25, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_25);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_30() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_30 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 30));
	SGA_LinkIdArrayList expected		= SGA_LinkIdArrayList_empty();

	bool success = SGA_LinkIdArrayList_equals(&links_present_at_30, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_30);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_40() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_40 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 40));
	SGA_LinkIdArrayList expected		= SGA_LinkIdArrayList_empty();

	bool success = SGA_LinkIdArrayList_equals(&links_present_at_40, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_40);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_60() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_60 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 60));
	SGA_LinkIdArrayList expected =
	    SGA_LinkIdArrayList_from(2, SGA_StreamGraph_link_between_nodes(&sg, 0, 2), SGA_StreamGraph_link_between_nodes(&sg, 1, 2));

	SGA_LinkIdArrayList_sort_unstable(&links_present_at_60);
	SGA_LinkIdArrayList_sort_unstable(&expected);
	bool success = SGA_LinkIdArrayList_equals(&links_present_at_60, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_60);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);
	return success;
}

bool test_links_at_time_74() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_74 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 74));
	SGA_LinkIdArrayList expected		= SGA_LinkIdArrayList_from(3,
								   SGA_StreamGraph_link_between_nodes(&sg, 0, 1),
								   SGA_StreamGraph_link_between_nodes(&sg, 1, 2),
								   SGA_StreamGraph_link_between_nodes(&sg, 0, 2));

	SGA_LinkIdArrayList_sort_unstable(&links_present_at_74);
	SGA_LinkIdArrayList_sort_unstable(&expected);
	bool success = SGA_LinkIdArrayList_equals(&links_present_at_74, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_74);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_75() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_75 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 75));
	SGA_LinkIdArrayList expected =
	    SGA_LinkIdArrayList_from(2, SGA_StreamGraph_link_between_nodes(&sg, 0, 1), SGA_StreamGraph_link_between_nodes(&sg, 1, 2));

	SGA_LinkIdArrayList_sort_unstable(&links_present_at_75);
	SGA_LinkIdArrayList_sort_unstable(&expected);
	bool success = SGA_LinkIdArrayList_equals(&links_present_at_75, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_75);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_76() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_76 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 76));
	SGA_LinkIdArrayList expected =
	    SGA_LinkIdArrayList_from(2, SGA_StreamGraph_link_between_nodes(&sg, 0, 1), SGA_StreamGraph_link_between_nodes(&sg, 1, 2));

	bool success = SGA_LinkIdArrayList_equals(&links_present_at_76, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_76);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_80() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_80 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 80));
	SGA_LinkIdArrayList expected		= SGA_LinkIdArrayList_from(1, SGA_StreamGraph_link_between_nodes(&sg, 1, 2));

	SGA_LinkIdArrayList_sort_unstable(&links_present_at_80);
	SGA_LinkIdArrayList_sort_unstable(&expected);
	bool success = SGA_LinkIdArrayList_equals(&links_present_at_80, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_80);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_90() {

	SGA_StreamGraph sg			= SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_LinkIdArrayList links_present_at_90 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 90));
	SGA_LinkIdArrayList expected		= SGA_LinkIdArrayList_empty();

	bool success = SGA_LinkIdArrayList_equals(&links_present_at_90, &expected);
	SGA_LinkIdArrayList_destroy(links_present_at_90);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

bool test_links_at_time_100() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/S.sga");

	SGA_LinkIdArrayList links_present_at_100 = SGA_collect_link_ids(SGA_StreamGraph_links_present_at(&sg, 100));
	SGA_LinkIdArrayList expected		 = SGA_LinkIdArrayList_empty();

	bool success = SGA_LinkIdArrayList_equals(&links_present_at_100, &expected);

	SGA_LinkIdArrayList_destroy(links_present_at_100);
	SGA_LinkIdArrayList_destroy(expected);
	SGA_StreamGraph_destroy(sg);

	return success;
}

int main() {
	Test* tests[] = {
	    &(Test){"nodes_at_time_0", test_nodes_at_time_0},
	    &(Test){"nodes_at_time_5", test_nodes_at_time_5},
	    &(Test){"nodes_at_time_10", test_nodes_at_time_10},
	    &(Test){"nodes_at_time_40", test_nodes_at_time_40},
	    &(Test){"nodes_at_time_60", test_nodes_at_time_60},
	    &(Test){"nodes_at_time_85", test_nodes_at_time_85},
	    &(Test){"nodes_at_time_90", test_nodes_at_90},
	    &(Test){"nodes_at_time_100", test_nodes_at_100},
	    &(Test){"links_at_time_25", test_links_at_time_25},
	    &(Test){"links_at_time_30", test_links_at_time_30},
	    &(Test){"links_at_time_40", test_links_at_time_40},
	    &(Test){"links_at_time_60", test_links_at_time_60},
	    &(Test){"links_at_time_74", test_links_at_time_74},
	    &(Test){"links_at_time_75", test_links_at_time_75},
	    &(Test){"links_at_time_76", test_links_at_time_76},
	    &(Test){"links_at_time_80", test_links_at_time_80},
	    &(Test){"links_at_time_90", test_links_at_time_90},
	    &(Test){"links_at_time_100", test_links_at_time_100},
	    NULL,
	};

	return test("StreamGraph", tests);
}