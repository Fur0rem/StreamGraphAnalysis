#include "general.h"
#include "../interval.h"
#include "cursor.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

SGA_GeneralHeader SGA_parse_general_header(SGA_ParsingCursor* cursor) {
	SGA_GeneralHeader header;

	// Parse lifespan
	SGA_ParsingResult result =
	    SGA_ParsingCursor_scan(cursor, SGA_CODE_HERE, "lifespan=(%zu %zu)", &header.lifespan.start, &header.lifespan.end);
	if (!result.success) {
		fprintf(stderr, "Failed to parse lifespan!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, "Hint - Expected: \"lifespan=(start end)\"\n");
		exit(1);
	}
	SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);

	// Parse time_scale
	result = SGA_ParsingCursor_scan(cursor, SGA_CODE_HERE, "time_scale=%zu", &header.time_scale);
	if (!result.success) {
		fprintf(stderr, "Failed to parse time_scale!\n");
		SGA_ParsingResult_print_error(&result, cursor);
		fprintf(stderr, "Hint - Expected: \"time_scale=value\"\n");
		exit(1);
	}
	SGA_ParsingCursor_move_to_next_line(cursor, SGA_CODE_HERE);

	// Parse weighted
	bool is_weighted;
	if (SGA_ParsingCursor_line_starts_with(cursor, "weighted=")) {
		char is_weighted_str[6];
		result = SGA_ParsingCursor_scan(cursor, SGA_CODE_HERE, "weighted=%5s", is_weighted_str);
		if (!result.success) {
			fprintf(stderr, "Failed to parse weighted!\n");
			SGA_ParsingResult_print_error(&result, cursor);
			fprintf(stderr, "Hint - Expected: \"weighted=true\" or \"weighted=false\"\n");
			exit(1);
		}
		if (strcmp(is_weighted_str, "true") == 0) {
			is_weighted = true;
		}
		else if (strcmp(is_weighted_str, "false") == 0) {
			is_weighted = false;
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
