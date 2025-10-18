#ifndef SGA_PARSING_CURSOR_H
#define SGA_PARSING_CURSOR_H

#include "../utils.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief A cursor to parse a stream graph step by step.
 */
typedef struct SGA_ParsingCursor {
	const char* str;		   ///< The string to parse.
	size_t cursor;			   ///< The current position of the cursor in the string.
	char* current_header_hierarchy[3]; ///< The current header hierarchy, used for error messages.
} SGA_ParsingCursor;

SGA_ParsingCursor SGA_ParsingCursor_begin(const char* str) {
	SGA_ParsingCursor cursor;
	cursor.str    = str;
	cursor.cursor = 0;
	for (size_t i = 0; i < 3; i++) {
		cursor.current_header_hierarchy[i] = NULL;
	}
	return cursor;
}

typedef struct SGA_CursorPosition {
	size_t line;
	size_t column;
} SGA_CursorPosition;

/**
 * @brief A structure representing the result of a parsing operation.
 * Can indicate success or failure, and contains error information if applicable.
 */
typedef struct SGA_ParsingResult {
	bool success;			   ///< Whether the parsing operation was successful.
	SGA_CursorPosition error_position; ///< The position in the string where the error occurred.
	String message;			   ///< A message describing the error.
} SGA_ParsingResult;

SGA_ParsingResult SGA_Parsing_success() {
	return (SGA_ParsingResult){
	    .success = true,
	};
}

#define RED	 "\033[31m"
#define RESET	 "\033[0m"
#define BOLD	 "\033[1m"
#define BOLD_RED "\033[1;31m"

void SGA_ParsingResult_print_error(const SGA_ParsingResult* result, const SGA_ParsingCursor* cursor) {
	if (result->success) {
		return;
	}
	// Print the error message
	fprintf(stderr,
		BOLD_RED "Parsing error" RESET " at line %zu, column %zu\n",
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
	fprintf(stderr, "%s\n", result->message.data);

	// Print surrounding context
	fprintf(stderr, "Surrounding context:\n");
	if (cursor->current_header_hierarchy[0] != NULL) {
		fprintf(stderr, "In header: %s\n", cursor->current_header_hierarchy[0]);
	}
	if (cursor->current_header_hierarchy[1] != NULL) {
		fprintf(stderr, "In sub-header: %s\n", cursor->current_header_hierarchy[1]);
	}
	if (cursor->current_header_hierarchy[2] != NULL) {
		fprintf(stderr, "In sub-sub-header: %s\n", cursor->current_header_hierarchy[2]);
	}
}

const char* const format_to_type_strings[][2] = {
    {"%zu", "unsigned integer"},
    {"%hhu", "unsigned integer"},
    {"%c", "character"},
    {"%f", "floating point number"},
    {"%lf", "floating point number"},
};

const char* get_type_string_from_format(const char* format) {
	for (size_t i = 0; i < sizeof(format_to_type_strings) / sizeof(format_to_type_strings[0]); i++) {
		if (strcmp(format, format_to_type_strings[i][0]) == 0) {
			return format_to_type_strings[i][1];
		}
	}
	return "unknown type";
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

SGA_ParsingResult SGA_ParsingCursor_move_to_next(SGA_ParsingCursor* cursor, const char* str_to_move) {
	size_t len = strlen(str_to_move);
	if (strncmp(cursor->str + cursor->cursor, str_to_move, len) != 0) {
		return (SGA_ParsingResult){
		    .success	    = false,
		    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
		    .message	    = String_from_format("Expected \"%s\", couldn't find it", str_to_move),
		};
	}
	cursor->cursor += len;
	return SGA_Parsing_success();
}

void SGA_ParsingCursor_set_current_header(SGA_ParsingCursor* cursor, size_t level, char* header) {
	if (level < 3) {
		cursor->current_header_hierarchy[level] = header;
		for (size_t i = level + 1; i < 3; i++) {
			cursor->current_header_hierarchy[i] = NULL;
		}
	}
}

SGA_ParsingResult SGA_ParsingCursor_move_to_next_line(SGA_ParsingCursor* cursor) {
	return SGA_ParsingCursor_move_to_next(cursor, "\n");
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

SGA_ParsingResult SGA_ParsingCursor_scan(SGA_ParsingCursor* cursor, const char* format, ...) {
	va_list args;
	va_start(args, format);
	size_t initial_cursor = cursor->cursor;
	int scanned	      = vsscanf(cursor->str + cursor->cursor, format, args);
	va_end(args);
	if (scanned <= 0) {
		return (SGA_ParsingResult){
		    .success	    = false,
		    .error_position = SGA_find_line_and_column(cursor->str, cursor->cursor),
		    .message	    = String_from_format("Expected a %s here", get_type_string_from_format(format)),
		};
	}
	cursor->cursor += initial_cursor - cursor->cursor;
	return SGA_Parsing_success();
}

#endif // SGA_PARSING_CURSOR_H