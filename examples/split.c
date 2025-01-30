#include "../StreamGraphAnalysis.h"

int main() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/tests/S.sga");

	SGA_Stream first_half  = SGA_TimeFrameStream_from(&sg, SGA_Interval_from(0, 50));
	SGA_Stream second_half = SGA_TimeFrameStream_from(&sg, SGA_Interval_from(50, 100));

	double number_of_nodes_first_half  = SGA_Stream_number_of_nodes(&first_half);
	double number_of_nodes_second_half = SGA_Stream_number_of_nodes(&second_half);

	printf("Number of nodes in the first half: %f\n", number_of_nodes_first_half);
	printf("Number of nodes in the second half: %f\n", number_of_nodes_second_half);

	SGA_StreamGraph_destroy(sg);
	SGA_TimeFrameStream_destroy(first_half);
	SGA_TimeFrameStream_destroy(second_half);

	return 0;
}