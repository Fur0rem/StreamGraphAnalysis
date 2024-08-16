#include "../src/walks.h"
#include "../src/stream/chunk_stream.h"
#include "../src/stream/chunk_stream_small.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool test_walk_a_c() {
	// // StreamGraph sg = StreamGraph_from_file("data/internal_cliques.txt");
	// StreamGraph sg = StreamGraph_from_external("data/L.txt");
	// /*FullStreamGraph fsg = (FullStreamGraph){
	// 	.underlying_stream_graph = &sg,
	// };
	// Stream st = (Stream){
	// 	.type = FULL_STREAM_GRAPH,
	// 	.stream = &fsg,
	// };*/
	// FullStreamGraph* fsg = malloc(sizeof(FullStreamGraph));
	// fsg->underlying_stream_graph = &sg;
	// printf("fsg->underlying_stream_graph : %p\n", fsg->underlying_stream_graph);
	// Stream* st = malloc(sizeof(Stream));
	// st->type = FULL_STREAM_GRAPH;
	// st->stream = fsg;
	// printf("st->stream : %p\n", st->stream);
	// printf("Loaded graph\n");
	StreamGraph sg = StreamGraph_from_external("data/L.txt");
	// String str = StreamGraph_to_string(&sg);
	// printf("%s\n", str.data);
	// String_destroy(str);
	Stream st = FullStreamGraph_from(&sg);

	// String str = StreamGraph_to_string(&sg);
	// printf("%s\n", str.data);
	// String_destroy(str);

	WalkInfo w = Stream_shortest_walk_from_to_at(&st, 0, 3, 0);
	// Walk w2 = Stream_fastest_shortest_walk(st, 0, 3, 0);
	String str = WalkInfo_to_string(&w);
	printf("w %s\n", str.data);
	String_destroy(str);

	WalkInfo w2 = Stream_shortest_walk_from_to_at(&st, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str.data);
	String_destroy(str);

	StreamGraph sg2 = StreamGraph_from_external("data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);
	printf("Loaded graph\n");

	WalkInfo w3 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 0);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str.data);
	String_destroy(str);
	WalkInfo w4 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w4);
	printf("w4 %s\n", str.data);
	String_destroy(str);

	StreamGraph_destroy(sg);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(st);
	FullStreamGraph_destroy(st2);

	WalkInfo_destroy(w);
	WalkInfo_destroy(w2);
	WalkInfo_destroy(w3);
	WalkInfo_destroy(w4);

	return true;
}

bool test_walk_optimal() {
	// StreamGraph sg = StreamGraph_from_external("data/test.txt");
	// FullStreamGraph fsg = (FullStreamGraph){
	// 	.underlying_stream_graph = &sg,
	// };
	// Stream st = (Stream){
	// 	.type = FULL_STREAM_GRAPH,
	// 	.stream = &fsg,
	// };
	StreamGraph sg = StreamGraph_from_external("data/test.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	WalkInfoVector walks = optimal_walks_between_two_nodes(&st, 0, 3, Stream_shortest_walk_from_to_at);
	printf("Optimal walks between 0 and 3\n");
	for (size_t i = 0; i < walks.size; i++) {
		String str = WalkInfo_to_string(&walks.array[i]);
		printf("%s\n", str.data);
		String_destroy(str);
	}

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);
	WalkInfoVector_destroy(walks);

	return true;
}

