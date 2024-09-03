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
	StreamGraph sg = StreamGraph_from_external("data/L.txt");
	Stream st = FullStreamGraph_from(&sg);

	bool result = true;

	WalkInfo w1 = Stream_shortest_walk_from_to_at(&st, 0, 3, 0);
	Walk w1_walk = WalkInfo_unwrap_checked(w1);
	result &= EXPECT(Walk_goes_through(&w1_walk, st, 2, 0, 1, 3));
	WalkInfo_destroy(w1);

	WalkInfo w2 = Stream_shortest_walk_from_to_at(&st, 0, 3, 5);
	Walk w2_walk = WalkInfo_unwrap_checked(w2);
	result &= EXPECT(Walk_goes_through(&w2_walk, st, 2, 0, 1, 3));
	WalkInfo_destroy(w2);

	StreamGraph sg2 = StreamGraph_from_external("data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);

	WalkInfo w3 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 0);
	Walk w3_walk = WalkInfo_unwrap_checked(w3);
	result &= EXPECT(Walk_goes_through(&w3_walk, st2, 2, 0, 1, 3));
	WalkInfo_destroy(w3);

	WalkInfo w4 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 5);
	Walk w4_walk = WalkInfo_unwrap_checked(w4);
	result &= EXPECT(Walk_goes_through(&w4_walk, st2, 3, 0, 1, 2, 3));
	WalkInfo_destroy(w4);

	StreamGraph_destroy(sg);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(st);
	FullStreamGraph_destroy(st2);

	return result;
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
	// printf("Loaded graph\n");

	// WalkInfo w = Stream_fastest_walk(&st, 0, 3, 0);
	// String str = WalkInfo_to_string(&w);
	// printf("w %s\n", str.data);
	// String_destroy(str);

	// WalkInfo w1 = Stream_fastest_shortest_walk(&st, 0, 3, 5);
	// str = WalkInfo_to_string(&w1);
	// printf("w1 %s\n", str.data);
	// String_destroy(str);

	// WalkInfo w12 = Stream_fastest_shortest_walk(&st, 0, 3, 4);
	// str = WalkInfo_to_string(&w12);
	// printf("w12 %s\n", str.data);
	// String_destroy(str);

	// StreamGraph sg2 = StreamGraph_from_external("data/test.txt");
	// Stream st2 = FullStreamGraph_from(&sg2);

	// WalkInfo w2 = Stream_fastest_walk(&st2, 0, 3, 5);
	// str = WalkInfo_to_string(&w2);
	// printf("w2 %s\n", str.data);
	// String_destroy(str);

	// WalkInfo w3 = Stream_fastest_walk(&st2, 0, 3, 9);
	// str = WalkInfo_to_string(&w3);
	// printf("w3 %s\n", str.data);
	// String_destroy(str);

	// StreamGraph_destroy(sg);
	// StreamGraph_destroy(sg2);
	// FullStreamGraph_destroy(st);
	// FullStreamGraph_destroy(st2);
	// WalkInfo_destroy(w);
	// WalkInfo_destroy(w1);
	// WalkInfo_destroy(w12);
	// WalkInfo_destroy(w2);
	// WalkInfo_destroy(w3);

	bool result = true;

	WalkInfo w1 = Stream_fastest_walk(&st, 0, 3, 0);
	Walk w1_walk = WalkInfo_unwrap_checked(w1);
	result &= EXPECT(Walk_goes_through(&w1_walk, st, 2, 0, 2, 3));
	result &= EXPECT_EQ(Walk_arrives_at(&w1_walk), 6);
	WalkInfo_destroy(w1);

	WalkInfo w2 = Stream_fastest_walk(&st, 0, 3, 4);
	Walk w2_walk = WalkInfo_unwrap_checked(w2);
	result &= EXPECT(Walk_goes_through(&w2_walk, st, 2, 0, 2, 3));
	result &= EXPECT_EQ(Walk_arrives_at(&w2_walk), 6);
	WalkInfo_destroy(w2);

	WalkInfo w3 = Stream_fastest_walk(&st, 0, 3, 5);
	Walk w3_walk = WalkInfo_unwrap_checked(w3);
	result &= EXPECT(Walk_goes_through(&w3_walk, st, 3, 0, 1, 2, 3));
	result &= EXPECT_EQ(Walk_arrives_at(&w3_walk), 6);
	WalkInfo_destroy(w3);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);

	StreamGraph sg2 = StreamGraph_from_external("data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);

	WalkInfo w4 = Stream_fastest_walk(&st2, 0, 3, 5);
	Walk w4_walk = WalkInfo_unwrap_checked(w4);
	result &= EXPECT(Walk_goes_through(&w4_walk, st2, 3, 0, 1, 2, 3));
	result &= EXPECT_EQ(Walk_arrives_at(&w4_walk), 8);
	WalkInfo_destroy(w4);

	WalkInfo w5 = Stream_fastest_walk(&st2, 0, 3, 9);
	result &= EXPECT(w5.type == NO_WALK);
	result &= EXPECT(w5.walk_or_reason.no_walk_reason.type == IMPOSSIBLE_TO_REACH);
	result &= EXPECT_EQ(w5.walk_or_reason.no_walk_reason.reason.impossible_to_reach_after, 9);
	WalkInfo_destroy(w5);

	return result;
}

bool test_walk() {
	StreamGraph sg = StreamGraph_from_external("data/robustness_1.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");
	WalkInfo w = Stream_shortest_walk_from_to_at(&st, 1, 2, 0);
	String str = WalkInfo_to_string(&w);
	printf("Walk from 1 to 2 at 0 : %s\n", str.data);
	String_destroy(str);

	return true;
}

bool test_walk_node_not_present() {
	StreamGraph sg = StreamGraph_from_external("data/bridge.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");
	WalkInfo w = Stream_shortest_walk_from_to_at(&st, 0, 3, 0);
	String str = WalkInfo_to_string(&w);
	printf("Walk from 0 to 3 at 0 : %s\n", str.data);

	StreamFunctions fns = FullStreamGraph_stream_functions;

	bool correct = true;
	correct &= EXPECT(w.type == WALK);
	Walk w1 = w.walk_or_reason.walk;
	correct &= EXPECT_EQ(w1.steps.size, 3);

	LinkId link_0_1 = fns.links_between_nodes(st.stream_data, 0, 1);
	LinkId link_1_2 = fns.links_between_nodes(st.stream_data, 1, 2);
	LinkId link_2_3 = fns.links_between_nodes(st.stream_data, 2, 3);

	correct &= EXPECT(w1.steps.array[0].link == link_0_1);
	correct &= EXPECT(w1.steps.array[1].link == link_1_2);
	correct &= EXPECT(w1.steps.array[2].link == link_2_3);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);
	WalkInfo_destroy(w);
	return correct;
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

	FullStreamGraph_destroy(st);
	StreamGraph_destroy(sg);
	return success;
}

int main() {
	Test* tests[] = {
		TEST(test_walk_a_c),   TEST(test_walk_optimal), TEST(test_fastest_shortest),	  TEST(test_fastest),
		TEST(test_robustness), TEST(test_walk),			TEST(test_walk_node_not_present), NULL,
	};

	return test("Walks", tests);
}