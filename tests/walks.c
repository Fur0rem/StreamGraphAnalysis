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
	// StreamGraph sg = StreamGraph_from_file("tests/test_data/internal_cliques.txt");
	StreamGraph sg = StreamGraph_from_external("tests/test_data/L.txt");
	/*FullStreamGraph fsg = (FullStreamGraph){
		.underlying_stream_graph = &sg,
	};
	Stream st = (Stream){
		.type = FULL_STREAM_GRAPH,
		.stream = &fsg,
	};*/
	FullStreamGraph* fsg = malloc(sizeof(FullStreamGraph));
	fsg->underlying_stream_graph = &sg;
	printf("fsg->underlying_stream_graph : %p\n", fsg->underlying_stream_graph);
	Stream* st = malloc(sizeof(Stream));
	st->type = FULL_STREAM_GRAPH;
	st->stream = fsg;
	printf("st->stream : %p\n", st->stream);
	printf("Loaded graph\n");

	// char* str = StreamGraph_to_string(&sg);
	// printf("%s\n", str);
	// free(str);

	WalkInfo w = Stream_shortest_walk_from_to_at(st, 0, 3, 0);
	// Walk w2 = Stream_fastest_shortest_walk(st, 0, 3, 0);
	char* str = WalkInfo_to_string(&w);
	printf("w %s\n", str);
	free(str);

	WalkInfo w2 = Stream_shortest_walk_from_to_at(st, 0, 3, 5);
	str = WalkInfo_to_string(&w2);
	printf("w2 %s\n", str);
	free(str);

	// str = Walk_to_string(&w2);
	// printf("w2 %s\n", str);

	// free(str);

	StreamGraph sg2 = StreamGraph_from_external("tests/test_data/test.txt");
	FullStreamGraph fsg2 = (FullStreamGraph){
		.underlying_stream_graph = &sg2,
	};
	Stream st2 = (Stream){
		.type = FULL_STREAM_GRAPH,
		.stream = &fsg2,
	};
	printf("Loaded graph\n");

	WalkInfo w3 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 0);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str);
	free(str);
	w3 = Stream_shortest_walk_from_to_at(&st2, 0, 3, 5);
	str = WalkInfo_to_string(&w3);
	printf("w3 %s\n", str);
	free(str);

	return true;
}

bool test_walk_optimal() {
	StreamGraph sg = StreamGraph_from_external("tests/test_data/L.txt");
	FullStreamGraph fsg = (FullStreamGraph){
		.underlying_stream_graph = &sg,
	};
	Stream st = (Stream){
		.type = FULL_STREAM_GRAPH,
		.stream = &fsg,
	};
	printf("Loaded graph\n");

	WalkInfoVector walks = optimal_walks_between_two_nodes(&st, 0, 3, Stream_shortest_walk_from_to_at);
	printf("Optimal walks between 0 and 3\n");
	for (size_t i = 0; i < walks.size; i++) {
		char* str = WalkInfo_to_string(&walks.array[i]);
		printf("%s\n", str);
		free(str);
	}

	return true;
}

bool test_optimals() {
	/*StreamGraph sg = StreamGraph_from_external("tests/test_data/L_10.txt");
	FullStreamGraph fsg = (FullStreamGraph){
		.underlying_stream_graph = &sg,
	};
	Stream st = (Stream){
		.type = FULL_STREAM_GRAPH,
		.stream = &fsg,
	};
	printf("Loaded graph\n");

	for (size_t n = 1; n <= 3; n++) {
		WalkOptimalVector optimals = optimals_between_two_nodes(&st, 0, n);
		printf("Optimals between 0 and %zu\n", n);
		for (size_t i = 0; i < optimals.size; i++) {
			char* str = WalkOptimal_to_string(&optimals.array[i]);
			printf("%s\n", str);
			free(str);
		}
	}

	StreamGraph s2 = StreamGraph_from_external("tests/test_data/L.txt");
	FullStreamGraph fsg2 = (FullStreamGraph){
		.underlying_stream_graph = &s2,
	};
	Stream st2 = (Stream){
		.type = FULL_STREAM_GRAPH,
		.stream = &fsg2,
	};
	printf("Loaded graph\n");

	for (size_t n = 1; n <= 3; n++) {
		WalkOptimalVector optimals = optimals_between_two_nodes(&st2, 0, n);
		printf("Optimals between 0 and %zu\n", n);
		for (size_t i = 0; i < optimals.size; i++) {
			char* str = WalkOptimal_to_string(&optimals.array[i]);
			printf("%s\n", str);
			free(str);
		}
	}*/

	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"walk_a_c",		test_walk_a_c	 },
		&(Test){"walk_optimal", test_walk_optimal},
		&(Test){"optimals",		test_optimals	 },

		NULL
	};

	return test("Walks", tests);
}