#include "general.h"
#include "../interval.h"
#include "cursor.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define BOLD	  "\033[1m"
#define BLUE	  "\033[0;34m"
#define BOLD_BLUE "\033[1;34m"
#define RESET	  "\033[0m"

void crash_if_lifespan_invalid(SGA_ParsingResult result, SGA_ParsingCursor* cursor, const char* error) {
	if (!result.success) {
		fprintf(stderr, "%s", error);
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: \"lifespan=(start end)\" with start <= end\n");
		exit(1);
	}
}

SGA_GeneralHeader SGA_parse_general_header(SGA_ParsingCursor* cursor) {
	SGA_GeneralHeader header;

	SGA_ParsingResult result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "lifespan=(", SGA_CODE_HERE);
	crash_if_lifespan_invalid(result, cursor, "Failed to parse lifespan!\n");

	result = SGA_ParsingCursor_get_number_and_move(cursor, &header.lifespan.start, SGA_CODE_HERE);
	crash_if_lifespan_invalid(result, cursor, "Failed to parse lifespan start!\n");
	cursor->cursor++; // Skip space

	result = SGA_ParsingCursor_get_number_and_move(cursor, &header.lifespan.end, SGA_CODE_HERE);
	crash_if_lifespan_invalid(result, cursor, "Failed to parse lifespan end!\n");

	result = SGA_ParsingCursor_expect_sequence_and_move(cursor, ")", SGA_CODE_HERE);
	crash_if_lifespan_invalid(result, cursor, "Failed to parse lifespan closing parenthesis!\n");

	if (header.lifespan.start > header.lifespan.end) {
		result.success = false;
	}
	crash_if_lifespan_invalid(result, cursor, "Lifespan start is greater than end!\n");

	SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);

	// Parse time_scale
	result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "time_scale=", SGA_CODE_HERE);
	if (!result.success) {
		fprintf(stderr, "Failed to parse time_scale!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: \"time_scale=\" followed by an unsigned integer\n");
		exit(1);
	}
	result = SGA_ParsingCursor_get_number_and_move(cursor, &header.time_scale, SGA_CODE_HERE);
	if (!result.success) {
		fprintf(stderr, "Failed to parse time_scale!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, BOLD_BLUE "Hint" RESET " - Expected: an unsigned integer for time_scale\n");
		exit(1);
	}

	// Parse weighted
	bool is_weighted;
	if (SGA_ParsingCursor_line_starts_with(cursor, "weighted=")) {
		result = SGA_ParsingCursor_expect_sequence_and_move(cursor, "weighted=", SGA_CODE_HERE);
		if (!result.success) {
			fprintf(stderr, "Failed to parse weighted!\n");
			SGA_ParsingResult_print_error(&result, cursor);
			fprintf(stderr, "Hint - Expected: \"weighted=true\" or \"weighted=false\"\n");
			exit(1);
		}

		if (SGA_ParsingCursor_line_starts_with(cursor, "true")) {
			is_weighted = true;
			cursor->cursor += 4;
		}
		else if (SGA_ParsingCursor_line_starts_with(cursor, "false")) {
			is_weighted = false;
			cursor->cursor += 5;
		}

		else {
			result.success = false;
			result.message = String_from_duplicate("Weighted was neither 'true' nor 'false'");
			SGA_ParsingResult_print_error(&result, cursor);
			fprintf(stderr, "Hint - Expected: \"weighted=true\" or \"weighted=false\"\n");
			exit(1);
		}
	}
	// Unweighted by default
	else {
		is_weighted = false;
	}
	header.is_weighted = is_weighted;

	SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);

	return header;
}
