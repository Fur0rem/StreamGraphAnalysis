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

	Path w = Stream_shortest_path_from_to_at(st, 0, 3, 0);
	Path w2 = Stream_fastest_shortest_path(st, 0, 3, 0);
	char* str = Path_to_string(&w);
	printf("w %s\n", str);
	free(str);
	str = Path_to_string(&w2);
	printf("w2 %s\n", str);
	free(str);
	return true;
}

int main() {
	Test* tests[] = {
		&(Test){"walk_a_c", test_walk_a_c},

		NULL
	};

	return test("Walks", tests);
}