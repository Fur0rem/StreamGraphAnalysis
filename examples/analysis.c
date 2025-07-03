/**
 * @file examples/analysis.c
 * @brief Example program for how to compute cliques, or walks
 */

#include "../StreamGraphAnalysis.h"
#include <stdlib.h>

int main() {
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/tests/S.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	printf("\n------ Stream analysis ------\n");

	SGA_CliqueArrayList cliques = SGA_Stream_maximal_cliques(&st);
	String str		    = SGA_CliqueArrayList_to_string(&cliques);
	printf("Maximal cliques: %s\n", str.data);
	String_destroy(str);

	printf("Robustness by length: %f\n", SGA_Stream_robustness_by_length(&st));
	printf("Robustness by duration: %f\n", SGA_Stream_robustness_by_duration(&st));

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}