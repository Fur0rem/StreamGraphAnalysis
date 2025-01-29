/**
 * @file benchmarks/stream.c
 * @brief Benchmarks for loading a StreamGraph
 */

#define SGA_INTERNAL
#include "../StreamGraphAnalysis.h"
#include "benchmark.h"

String external_format;
String internal_format;
SGA_StreamGraph sg;

DONT_OPTIMISE void external_to_internal() {
	internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(&external_format);
	String_destroy(internal_format);
}

DONT_OPTIMISE void load_from_internal() {
	sg = SGA_StreamGraph_from_string(&internal_format);
	SGA_StreamGraph_destroy(sg);
}

DONT_OPTIMISE void init_events() {
	events_destroy(&sg);
	init_events_table(&sg);
}

int main() {

	char* filename	= "data/tests/S.sga";
	external_format = String_from_file(filename);
	benchmark(external_to_internal, "S external_to_internal", 5);
	internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(&external_format);
	benchmark(load_from_internal, "S load_from_internal", 5);
	sg = SGA_StreamGraph_from_string(&internal_format);
	benchmark(init_events, "S init_events", 5);
	String_destroy(external_format);
	String_destroy(internal_format);
	SGA_StreamGraph_destroy(sg);

	filename	= "data/benchmarks/LS_90.sga";
	external_format = String_from_file(filename);
	benchmark(external_to_internal, "LS_90 external_to_internal", 5);
	internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(&external_format);
	benchmark(load_from_internal, "LS_90 load_from_internal", 5);
	sg = SGA_StreamGraph_from_string(&internal_format);
	benchmark(init_events, "LS_90 init_events", 5);
	String_destroy(external_format);
	String_destroy(internal_format);
	SGA_StreamGraph_destroy(sg);

	filename	= "data/benchmarks/primaryschool_3125_transformed.sga";
	external_format = String_from_file(filename);
	benchmark(external_to_internal, "primaryschool external_to_internal", 5);
	internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(&external_format);
	benchmark(load_from_internal, "primaryschool load_from_internal", 5);
	sg = SGA_StreamGraph_from_string(&internal_format);
	benchmark(init_events, "primaryschool init_events", 5);
	String_destroy(external_format);
	String_destroy(internal_format);
	SGA_StreamGraph_destroy(sg);

	filename	= "data/benchmarks/facebooklike_1_transformed.sga";
	external_format = String_from_file(filename);
	benchmark(external_to_internal, "facebooklike external_to_internal", 5);
	internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(&external_format);
	benchmark(load_from_internal, "facebooklike load_from_internal", 5);
	sg = SGA_StreamGraph_from_string(&internal_format);
	benchmark(init_events, "facebooklike init_events", 5);
	String_destroy(external_format);
	String_destroy(internal_format);
	SGA_StreamGraph_destroy(sg);
}