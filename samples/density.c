#include "../StreamGraphAnalysis.h"

int main() {
	char* content = read_file("../data/S_external.txt");
	char* to_internal = InternalFormat_from_External_str(content);
	StreamGraph sg = StreamGraph_from_string(to_internal);
	Stream st = FullStreamGraph_from(&sg);

	double den = Stream_density(&st);
	printf("Density: %f\n", den);

	StreamGraph_destroy(sg);
	FullStreamGraph_destroy(st);
	free(content);
	free(to_internal);

	return 0;
}