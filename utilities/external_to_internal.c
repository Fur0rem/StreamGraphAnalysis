/**
 * @file utilities/external_to_internal.c
 * @brief Converts an external format StreamGraph to an internal format StreamGraph.
 */

#include <stdlib.h>
#define SGA_INTERNAL
#include "../StreamGraphAnalysis.h"

// First arg: path to the file to load (external format)
// Second arg: path to the file to save (internal format)

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <external_format_file> <internal_format_file>\n", argv[0]);
		fprintf(stderr,
			"This program converts a StreamGraph from external format to internal format, it reads the StreamGraph from the "
			"file <external_format_file> and writes it to the file <internal_format_file>\n");
		exit(1);
	}
	String external_format = String_from_file(argv[1]);
	String internal_format = SGA_external_v_1_0_0_to_internal_v_1_0_0_format(&external_format);
	String_write_to_file(&internal_format, argv[2]);
	String_destroy(external_format);
	String_destroy(internal_format);
	exit(EXIT_SUCCESS);
}