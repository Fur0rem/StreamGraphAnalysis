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

	Walk w = Stream_shortest_walk_from_to_at(st, 0, 3, 0);
	Walk w2 = Stream_fastest_shortest_walk(st, 0, 3, 0);
	char* str = Walk_to_string(&w);
	printf("w %s\n", str);
	free(str);

	str = Walk_to_string(&w2);
	printf("w2 %s\n", str);

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

	Walk w = Stream_shortest_walk_from_to_at(&st, 0, 3, 0);
	Interval i = Walk_is_still_optimal_between(&w);
	char* str = Walk_to_string(&w);
	printf("w %s", str);
	free(str);
	str = Interval_to_string(&i);
	printf("optimal between %s\n\n", str);
	free(str);

	Walk wa = Stream_shortest_walk_from_to_at(&st, 0, 3, 1);
	Interval ia = Walk_is_still_optimal_between(&wa);
	str = Walk_to_string(&wa);
	printf("wa %s", str);
	free(str);
	str = Interval_to_string(&ia);
	printf("optimal between %s\n\n", str);
	free(str);

	Walk w2 = Stream_fastest_shortest_walk(&st, 0, 3, 0);
	Interval i2 = Walk_is_still_optimal_between(&w2);
	str = Walk_to_string(&w2);
	printf("w2 %s", str);
	free(str);
	str = Interval_to_string(&i2);
	printf("optimal between %s\n\n", str);
	free(str);

	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"walk_a_c",		test_walk_a_c	 },
		&(Test){"walk_optimal", test_walk_optimal},

		NULL
	};

	return test("Walks", tests);
}