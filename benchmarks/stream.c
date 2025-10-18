/**
 * @file benchmarks/stream.c
 * @brief Benchmarks for loading a StreamGraph
 */

#define SGA_INTERNAL
#include "../StreamGraphAnalysis.h"
#include "benchmark.h"

char* filename;
SGA_StreamGraph sg;

DONT_OPTIMISE void load() {
	sg = SGA_StreamGraph_from_file(filename);
	SGA_StreamGraph_destroy(sg);
}

DONT_OPTIMISE void init_events() {
	EventsTable_destroy(sg.events);
	init_events_table(&sg, KeyInstantsTable_total_nb_instants(&sg.key_instants));
}

int main() {
	filename = "data/tests/S.sga";
	benchmark(load, "S load", 5);
	sg = SGA_StreamGraph_from_file(filename);
	benchmark(init_events, "S init_events", 5);
	SGA_StreamGraph_destroy(sg);

	filename = "data/benchmarks/LS_90.sga";
	benchmark(load, "LS_90 load", 5);
	sg = SGA_StreamGraph_from_file(filename);
	benchmark(init_events, "LS_90 init_events", 5);
	SGA_StreamGraph_destroy(sg);

	filename = "data/benchmarks/primaryschool_3125_transformed.sga";
	benchmark(load, "primaryschool load", 5);
	sg = SGA_StreamGraph_from_file(filename);
	benchmark(init_events, "primaryschool init_events", 5);
	SGA_StreamGraph_destroy(sg);

	filename = "data/benchmarks/facebooklike_1_transformed.sga";
	benchmark(load, "facebooklike load", 5);
	sg = SGA_StreamGraph_from_file(filename);
	benchmark(init_events, "facebooklike init_events", 5);
	SGA_StreamGraph_destroy(sg);
}