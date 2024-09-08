#include "../src/analysis/kcores.h"
#include "../src/stream.h"
#include "../src/stream/full_stream_graph.h"
#include "../src/stream/link_stream.h"
#include "benchmark.h"

Stream stream;

DONT_OPTIMISE void kcores() {
	for (size_t i = 0; i < 10; i++) {
		KCore kcore = Stream_k_cores(&stream, i);
		KCore_destroy(kcore);
	}
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
	benchmark(kcores, "kcores S_concat_L", 10);
	stream = stream2;
	benchmark(kcores, "kcores LS_90", 10);
	// stream = stream3;
	// benchmark(kcores, "kcores primaryschool", 10);
	// stream = stream4;
	// benchmark(kcores, "kcores facebooklike", 10);

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