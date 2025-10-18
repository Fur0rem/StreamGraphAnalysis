#include "cursor.h"

SGA_ParsingCursor SGA_ParsingCursor_begin(const char* str, const char* filename) {
	SGA_ParsingCursor cursor;
	cursor.filename = filename;
	cursor.str	= str;
	cursor.cursor	= 0;
	for (size_t i = 0; i < 3; i++) {
		cursor.current_header_hierarchy[i] = NULL;
	}
	return cursor;
}

SGA_ParsingResult SGA_Parsing_success() {
	return (SGA_ParsingResult){
	    .success = true,
	};
}

#define RED	 "\033[31m"
#define RESET	 "\033[0m"
#define BOLD	 "\033[1m"
#define BOLD_RED "\033[1;31m"

void SGA_ParsingResult_print_error(SGA_ParsingResult* result, const SGA_ParsingCursor* cursor) {
	if (result->success) {
		return;
	}

	// Print the error message
	fprintf(stderr,
		BOLD_RED "Parsing error" RESET " in file %s at line %zu, column %zu\n",
		cursor->filename,
		result->error_position.line,
		result->error_position.column);

	// Print the faulty line
	const char* ptr	    = cursor->str;
	size_t current_line = 1;
	while (current_line < result->error_position.line && *ptr != '\0') {
		if (*ptr == '\n') {
			current_line++;
		}
		ptr++;
	}
	// Now ptr is at the beginning of the faulty line
	const char* line_start = ptr;
	while (*ptr != '\n' && *ptr != '\0') {
		ptr++;
	}
	size_t line_length = ptr - line_start;
	char* line	   = (char*)malloc(line_length + 1);
	strncpy(line, line_start, line_length);
	line[line_length] = '\0';
	fprintf(stderr, "%s\n", line);
	free(line);

	// Print a caret under the error position
	for (size_t i = 1; i < result->error_position.column; i++) {
		fprintf(stderr, " ");
	}

	fprintf(stderr, "%s^\n%s", RED, RESET);

	// Print the error message
	String_null_terminate(&result->message);
	fprintf(stderr, "%s\n", result->message.data);

	// Print surrounding context
	fprintf(stderr, "Surrounding context:\n");
	if (cursor->current_header_hierarchy[0] != NULL) {
		fprintf(stderr, "In header: %s\n", cursor->current_header_hierarchy[0]);
	}
	else {
		fprintf(stderr, "In file prelude\n");
	}

	if (cursor->current_header_hierarchy[1] != NULL) {
		fprintf(stderr, "In sub-header: %s\n", cursor->current_header_hierarchy[1]);
	}
	if (cursor->current_header_hierarchy[2] != NULL) {
		fprintf(stderr, "In sub-sub-header: %s\n", cursor->current_header_hierarchy[2]);
	}

	// Print source code reference
	fprintf(stderr, "Source code reference: %s:%zu\n", result->src_ref.src_file_name, result->src_ref.line_number);
}

const char* const format_to_type_strings[][2] = {
    {"%zu", "unsigned integer"},
    {"%hhu", "unsigned integer"},
    {"%c", "character"},
    {"%s", "string"},
    {"%f", "floating point number"},
    {"%lf", "floating point number"},
};

const char* get_type_string_from_format(const char* format) {
	for (size_t i = 0; i < sizeof(format_to_type_strings) / sizeof(format_to_type_strings[0]); i++) {
		if (strstr(format, format_to_type_strings[i][0]) != NULL) {
			return format_to_type_strings[i][1];
		}
	}
	return "unknown type (Shouldn't happen - Please report a bug)";
}

SGA_CursorPosition SGA_find_line_and_column(const char* full_str, size_t cursor_position) {
	size_t line	= 1;
	size_t column	= 1;
	const char* ptr = full_str;
	while ((size_t)(ptr - full_str) < cursor_position) {
		if (*ptr == '\n') {
			line++;
			column = 1;
		}
		else {
			column++;
		}
		ptr++;
	}
	return (SGA_CursorPosition){
	    .line   = line,
	    .column = column,
	};
}

SGA_ParsingResult SGA_ParsingCursor_move_to_next_instance_of(SGA_ParsingCursor* cursor, const char* str_to_move,
							     SGA_SourceCodeReference src_ref) {
	const char* found = strstr(cursor->str + cursor->cursor, str_to_move);
	if (found == NULL) {
		return (SGA_ParsingResult){
		    .success	    = false,
		    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
		    .message	    = String_from_format("Expected \"%s\", couldn't find it", str_to_move),
		    .src_ref	    = src_ref,
		};
	}
	cursor->cursor = (size_t)(found - cursor->str);
	return SGA_Parsing_success();
}

