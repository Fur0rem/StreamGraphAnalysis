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

int main() {
	StreamGraph sg;
	/*sg = StreamGraph_from_external("benchmarks/data/LS_90.ls");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links big", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);*/

	/*sg = StreamGraph_from_external("benchmarks/data/primaryschool_3125_transformed.ls");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links primaryschool", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);*/

	sg = StreamGraph_from_external("benchmarks/data/facebooklike_0_transformed.ls");
	stream = FullStreamGraph_from(&sg);
	benchmark(number_of_links, "number_of_links facebooklike", 100);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	return 0;
}