#include "../StreamGraphAnalysis.h"

int main() {
	char* content	   = read_file("../data/S_external.txt");
	char* to_internal  = SGA_InternalFormat_from_External_str(content);
	SGA_StreamGraph sg = SGA_StreamGraph_from_string(to_internal);

	SGA_Stream first_half  = SGA_SnapshotStream_from(&sg, Interval_from(0, 50));
	SGA_Stream second_half = SGA_SnapshotStream_from(&sg, Interval_from(50, 100));

	double number_of_nodes_first_half  = SGA_Stream_number_of_nodes(&first_half);
	double number_of_nodes_second_half = SGA_Stream_number_of_nodes(&second_half);

	printf("Number of nodes in the first half: %f\n", number_of_nodes_first_half);
	printf("Number of nodes in the second half: %f\n", number_of_nodes_second_half);

	SGA_StreamGraph_destroy(sg);
	SGA_SnapshotStream_destroy(first_half);
	SGA_SnapshotStream_destroy(second_half);
	free(content);
	free(to_internal);

	return 0;
}