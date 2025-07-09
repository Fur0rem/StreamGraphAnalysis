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

bool test_nodes_at_time_90() {
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

bool test_nodes_at_time_100() {
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

bool test_other_links_present_at_4() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/delta_stream/expected_2delta_stream.sga");
	SGA_Stream stream  = SGA_FullStreamGraph_from(&sg);

	SGA_LinksIterator _links_present_at_4  = SGA_Stream_links_present_at_t(&stream, 4);
	SGA_LinkIdArrayList links_present_at_4 = SGA_collect_link_ids(_links_present_at_4);

	SGA_LinkIdArrayList expected_links = SGA_LinkIdArrayList_new();
	SGA_LinkIdArrayList_push(&expected_links, 1);

	bool result = EXPECT(SGA_LinkIdArrayList_equals(&links_present_at_4, &expected_links));

	SGA_LinkIdArrayList_destroy(links_present_at_4);
	SGA_LinkIdArrayList_destroy(expected_links);
	SGA_FullStreamGraph_destroy(stream);
	SGA_StreamGraph_destroy(sg);

	return result;
}

bool test_other_nodes_present_at_8() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/delta_stream/expected_2delta_stream.sga");
	SGA_Stream stream  = SGA_FullStreamGraph_from(&sg);

	SGA_NodesIterator _nodes_present_at_8  = SGA_Stream_nodes_present_at_t(&stream, 8);
	SGA_NodeIdArrayList nodes_present_at_8 = SGA_collect_node_ids(_nodes_present_at_8);
	SGA_NodeIdArrayList_sort_unstable(&nodes_present_at_8);

	SGA_NodeIdArrayList expected_nodes = SGA_NodeIdArrayList_new();
	SGA_NodeIdArrayList_push(&expected_nodes, 0);
	SGA_NodeIdArrayList_push(&expected_nodes, 1);
	SGA_NodeIdArrayList_push(&expected_nodes, 2);

	bool result = EXPECT(SGA_NodeIdArrayList_equals(&nodes_present_at_8, &expected_nodes));

	SGA_NodeIdArrayList_destroy(nodes_present_at_8);
	SGA_NodeIdArrayList_destroy(expected_nodes);
	SGA_FullStreamGraph_destroy(stream);
	SGA_StreamGraph_destroy(sg);

	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_nodes_at_time_0),		 TEST(test_nodes_at_time_5),	      TEST(test_nodes_at_time_10),
	    TEST(test_nodes_at_time_40),	 TEST(test_nodes_at_time_60),	      TEST(test_nodes_at_time_85),
	    TEST(test_nodes_at_time_90),	 TEST(test_nodes_at_time_100),	      TEST(test_links_at_time_25),
	    TEST(test_links_at_time_30),	 TEST(test_links_at_time_40),	      TEST(test_links_at_time_60),
	    TEST(test_links_at_time_74),	 TEST(test_links_at_time_75),	      TEST(test_links_at_time_76),
	    TEST(test_links_at_time_80),	 TEST(test_links_at_time_90),	      TEST(test_links_at_time_100),
	    TEST(test_other_links_present_at_4), TEST(test_other_nodes_present_at_8), NULL,
	};

	return test("StreamGraph", tests);
}