#include "../StreamGraphAnalysis.h"

int main() {
	char* content = read_file("../data/S_external.txt");
	char* to_internal = InternalFormat_from_External_str(content);
	StreamGraph sg = StreamGraph_from_string(to_internal);

	Stream first_half = SnapshotStream_from(&sg, Interval_from(0, 50));
	Stream second_half = SnapshotStream_from(&sg, Interval_from(50, 100));

	double number_of_nodes_first_half = Stream_number_of_nodes(&first_half);
	double number_of_nodes_second_half = Stream_number_of_nodes(&second_half);

	printf("Number of nodes in the first half: %f\n", number_of_nodes_first_half);
	printf("Number of nodes in the second half: %f\n", number_of_nodes_second_half);

	StreamGraph_destroy(sg);
	SnapshotStream_destroy(first_half);
	SnapshotStream_destroy(second_half);
	free(content);
	free(to_internal);

	return 0;
}