#include "../src/metrics.h"
#include "../src/stream.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "benchmark.h"

Stream stream;

DONT_OPTIMISE void number_of_links() {
	Stream_number_of_links(&stream);
	reset_cache(&stream);
}

char* file_name;

DONT_OPTIMISE void load() {
	StreamGraph s = StreamGraph_from_external(file_name);
	StreamGraph_destroy(s);
}

DONT_OPTIMISE void transitivity_ratio() {
	Stream_transitivity_ratio(&stream);
}

DONT_OPTIMISE void kcores() {
	for (size_t i = 0; i < 10; i++) {
		KCore kcore = Stream_k_cores(&stream, i);
		KCore_destroy(kcore);
	}
}

int main() {
	StreamGraph sg;
	/*sg = StreamGraph_from_external("data/LS_90.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links big", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg = StreamGraph_from_external("data/primaryschool_3125_transformed.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links primaryschool", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg = StreamGraph_from_external("data/facebooklike_1_transformed.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links facebooklike", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	file_name = "data/LS_90.txt";
	benchmark(load, "load big", 10);

	file_name = "data/primaryschool_3125_transformed.txt";
	benchmark(load, "load primaryschool", 10);

	file_name = "data/facebooklike_1_transformed.txt";
	benchmark(load, "load facebooklike", 10);*/

	sg	   = StreamGraph_from_external("data/Figure_8.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(transitivity_ratio, "transitivity_ratio Figure_8", 10000);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/LS_90.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(transitivity_ratio, "transitivity_ratio LS_90", 1);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/kcores_test.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(kcores, "kcores kcores_test", 1000);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/S_concat_L.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(kcores, "kcores S_concat_L", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/LS_90.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(kcores, "kcores LS_90", 1);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/primaryschool_3125_transformed.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(kcores, "kcores primaryschool", 1);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/facebooklike_1_transformed.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(kcores, "kcores facebooklike", 1);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	return 0;
}