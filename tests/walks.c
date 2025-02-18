#include "../StreamGraphAnalysis.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool test_walk_a_c() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	bool result = true;

	SGA_OptionalWalk w1 = SGA_shortest_walk(&st, 0, 3, 0);
	SGA_Walk w1_walk    = SGA_OptionalWalk_unwrap_checked(w1);
	result &= EXPECT(SGA_Walk_goes_through(&w1_walk, st, 2, 0, 1, 3));
	// printf("w1 %s\n", SGA_Walk_to_string(&w1_walk).data);
	SGA_OptionalWalk_destroy(w1);

	SGA_OptionalWalk w2 = SGA_shortest_walk(&st, 0, 3, 5);
	SGA_Walk w2_walk    = SGA_OptionalWalk_unwrap_checked(w2);
	result &= EXPECT(SGA_Walk_goes_through(&w2_walk, st, 2, 0, 1, 3));
	// printf("w2 %s\n", SGA_Walk_to_string(&w2_walk).data);
	SGA_OptionalWalk_destroy(w2);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/walks.sga");
	SGA_Stream st2	    = SGA_FullStreamGraph_from(&sg2);

	SGA_OptionalWalk w3 = SGA_shortest_walk(&st2, 0, 3, 0);
	SGA_Walk w3_walk    = SGA_OptionalWalk_unwrap_checked(w3);
	result &= EXPECT(SGA_Walk_goes_through(&w3_walk, st2, 2, 0, 1, 3));
	// printf("w3 %s\n", SGA_Walk_to_string(&w3_walk).data);
	SGA_OptionalWalk_destroy(w3);

	SGA_OptionalWalk w4 = SGA_shortest_walk(&st2, 0, 3, 5);
	SGA_Walk w4_walk    = SGA_OptionalWalk_unwrap_checked(w4);
	result &= EXPECT(SGA_Walk_goes_through(&w4_walk, st2, 3, 0, 1, 2, 3));
	// printf("w4 %s\n", SGA_Walk_to_string(&w4_walk).data);
	SGA_OptionalWalk_destroy(w4);

	SGA_StreamGraph_destroy(sg);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(st);
	SGA_FullStreamGraph_destroy(st2);

	return result;
}

bool test_walk_optimal() {
	// StreamGraph sg = StreamGraph_from_external("data/tests/walks.sga");
	// FullStreamGraph fsg = (FullStreamGraph){
	// 	.underlying_stream_graph = &sg,
	// };
	// SGA_Stream st = (SGA_Stream){
	// 	.type = FULL_STREAM_GRAPH,
	// 	.stream = &fsg,
	// };
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/walks.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	SGA_OptionalWalkArrayList walks = SGA_optimal_walks_between_two_nodes(&st, 0, 3, SGA_shortest_walk);
	printf("Optimal walks between 0 and 3\n");
	for (size_t i = 0; i < walks.length; i++) {
		String str = SGA_OptionalWalk_to_string(&walks.array[i]);
		printf("%s\n", str.data);
		String_destroy(str);
	}

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	SGA_OptionalWalkArrayList_destroy(walks);

	return true;
}

bool test_fastest_shortest() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	SGA_OptionalWalk w = Stream_fastest_shortest_walk(&st, 0, 3, 0);
	String str	   = SGA_OptionalWalk_to_string(&w);
	printf("w %s\n", str.data);
	String_destroy(str);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/walks.sga");
	SGA_Stream st2	    = SGA_FullStreamGraph_from(&sg2);

	SGA_OptionalWalk w2 = Stream_fastest_shortest_walk(&st2, 0, 3, 5);
	str		    = SGA_OptionalWalk_to_string(&w2);
	printf("w2 %s\n", str.data);
	String_destroy(str);

	SGA_OptionalWalk w3 = Stream_fastest_shortest_walk(&st2, 0, 3, 9);
	str		    = SGA_OptionalWalk_to_string(&w3);
	printf("w3 %s\n", str.data);
	String_destroy(str);

	SGA_StreamGraph_destroy(sg);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(st);
	SGA_FullStreamGraph_destroy(st2);
	SGA_OptionalWalk_destroy(w);
	SGA_OptionalWalk_destroy(w2);
	SGA_OptionalWalk_destroy(w3);

	return true;
}

