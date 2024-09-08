#include "../src/stream.h"
#include "benchmark.h"

char* external_format;
char* internal_format;
StreamGraph sg;

DONT_OPTIMISE void external_to_internal() {
	internal_format = InternalFormat_from_External_str(external_format);
	free(internal_format);
}

DONT_OPTIMISE void load_from_internal() {
	sg = StreamGraph_from_string(internal_format);
	StreamGraph_destroy(sg);
}

DONT_OPTIMISE void init_events() {
	init_events_table(&sg);
	events_destroy(&sg);
}

int main() {
	char* filename	= "data/LS_90.txt";
	external_format = read_file(filename);
	benchmark(external_to_internal, "LS_90 external_to_internal", 5);
	internal_format = InternalFormat_from_External_str(external_format);
	benchmark(load_from_internal, "LS_90 load_from_internal", 5);
	sg = StreamGraph_from_string(internal_format);
	benchmark(init_events, "LS_90 init_events", 5);
	free(external_format);
	free(internal_format);
	StreamGraph_destroy(sg);

	filename		= "data/primaryschool_3125_transformed.txt";
	external_format = read_file(filename);
	benchmark(external_to_internal, "primaryschool external_to_internal", 5);
	internal_format = InternalFormat_from_External_str(external_format);
	benchmark(load_from_internal, "primaryschool load_from_internal", 5);
	sg = StreamGraph_from_string(internal_format);
	benchmark(init_events, "primaryschool init_events", 5);
	free(external_format);
	free(internal_format);
	StreamGraph_destroy(sg);

	filename		= "data/facebooklike_1_transformed.txt";
	external_format = read_file(filename);
	benchmark(external_to_internal, "facebooklike external_to_internal", 5);
	internal_format = InternalFormat_from_External_str(external_format);
	benchmark(load_from_internal, "facebooklike load_from_internal", 5);
	sg = StreamGraph_from_string(internal_format);
	benchmark(init_events, "facebooklike init_events", 5);
	free(external_format);
	free(internal_format);
	StreamGraph_destroy(sg);

	return 0;
}