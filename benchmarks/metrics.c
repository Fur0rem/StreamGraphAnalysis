#include "../src/metrics.h"
#include "../src/stream.h"
#include "../src/stream_wrappers.h"
#include "benchmark.h"

Stream stream;

DONT_OPTIMISE void number_of_links() {
	Stream_number_of_links(&stream);
	reset_cache(&stream);
}

DONT_OPTIMISE void coverage() {
	Stream_coverage(&stream);
	reset_cache(&stream);
}

DONT_OPTIMISE void uniformity() {
	Stream_node_duration(&stream);
	reset_cache(&stream);
}

char* file_name;

DONT_OPTIMISE void transitivity_ratio() {
	Stream_transitivity_ratio(&stream);
}

// // TODO: move to analytics or smth
// DONT_OPTIMISE void kcores() {
// 	for (size_t i = 0; i < 10; i++) {
// 		KCore kcore = Stream_k_cores(&stream, i);
// 		KCore_destroy(kcore);
// 	}
// }

DONT_OPTIMISE void clustering_coeff() {
	Stream_node_clustering_coeff(&stream);
}

int main() {
	StreamGraph sg1 = StreamGraph_from_external("data/S_concat_L.txt");
	Stream stream1	= FullStreamGraph_from(&sg1);

	StreamGraph sg2 = StreamGraph_from_external("data/LS_90.txt");
	Stream stream2	= FullStreamGraph_from(&sg2);

	StreamGraph sg3 = StreamGraph_from_external("data/primaryschool_3125_transformed.txt");
	Stream stream3	= FullStreamGraph_from(&sg3);

	StreamGraph sg4 = StreamGraph_from_external("data/facebooklike_1_transformed.txt");
	Stream stream4	= FullStreamGraph_from(&sg4);

	stream = stream1;
	benchmark(number_of_links, "number_of_links S_concat_L", 10);
	stream = stream2;
	benchmark(number_of_links, "number_of_links LS_90", 10);
	stream = stream3;
	benchmark(number_of_links, "number_of_links primaryschool", 10);
	stream = stream4;
	benchmark(number_of_links, "number_of_links facebooklike", 10);
	printf("\n");

	stream = stream1;
	benchmark(coverage, "coverage S_concat_L", 10);
	stream = stream2;
	benchmark(coverage, "coverage LS_90", 10);
	stream = stream3;
	benchmark(coverage, "coverage primaryschool", 10);
	stream = stream4;
	benchmark(coverage, "coverage facebooklike", 10);
	printf("\n");

	stream = stream1;
	benchmark(uniformity, "uniformity S_concat_L", 10);
	stream = stream2;
	benchmark(uniformity, "uniformity LS_90", 10);
	stream = stream3;
	benchmark(uniformity, "uniformity primaryschool", 10);
	stream = stream4;
	benchmark(uniformity, "uniformity facebooklike", 10);
	printf("\n");

	stream = stream1;
	benchmark(transitivity_ratio, "transitivity_ratio S_concat_L", 10);
	stream = stream2;
	benchmark(transitivity_ratio, "transitivity_ratio LS_90", 10);
	stream = stream3;
	benchmark(transitivity_ratio, "transitivity_ratio primaryschool", 10);
	stream = stream4;
	benchmark(transitivity_ratio, "transitivity_ratio facebooklike", 10);
	printf("\n");

	stream = stream1;
	benchmark(clustering_coeff, "clustering_coeff S_concat_L", 10);
	stream = stream2;
	benchmark(clustering_coeff, "clustering_coeff LS_90", 10);
	stream = stream3;
	benchmark(clustering_coeff, "clustering_coeff primaryschool", 10);
	stream = stream4;
	benchmark(clustering_coeff, "clustering_coeff facebooklike", 10);
	printf("\n");

	StreamGraph_destroy(sg1);
	FullStreamGraph_destroy(stream1);
	StreamGraph_destroy(sg2);
	FullStreamGraph_destroy(stream2);
	StreamGraph_destroy(sg3);
	FullStreamGraph_destroy(stream3);
	StreamGraph_destroy(sg4);
	FullStreamGraph_destroy(stream4);

	return 0;
}