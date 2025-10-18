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

void crash_if_not_successful(SGA_ParsingResult result, SGA_ParsingCursor* cursor) {
	if (!result.success) {
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD "Failed to parse version header!\n" RESET);
		fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: \"SGA version X.Y.Z\"\n");
		exit(1);
	}
}

SGA_Version SGA_parse_version(SGA_ParsingCursor* cursor) {
	SGA_Version version;

	SGA_ParsingResult result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "SGA version ", SGA_CODE_HERE);
	crash_if_not_successful(result, cursor);

	size_t version_numbers[3];
	for (size_t i = 0; i < 3; i++) {
		result = SGA_ParsingCursor_get_number_and_move(cursor, &version_numbers[i], SGA_CODE_HERE);
		if (!result.success) {
			SGA_ParsingResult_print_error(&result, cursor);
			fprintf(stderr, BOLD "Failed to parse version header!\n" RESET);
			fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: \"SGA version X.Y.Z\"\n");
			exit(1);
		}
		if (i < 2) {
			SGA_ParsingCursor_expect_and_move(cursor, '.', SGA_CODE_HERE);
		}
	}

	size_t major = version_numbers[0];
	size_t minor = version_numbers[1];
	size_t patch = version_numbers[2];
	if (major > 255 || minor > 255 || patch > 255) {
		result.success = false;
		result.message = String_from_format("Version numbers must be between 0 and 255.");
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD "Failed to parse version header!\n" RESET);
		exit(1);
	}

	version.major = major;
	version.minor = minor;
	version.patch = patch;
	return version;
}