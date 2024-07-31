
#include "src/metrics.h"
#include "src/stream.h"
#include "src/stream/chunk_stream.h"
#include "src/stream/chunk_stream_small.h"
#include "src/stream/full_stream_graph.h"
#include "src/stream/link_stream.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
	StreamGraph sg = StreamGraph_from_file("../benchmarks/data/Figure 8.txt"); /*primaryschool_3125_t*/
	Stream st = FullStreamGraph_from(&sg);
	double cov = Stream_coverage(&st);
	printf("Coverage: %f\n", cov);
	double num_nodes = Stream_number_of_nodes(&st);
	printf("Number of nodes: %f\n", num_nodes);
	double num_links = Stream_number_of_links(&st);
	printf("Number of links: %f\n", num_links);
	double node_dur = Stream_node_duration(&st);
	printf("Node duration: %f\n", node_dur);
	FullStreamGraph_destroy(st);
	StreamGraph_destroy(sg);
	return 0;
}