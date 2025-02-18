#undef SGA_INTERNAL // TODO just don't define it
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

	SGA_Cluster kcore;
	for (size_t k = 1;; k++) {
		kcore = SGA_Stream_k_core(&st, k);
		if (kcore.nodes.length == 0) {
			SGA_Cluster_destroy(kcore);
			break;
		}
		str = SGA_Cluster_to_string(&kcore);
		printf("K-core for k=%zu: %s\n", k, str.data);
		String_destroy(str);
		SGA_Cluster_destroy(kcore);
	}

	printf("Robustness by length: %f\n", SGA_Stream_robustness_by_length(&st));
	printf("Robustness by duration: %f\n", SGA_Stream_robustness_by_duration(&st));

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}