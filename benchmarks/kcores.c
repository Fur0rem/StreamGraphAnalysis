#include "../src/analysis/kcores.h"
#include "../src/stream.h"
#include "../src/streams.h"
#include "benchmark.h"

SGA_Stream stream;

DONT_OPTIMISE void kcores() {
	for (size_t i = 0; i < 10; i++) {
		SGA_KCore kcore = SGA_Stream_k_core(&stream, i);
		SGA_KCore_destroy(kcore);
	}
}

int main() {

	SGA_StreamGraph sg1 = SGA_StreamGraph_from_external("data/S_concat_L.txt");
	SGA_Stream stream1  = SGA_FullStreamGraph_from(&sg1);

	SGA_StreamGraph sg2 = SGA_StreamGraph_from_external("data/LS_90.txt");
	SGA_Stream stream2  = SGA_FullStreamGraph_from(&sg2);

	SGA_StreamGraph sg3 = SGA_StreamGraph_from_external("data/primaryschool_3125_transformed.txt");
	SGA_Stream stream3  = SGA_FullStreamGraph_from(&sg3);

	SGA_StreamGraph sg4 = SGA_StreamGraph_from_external("data/facebooklike_1_transformed.txt");
	SGA_Stream stream4  = SGA_FullStreamGraph_from(&sg4);

	stream = stream1;
	benchmark(kcores, "kcores S_concat_L", 10);
	stream = stream2;
	benchmark(kcores, "kcores LS_90", 10);
	// stream = stream3;
	// benchmark(kcores, "kcores primaryschool", 10);
	// stream = stream4;
	// benchmark(kcores, "kcores facebooklike", 10);

	SGA_StreamGraph_destroy(sg1);
	SGA_FullStreamGraph_destroy(stream1);
	SGA_StreamGraph_destroy(sg2);
	SGA_FullStreamGraph_destroy(stream2);
	SGA_StreamGraph_destroy(sg3);
	SGA_FullStreamGraph_destroy(stream3);
	SGA_StreamGraph_destroy(sg4);
	SGA_FullStreamGraph_destroy(stream4);

	return 0;
}