/**
 * @file tests/weight.c
 * @brief Tests regarding weights
 */

#define SGA_INTERNAL

#include "../StreamGraphAnalysis.h"
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

int main() {
	Test* tests[] = {
	    TEST(test_constant_universally),
	    NULL,
	};

	return test("Weight", tests);
}