bool test_fastest_shortest() {
	StreamGraph sg = StreamGraph_from_external("data/L.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	WalkInfo w = Stream_fastest_shortest_walk(&st, 0, 3, 0);
	String str = WalkInfo_to_string(&w);
	printf("w %s\n", str.data);
	String_destroy(str);

	StreamGraph sg2 = StreamGraph_from_external("data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);

	WalkInfo w2 = Stream_fastest_shortest_walk(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str.data);
	String_destroy(str);

	WalkInfo w3 = Stream_fastest_shortest_walk(&st2, 0, 3, 9);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str.data);
	String_destroy(str);

	StreamGraph_destroy(sg);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(st);
	FullStreamGraph_destroy(st2);
	WalkInfo_destroy(w);
	WalkInfo_destroy(w2);
	WalkInfo_destroy(w3);

	return true;
}

bool test_fastest() {
	StreamGraph sg = StreamGraph_from_external("data/L.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	WalkInfo w = Stream_fastest_walk(&st, 0, 3, 0);
	String str = WalkInfo_to_string(&w);
	printf("w %s\n", str.data);
	String_destroy(str);

	WalkInfo w1 = Stream_fastest_shortest_walk(&st, 0, 3, 5);
	str = WalkInfo_to_string(&w1);
	printf("w1 %s\n", str.data);
	String_destroy(str);

	WalkInfo w12 = Stream_fastest_shortest_walk(&st, 0, 3, 4);
	str = WalkInfo_to_string(&w12);
	printf("w12 %s\n", str.data);
	String_destroy(str);

	StreamGraph sg2 = StreamGraph_from_external("data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);

	WalkInfo w2 = Stream_fastest_walk(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str.data);
	String_destroy(str);

	WalkInfo w3 = Stream_fastest_walk(&st2, 0, 3, 9);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str.data);
	String_destroy(str);

	StreamGraph_destroy(sg);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(st);
	FullStreamGraph_destroy(st2);
	WalkInfo_destroy(w);
	WalkInfo_destroy(w1);
	WalkInfo_destroy(w12);
	WalkInfo_destroy(w2);
	WalkInfo_destroy(w3);

	return true;
}

// NOTE: I realised the definition of betweenness is more complicated than I thought
// So ignore these tests for now
// bool test_betweenness_of_node_at_time() {
// 	StreamGraph sg = StreamGraph_from_external("data/fig_18_left.txt");
// 	Stream st = FullStreamGraph_from(&sg);
// 	printf("Loaded graph\n");

// 	double b_2_v = betweenness_of_node_at_time(&st, 1, 2);
// 	double b_3_v = betweenness_of_node_at_time(&st, 1, 3);
// 	bool one = EXPECT_F_APPROX_EQ(b_2_v, 8.0, 1e-6);
// 	bool two = EXPECT_F_APPROX_EQ(b_3_v, 0.0, 1e-6);
// 	return one && two;
// }

// bool test_betweenness_of_node_at_time_2() {
// 	StreamGraph sg = StreamGraph_from_external("data/fig_19_L4.txt");
// 	Stream st = FullStreamGraph_from(&sg);
// 	printf("Loaded graph\n");

// 	double b_35_v = betweenness_of_node_at_time(&st, 2, 3.5);
// 	double b_4_v = betweenness_of_node_at_time(&st, 2, 4);
// 	bool one = EXPECT_F_APPROX_EQ(b_35_v, 0.64, 1e-2);
// 	bool two = EXPECT_F_APPROX_EQ(b_4_v, 0.0, 1e-6);
// 	return one && two;
// }

bool test_robustness() {
	StreamGraph sg = StreamGraph_from_external("data/robustness_1.txt");
	Stream st = FullStreamGraph_from(&sg);

	double robustness = Stream_robustness_by_length(&st);
	bool success = EXPECT_F_APPROX_EQ(robustness, 1.0, 1e-6);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);
	return success;
}

int main() {
	Test* tests[] = {
		&(Test){"walk_a_c",			test_walk_a_c		 },
		&(Test){"walk_optimal",		test_walk_optimal	 },
		&(Test){"fastest_shortest", test_fastest_shortest},
		&(Test){"fastest",		   test_fastest		   },
		&(Test){"robustness",		  test_robustness		 },
 // &(Test){"betweenness_of_node_at_time",   test_betweenness_of_node_at_time  },
	// &(Test){"betweenness_of_node_at_time_2", test_betweenness_of_node_at_time_2},

		NULL
	};

	return test("Walks", tests);
}