#include "../src/walks.h"
#include "../src/stream/chunk_stream.h"
#include "../src/stream/chunk_stream_small.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "../src/stream_graph.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool test_walk_a_c() {
	// // StreamGraph sg = StreamGraph_from_file("tests/test_data/internal_cliques.txt");
	// StreamGraph sg = StreamGraph_from_external("tests/test_data/L.txt");
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
	StreamGraph sg = StreamGraph_from_external("tests/test_data/L.txt");
	// char* str = StreamGraph_to_string(&sg);
	// printf("%s\n", str);
	// free(str);
	Stream st = FullStreamGraph_from(&sg);

	// char* str = StreamGraph_to_string(&sg);
	// printf("%s\n", str);
	// free(str);

	WalkInfo w = Stream_shortest_walk_from_to_at(&st, 0, 3, 0);
	// Walk w2 = Stream_fastest_shortest_walk(st, 0, 3, 0);
	char* str = WalkInfo_to_string(&w);
	printf("w %s\n", str);
	free(str);

	WalkInfo w2 = Stream_shortest_walk_from_to_at(&st, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str);
	free(str);

	StreamGraph sg2 = StreamGraph_from_external("tests/test_data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);
	printf("Loaded graph\n");

	WalkInfo w3 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 0);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str);
	free(str);
	WalkInfo w4 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w4);
	printf("w4 %s\n", str);
	free(str);

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
	// StreamGraph sg = StreamGraph_from_external("tests/test_data/test.txt");
	// FullStreamGraph fsg = (FullStreamGraph){
	// 	.underlying_stream_graph = &sg,
	// };
	// Stream st = (Stream){
	// 	.type = FULL_STREAM_GRAPH,
	// 	.stream = &fsg,
	// };
	StreamGraph sg = StreamGraph_from_external("tests/test_data/test.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	WalkInfoVector walks = optimal_walks_between_two_nodes(&st, 0, 3, Stream_shortest_walk_from_to_at);
	printf("Optimal walks between 0 and 3\n");
	for (size_t i = 0; i < walks.size; i++) {
		char* str = WalkInfo_to_string(&walks.array[i]);
		printf("%s\n", str);
		free(str);
	}

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);
	WalkInfoVector_destroy(walks);

	return true;
}

bool test_fastest_shortest() {
	StreamGraph sg = StreamGraph_from_external("tests/test_data/L.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	WalkInfo w = Stream_fastest_shortest_walk(&st, 0, 3, 0);
	char* str = WalkInfo_to_string(&w);
	printf("w %s\n", str);
	free(str);

	StreamGraph sg2 = StreamGraph_from_external("tests/test_data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);

	WalkInfo w2 = Stream_fastest_shortest_walk(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str);
	free(str);

	WalkInfo w3 = Stream_fastest_shortest_walk(&st2, 0, 3, 9);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str);
	free(str);

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
	StreamGraph sg = StreamGraph_from_external("tests/test_data/L.txt");
	Stream st = FullStreamGraph_from(&sg);
	printf("Loaded graph\n");

	WalkInfo w = Stream_fastest_walk(&st, 0, 3, 0);
	char* str = WalkInfo_to_string(&w);
	printf("w %s\n", str);
	free(str);

	WalkInfo w1 = Stream_fastest_shortest_walk(&st, 0, 3, 5);
	str = WalkInfo_to_string(&w1);
	printf("w1 %s\n", str);
	free(str);

	WalkInfo w12 = Stream_fastest_shortest_walk(&st, 0, 3, 4);
	str = WalkInfo_to_string(&w12);
	printf("w12 %s\n", str);
	free(str);

	StreamGraph sg2 = StreamGraph_from_external("tests/test_data/test.txt");
	Stream st2 = FullStreamGraph_from(&sg2);

	WalkInfo w2 = Stream_fastest_walk(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str);
	free(str);

	WalkInfo w3 = Stream_fastest_walk(&st2, 0, 3, 9);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str);
	free(str);

	StreamGraph_destroy(sg);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(st);
	FullStreamGraph_destroy(st2);
	WalkInfo_destroy(w);
	WalkInfo_destroy(w2);
	WalkInfo_destroy(w3);

	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"walk_a_c",			test_walk_a_c		 },
		&(Test){"walk_optimal",		test_walk_optimal	 },
		&(Test){"fastest_shortest", test_fastest_shortest},
		&(Test){"fastest",		   test_fastest		   },

		NULL
	};

	return test("Walks", tests);
}