SGA_ParsingResult SGA_ParsingCursor_set_current_header(SGA_ParsingCursor* cursor, size_t level, char* header,
						       SGA_SourceCodeReference src_ref) {
	ASSERT(level < 3);

	// Set the header at the given level and clear lower levels
	cursor->current_header_hierarchy[level] = header;
	for (size_t i = level + 1; i < 3; i++) {
		cursor->current_header_hierarchy[i] = NULL;
	}

	// Move the cursor to the header
	SGA_ParsingResult result = SGA_ParsingCursor_move_to_next_instance_of(cursor, header, src_ref);
	return result;
}

SGA_ParsingResult SGA_ParsingCursor_move_to_next_line(SGA_ParsingCursor* cursor, SGA_SourceCodeReference src_ref) {
	SGA_ParsingResult result = SGA_ParsingCursor_move_to_next_instance_of(cursor, "\n", src_ref);
	if (!result.success) {
		return result;
	}
	cursor->cursor += 1; // Move past the newline
	return result;
}

bool SGA_ParsingCursor_line_starts_with(const SGA_ParsingCursor* cursor, const char* str_to_check) {
	const char* ptr = cursor->str + cursor->cursor;
	while (*ptr == ' ' || *ptr == '\t') {
		ptr++;
	}
	size_t len = strlen(str_to_check);
	return strncmp(ptr, str_to_check, len) == 0;
}

bool SGA_ParsingCursor_line_is_empty(const SGA_ParsingCursor* cursor) {
	return cursor->str[cursor->cursor] == '\n';
}

SGA_ParsingResult SGA_ParsingCursor_expect_and_move(SGA_ParsingCursor* cursor, char expected, SGA_SourceCodeReference src_ref) {
	if (*(cursor->str + cursor->cursor) != expected) {
		return (SGA_ParsingResult){
		    .success	    = false,
		    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
		    .message	    = String_from_format("Expected '%c'", expected),
		    .src_ref	    = src_ref,
		};
	}
	cursor->cursor++;
	return SGA_Parsing_success();
}

SGA_ParsingResult SGA_ParsingCursor_expect_sequence_and_move(SGA_ParsingCursor* cursor, const char* sequence,
							     SGA_SourceCodeReference src_ref) {
	size_t len = strlen(sequence);
	if (strncmp(cursor->str + cursor->cursor, sequence, len) != 0) {
		return (SGA_ParsingResult){
		    .success	    = false,
		    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
		    .message	    = String_from_format("Expected \"%s\"", sequence),
		    .src_ref	    = src_ref,
		};
	}
	cursor->cursor += len;
	return SGA_Parsing_success();
}

SGA_ParsingResult SGA_ParsingCursor_get_number_and_move(SGA_ParsingCursor* cursor, size_t* out_number, SGA_SourceCodeReference src_ref) {
	char* new_ptr;
	const char* str = cursor->str + cursor->cursor;
	long number	= strtol(str, &new_ptr, 10);
	if (str == new_ptr) {
		return (SGA_ParsingResult){
		    .success	    = false,
		    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
		    .message	    = String_from_format("Expected a number here."),
		    .src_ref	    = src_ref,
		};
	}
	str	       = new_ptr;
	*out_number    = (size_t)number;
	cursor->cursor = (size_t)(str - cursor->str);
	return SGA_Parsing_success();
}

SGA_ParsingResult SGA_ParsingResult_error(SGA_ParsingCursor* cursor, String message, SGA_SourceCodeReference src_ref) {
	SGA_ParsingResult result = {
	    .success	    = false,
	    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
	    .message	    = message,
	    .src_ref	    = src_ref,
	};
	return result;
}

void SGA_ParsingCursor_skip_whitespace(SGA_ParsingCursor* cursor) {
	while (true) {
		char current_char = cursor->str[cursor->cursor];
		if (current_char == ' ' || current_char == '\t' || current_char == '\r') {
			cursor->cursor++;
		}
		else {
			break;
		}
	}
}

void SGA_ParsingCursor_print_current_line(const SGA_ParsingCursor* cursor) {
	const char* ptr	    = cursor->str;
	size_t current_line = 1;
	while (current_line < 1 && *ptr != '\0') {
		if (*ptr == '\n') {
			current_line++;
		}
		ptr++;
	}
	// Now ptr is at the beginning of the current line
	const char* line_start = ptr;
	while (*ptr != '\n' && *ptr != '\0') {
		ptr++;
	}
	size_t line_length = ptr - line_start;
	char* line	   = (char*)malloc(line_length + 1);
	strncpy(line, line_start, line_length);
	line[line_length] = '\0';
	fprintf(stderr, "Current line: %s\n", line);
	free(line);
}