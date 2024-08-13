#include "../StreamGraphAnalysis.h"

int main() {
	char* content = read_file("../data/robustness_1.txt");
	char* to_internal = InternalFormat_from_External_str(content);
	StreamGraph sg = StreamGraph_from_string(to_internal);

	Stream st = FullStreamGraph_from(&sg);

	double robustness = Stream_robustness_by_length(&st);

	printf("robustness : %f", robustness);

	FullStreamGraph_destroy(st);
	StreamGraph_destroy(sg);
	free(content);
	free(to_internal);

	return 0;
}