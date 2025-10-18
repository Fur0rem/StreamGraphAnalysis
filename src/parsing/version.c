#include "cursor.h"
#include <stdint.h>
#include <stdio.h>

typedef struct SGA_Version {
	uint8_t major, minor, patch;
} SGA_Version;

#define BOLD	  "\033[1m"
#define BLUE	  "\033[0;34m"
#define BOLD_BLUE "\033[1;34m"
#define RESET	  "\033[0m"

SGA_Version SGA_parse_version(SGA_ParsingCursor* cursor) {
	SGA_Version version;

	SGA_ParsingResult result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "SGA version ", SGA_CODE_HERE);
	if (!result.success) {
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD "Failed to parse version header!\n" RESET);
		fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: \"SGA version X.Y.Z\"\n");
		exit(1);
	}
	result = SGA_ParsingCursor_scan(cursor, SGA_CODE_HERE, "%hhu.%hhu.%hhu", &version.major, &version.minor, &version.patch);
	if (!result.success) {
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD "Failed to parse version!\n" RESET);
		fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: \"SGA version X.Y.Z\", with X Y Z three unsigned integers < 256\n");
		exit(1);
	}
	return version;
}