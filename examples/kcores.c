/**
 * @file examples/kcores.c
 * @brief Example program on how to compute k-cores
 */

#include "../StreamGraphAnalysis.h"

int main() {
	// Load the stream into memory
	SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/tests/S.sga");
	SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	// Compute all k-cores
	for (size_t k = 1;; k++) {
		// Compute the k-core for the given k
		SGA_Cluster k_core = SGA_Stream_k_core(&st, k);

		// If it's empty, all further k-cores will be empty
		if (k_core.nodes.length == 0) {
			SGA_Cluster_destroy(k_core);
			printf("K-core for k=%zu is empty\n", k);
			break;
		}

		// Otherwise, print it
		String str = SGA_Cluster_to_string(&k_core);
		printf("K-core for k=%zu: %s\n", k, str.data);
		String_destroy(str);

		// Don't forget to release the memory used
		SGA_Cluster_destroy(k_core);
	}

	// Cleanup
	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}
/**
 * @file examples/kcores.c
 * @brief Example program for how to compute k-cores.
 */
