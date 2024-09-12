#include "../src/analysis/walks.h"
#include "../src/stream.h"
#include "../src/stream_wrappers.h"
#include "benchmark.h"

Stream stream;

DONT_OPTIMISE void robustness_by_length() {
	Stream_robustness_by_length(&stream);
}

int main() {
	StreamGraph sg;
	sg	   = StreamGraph_from_external("data/S_external.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length Figure_8", 1000);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/S_concat_L.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length S_concat_L", 1000);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	sg	   = StreamGraph_from_external("data/LS_90.txt");
	stream = FullStreamGraph_from(&sg);
	benchmark(robustness_by_length, "robustness_by_length LS_90", 1);
	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(stream);

	return 0;
}