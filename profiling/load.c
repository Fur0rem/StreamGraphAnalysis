#include <stdio.h>
#define SGA_INTERNAL
#include "../StreamGraphAnalysis.h"

int main() {
	char* filename	   = "data/benchmarks/facebooklike_1_transformed.sga";
	SGA_StreamGraph sg = SGA_StreamGraph_from_file(filename);
	SGA_StreamGraph_destroy(sg);
}