#undef SGA_INTERNAL // TODO just don't define it
#include "../StreamGraphAnalysis.h"
#include <stdlib.h>

int main() {
	// SGA_StreamGraph sg = SGA_StreamGraph_from_file("../data/S_external.txt");
	// SGA_Stream st	   = SGA_FullStreamGraph_from(&sg);

	char* content	   = read_file("../data/S_external.txt");
	char* to_internal  = SGA_InternalFormat_from_External_str(content);
	SGA_StreamGraph sg = SGA_StreamGraph_from_string(to_internal);

	SGA_Stream st = SGA_FullStreamGraph_from(&sg);

	printf("\n------ Stream analysis ------\n");

	SGA_CliqueArrayList cliques = SGA_Stream_maximal_cliques(&st);
	String str		    = SGA_CliqueArrayList_to_string(&cliques);
	printf("Maximal cliques: %s\n", str.data);
	String_destroy(str);

	SGA_KCore kcore;
	for (size_t k = 1;; k++) {
		kcore = SGA_Stream_k_core(&st, k);
		if (kcore.nodes.length == 0) {
			SGA_KCore_destroy(kcore);
			break;
		}
		str = SGA_KCore_to_string(&kcore);
		printf("K-core for k=%zu: %s\n", k, str.data);
		String_destroy(str);
		SGA_KCore_destroy(kcore);
	}

	printf("Robustness by length: %f\n", Stream_robustness_by_length(&st));
	printf("Robustness by duration: %f\n", Stream_robustness_by_duration(&st));

	SGA_FullStreamGraph_destroy(st);
	SGA_StreamGraph_destroy(sg);
	exit(EXIT_SUCCESS);
}