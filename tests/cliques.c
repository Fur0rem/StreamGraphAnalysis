#include "../src/analysis/cliques.h"
#include "../src/stream.h"
#include "../src/streams.h"
#include "../src/units.h"

#include "test.h"
#include <stdio.h>

bool test_maximal_cliques() {
	// StreamGraph sg = StreamGraph_from_file("data/internal_cliques.txt");
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/cliques.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	SGA_CliqueArrayList cliques = SGA_Stream_maximal_cliques(&st);

	String cliques_str = SGA_CliqueArrayList_to_string(&cliques);
	printf("%s\n", cliques_str.data);
	String_destroy(cliques_str);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	/*1 5 1 2
	2 4 0 1 2
	3 11 2 3
	6 10 1 2 3
	*/
	bool result =
	    EXPECT(cliques.length == 4) && EXPECT(cliques.array[0].time_start == 1) && EXPECT(cliques.array[0].time_end == 5) &&
	    EXPECT(cliques.array[0].nb_nodes == 2) && EXPECT(cliques.array[0].nodes[0] == 1) && EXPECT(cliques.array[0].nodes[1] == 2) &&
	    EXPECT(cliques.array[1].time_start == 2) && EXPECT(cliques.array[1].time_end == 4) && EXPECT(cliques.array[1].nb_nodes == 3) &&
	    EXPECT(cliques.array[1].nodes[0] == 0) && EXPECT(cliques.array[1].nodes[1] == 1) && EXPECT(cliques.array[1].nodes[2] == 2) &&
	    EXPECT(cliques.array[2].time_start == 3) && EXPECT(cliques.array[2].time_end == 11) && EXPECT(cliques.array[2].nb_nodes == 2) &&
	    EXPECT(cliques.array[2].nodes[0] == 2) && EXPECT(cliques.array[2].nodes[1] == 3) && EXPECT(cliques.array[3].time_start == 6) &&
	    EXPECT(cliques.array[3].time_end == 10) && EXPECT(cliques.array[3].nb_nodes == 3) && EXPECT(cliques.array[3].nodes[0] == 1) &&
	    EXPECT(cliques.array[3].nodes[1] == 2) && EXPECT(cliques.array[3].nodes[2] == 3);

	SGA_CliqueArrayList_destroy(cliques);
	// LinkArrayList_destroy(v);

	return result;
}

int main() {
	Test* tests[] = {
	    TEST(test_maximal_cliques),
	    NULL,
	};

	return test("Cliques", tests);
}