bool test_fastest() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/L.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	bool result = true;

	SGA_OptionalWalk w1 = SGA_fastest_walk(&st, 0, 3, 0);
	SGA_Walk w1_walk    = SGA_OptionalWalk_unwrap_checked(w1);
	result &= EXPECT(SGA_Walk_goes_through(&w1_walk, st, 2, 0, 2, 3));
	result &= EXPECT_EQ(SGA_Walk_arrives_at(&w1_walk), 6);
	// printf("w1 %s\n", SGA_Walk_to_string(&w1_walk).data);
	SGA_OptionalWalk_destroy(w1);

	SGA_OptionalWalk w2 = SGA_fastest_walk(&st, 0, 3, 4);
	SGA_Walk w2_walk    = SGA_OptionalWalk_unwrap_checked(w2);
	result &= EXPECT(SGA_Walk_goes_through(&w2_walk, st, 2, 0, 2, 3));
	result &= EXPECT_EQ(SGA_Walk_arrives_at(&w2_walk), 6);
	// printf("w2 %s\n", SGA_Walk_to_string(&w2_walk).data);
	SGA_OptionalWalk_destroy(w2);

	SGA_OptionalWalk w3 = SGA_fastest_walk(&st, 0, 3, 5);
	SGA_Walk w3_walk    = SGA_OptionalWalk_unwrap_checked(w3);
	result &= EXPECT(SGA_Walk_goes_through(&w3_walk, st, 3, 0, 1, 2, 3));
	result &= EXPECT_EQ(SGA_Walk_arrives_at(&w3_walk), 6);
	// printf("w3 %s\n", SGA_Walk_to_string(&w3_walk).data);
	SGA_OptionalWalk_destroy(w3);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_file("data/tests/walks.sga");
	SGA_Stream st2	    = SGA_FullStreamGraph_from(&sg2);

	SGA_OptionalWalk w4 = SGA_fastest_walk(&st2, 0, 3, 5);
	SGA_Walk w4_walk    = SGA_OptionalWalk_unwrap_checked(w4);
	result &= EXPECT(SGA_Walk_goes_through(&w4_walk, st2, 3, 0, 1, 2, 3));
	result &= EXPECT_EQ(SGA_Walk_arrives_at(&w4_walk), 8);
	SGA_OptionalWalk_destroy(w4);

	SGA_OptionalWalk w5 = SGA_fastest_walk(&st2, 0, 3, 9);
	result &= EXPECT(w5.type == NO_WALK);
	result &= EXPECT(w5.result.no_walk_reason.type == UNREACHABLE);
	result &= EXPECT_EQ(w5.result.no_walk_reason.reason.unreachable_after, 9);
	SGA_OptionalWalk_destroy(w5);

	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(st2);

	return result;
}

bool test_walk_node_not_present() {
	bool correct	   = true;
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/gap_in_node.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	SGA_OptionalWalk wi = SGA_shortest_walk(&st, 0, 3, 0);
	SGA_Walk w	    = SGA_OptionalWalk_unwrap_checked(wi);
	correct &= EXPECT(SGA_Walk_goes_through(&w, st, 3, 0, 1, 2, 3));
	SGA_OptionalWalk_destroy(wi);

	SGA_StreamGraph_destroy(sg);
	SGA_FullStreamGraph_destroy(st);
	return correct;
}

// NOTE: I realised the definition of betweenness is more complicated than I
// thought So ignore these tests for now bool test_betweenness_of_node_at_time()
// { 	StreamGraph sg = StreamGraph_from_external("data/fig_18_left.txt");
// SGA_Stream st = FullStreamGraph_from(&sg); 	printf("Loaded graph\n");

// 	double b_2_v = betweenness_of_node_at_time(&st, 1, 2);
// 	double b_3_v = betweenness_of_node_at_time(&st, 1, 3);
// 	bool one = EXPECT_F_APPROX_EQ(b_2_v, 8.0, 1e-6);
// 	bool two = EXPECT_F_APPROX_EQ(b_3_v, 0.0, 1e-6);
// 	return one && two;
// }

// bool test_betweenness_of_node_at_time_2() {
// 	StreamGraph sg = StreamGraph_from_external("data/fig_19_L4.txt");
// 	SGA_Stream st = FullStreamGraph_from(&sg);
// 	printf("Loaded graph\n");

// 	double b_35_v = betweenness_of_node_at_time(&st, 2, 3.5);
// 	double b_4_v = betweenness_of_node_at_time(&st, 2, 4);
// 	bool one = EXPECT_F_APPROX_EQ(b_35_v, 0.64, 1e-2);
// 	bool two = EXPECT_F_APPROX_EQ(b_4_v, 0.0, 1e-6);
// 	return one && two;
// }

bool test_robustness_length_of_1() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/robustness_of_1.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	double robustness = SGA_Stream_robustness_by_length(&st);
	bool success	  = EXPECT_F_APPROX_EQ(robustness, 1.0, 1e-6);

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return success;
}

bool test_robustness_duration_of_1() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("data/tests/robustness_of_1.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	double robustness = SGA_Stream_robustness_by_duration(&st);
	bool success	  = EXPECT_F_APPROX_EQ(robustness, 1.0, 1e-6);

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	return success;
}

int main() {
	Test* tests[] = {
	    TEST(test_walk_a_c),
	    TEST(test_walk_optimal),
	    TEST(test_fastest_shortest),
	    TEST(test_fastest),
	    TEST(test_robustness_length_of_1),
	    TEST(test_robustness_duration_of_1),
	    TEST(test_walk_node_not_present),
	    NULL,
	};

	return test("SGA_Walks", tests);
}