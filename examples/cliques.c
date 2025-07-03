/**
 * @file examples/cliques.c
 * @brief Example program for how to compute maximal cliques.
 */

#include "../StreamGraphAnalysis.h"

int main() {
	// Load the stream into memory
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/tests/S.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	// Compute the maximal cliques
	SGA_CliqueArrayList cliques = SGA_Stream_maximal_cliques(&st);

	// Print the maximal cliques
	String str = SGA_CliqueArrayList_to_string(&cliques);
	printf("Maximal cliques: %s\n", str.data);
	String_destroy(str);

	// Don't forget to release the memory used
	SGA_CliqueArrayList_destroy(cliques);

	// Cleanup
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}
