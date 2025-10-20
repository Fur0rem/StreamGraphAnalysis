/**
 * @file tests/weight.c
 * @brief Tests regarding weights
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
#include "../src/parsing/parse_weights.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool test_constant_universally() {
	SGA_StreamGraph sg    = SGA_StreamGraph_from_file("data/tests/S.sga");
	SGA_WeightFunc wf1    = SGA_WeightFunc_const_universally(5.0);
	SGA_WeightFunc wf2    = SGA_WeightFunc_const_universally(-2.0);
	SGA_W_StreamGraph wsg = SGA_StreamGraph_equip_weight(sg, wf1, wf2);
	bool okay	      = true;
	SGA_Interval lifespan = sg.lifespan;
	for (SGA_Time t = lifespan.start; t < lifespan.end; t++) {
		SGA_NodesIterator nodes = SGA_StreamGraph_nodes_set(&sg);
		SGA_FOR_EACH_NODE(node, nodes) {
			SGA_Weight w = SGA_W_StreamGraph_node_weight_at_t(&wsg, node, t);
			okay &= EXPECT_EQ(w, 5.0);
		}

		SGA_LinksIterator links = SGA_StreamGraph_links_set(&sg);
		SGA_FOR_EACH_LINK(link, links) {
			SGA_Weight w = SGA_W_StreamGraph_link_weight_at_t(&wsg, link, t);
			okay &= EXPECT_EQ(w, -2.0);
		}
	}

	SGA_StreamGraph_destroy(sg);

	return okay;
}

bool test_parse_lerp_nodes() {
	const char* str			= "5 ([9: -17.5, 41: 1e9, 89: 0] [100: -1])\n"
					  "[end]\n";
	SGA_ParsingCursor cursor	= SGA_ParsingCursor_begin(str, "test/weight.c:test_parse_lerp()");
	ParsedLerpWeightFunction parsed = SGA_parse_lerp_weight_function(&cursor, true);
	ParsedLerpWeight* weights	= parsed.weights_per_elem.array;
	bool result			= true;

	result &= EXPECT(parsed.weights_per_elem.length == 1);
	result &= EXPECT(weights->elem.node == 5);
	result &= EXPECT(weights->associated_weights.length == 2);

	__auto_type first_interval = weights->associated_weights.array[0];
	result &= EXPECT(first_interval.length == 3);

	result &= EXPECT_EQ(first_interval.array[0].time_instant, 9);
	result &= EXPECT_F_APPROX_EQ(first_interval.array[0].associated_weight, -17.5, 1e-6);

	result &= EXPECT_EQ(first_interval.array[1].time_instant, 41);
	result &= EXPECT_F_APPROX_EQ(first_interval.array[1].associated_weight, 1e9, 1e-6);

	result &= EXPECT_EQ(first_interval.array[2].time_instant, 89);
	result &= EXPECT_F_APPROX_EQ(first_interval.array[2].associated_weight, 0.0, 1e-6);

	__auto_type second_interval = weights->associated_weights.array[1];
	result &= EXPECT_EQ(second_interval.length, 1);

	result &= EXPECT_EQ(second_interval.array[0].time_instant, 100);
	result &= EXPECT_F_APPROX_EQ(second_interval.array[0].associated_weight, -1.0, 1e-6);

	return result;
}

bool test_parse_lerp_links() {
	const char* str			= "10 200 ([9: -17.5, 41: 1e9, 89: 0] [100: -1])\n"
					  "[end]\n";
	SGA_ParsingCursor cursor	= SGA_ParsingCursor_begin(str, "test/weight.c:test_parse_lerp()");
	ParsedLerpWeightFunction parsed = SGA_parse_lerp_weight_function(&cursor, false);
	ParsedLerpWeight* weights	= parsed.weights_per_elem.array;
	bool result			= true;

	result &= EXPECT(parsed.weights_per_elem.length == 1);
	result &= EXPECT(weights->elem.link.nodes[0] == 10);
	result &= EXPECT(weights->elem.link.nodes[1] == 200);
	result &= EXPECT(weights->associated_weights.length == 2);

	__auto_type first_interval = weights->associated_weights.array[0];
	result &= EXPECT(first_interval.length == 3);

	result &= EXPECT_EQ(first_interval.array[0].time_instant, 9);
	result &= EXPECT_F_APPROX_EQ(first_interval.array[0].associated_weight, -17.5, 1e-6);

	result &= EXPECT_EQ(first_interval.array[1].time_instant, 41);
	result &= EXPECT_F_APPROX_EQ(first_interval.array[1].associated_weight, 1e9, 1e-6);

	result &= EXPECT_EQ(first_interval.array[2].time_instant, 89);
	result &= EXPECT_F_APPROX_EQ(first_interval.array[2].associated_weight, 0.0, 1e-6);

	__auto_type second_interval = weights->associated_weights.array[1];
	result &= EXPECT_EQ(second_interval.length, 1);

	result &= EXPECT_EQ(second_interval.array[0].time_instant, 100);
	result &= EXPECT_F_APPROX_EQ(second_interval.array[0].associated_weight, -1.0, 1e-6);

	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_constant_universally),
	    TEST(test_parse_lerp_nodes),
	    TEST(test_parse_lerp_links),
	    NULL,
	};

	return test("Weight", tests);
}