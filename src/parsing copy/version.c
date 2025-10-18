#include "cursor.h"
#include <stdint.h>
#include <stdio.h>

typedef struct SGA_Version {
	uint8_t major, minor, patch;
} SGA_Version;

SGA_Version SGA_parse_version(SGA_ParsingCursor* cursor) {
	SGA_Version version;
	SGA_ParsingResult result = SGA_ParsingCursor_scan(cursor, "%hhu.%hhu.%hhu", &version.major, &version.minor, &version.patch);
	if (!result.success) {
		fprintf(stderr, "Failed to parse version!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, "Hint - Expected: \"SGA version X.Y.Z\"\n");
		exit(1);
	}
	return version;
}