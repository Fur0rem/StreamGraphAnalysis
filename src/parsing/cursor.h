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
	const char* filename;		   ///< The name of the file the string is from.
	const char* str;		   ///< The string to parse.
	size_t cursor;			   ///< The current position of the cursor in the string.
	char* current_header_hierarchy[3]; ///< The current header hierarchy, used for error messages.
} SGA_ParsingCursor;

/**
 * @brief Initializes a parsing cursor at the beginning of the given string.
 * @param str The string to parse.
 * @param filename The name of the file the string is from.
 * @return An initialized SGA_ParsingCursor.
 */
SGA_ParsingCursor SGA_ParsingCursor_begin(const char* str, const char* filename);

/**
 * @brief The position of the cursor in the string. Useful to print errors.
 */
typedef struct SGA_CursorPosition {
	size_t line;   ///< Line at which the cursor is at
	size_t column; ///< Column at which the cursor is at
} SGA_CursorPosition;

typedef struct SGA_SourceCodeReference {
	char* src_file_name;
	size_t line_number;
} SGA_SourceCodeReference;

#define SGA_CODE_HERE                                                                                                                      \
	(SGA_SourceCodeReference) {                                                                                                        \
		__FILE__, __LINE__                                                                                                         \
	}

/**
 * @brief A structure representing the result of a parsing operation.
 * Can indicate success or failure, and contains error information if applicable.
 */
typedef struct SGA_ParsingResult {
	bool success;			   ///< Whether the parsing operation was successful.
	SGA_CursorPosition error_position; ///< The position in the string where the error occurred.
	SGA_SourceCodeReference src_ref;   ///< The source code reference for the error.
	String message;			   ///< A message describing the error.
} SGA_ParsingResult;

/**
 * @brief Creates a successful parsing result.
 * @return A SGA_ParsingResult indicating success.
 */
SGA_ParsingResult SGA_Parsing_success();

/**
 * @brief Prints the error message of a parsing result to stderr. Does nothing if the result indicates success.
 * @param result The parsing result to print.
 * @param cursor The parsing cursor, used to extract context for the error.
 */
void SGA_ParsingResult_print_error(SGA_ParsingResult* result, const SGA_ParsingCursor* cursor);

/**
 * @brief Finds the line and column of a cursor position in a string.
 * @param full_str The full string being parsed.
 * @param cursor_position The cursor position to find.
 * @return A SGA_CursorPosition indicating the line and column of the cursor position.
 */
SGA_CursorPosition SGA_find_line_and_column(const char* full_str, size_t cursor_position);

/**
 * @brief Moves the parsing cursor to the next occurrence of the given string.
 * If the string is not found at the current cursor position, returns an error.
 * @param cursor The parsing cursor to move.
 * @param str_to_move The string to move to.
 * @return A SGA_ParsingResult indicating success or failure.
 */
SGA_ParsingResult SGA_ParsingCursor_move_to_next_instance_of(SGA_ParsingCursor* cursor, const char* str_to_move,
							     SGA_SourceCodeReference src_ref);

SGA_ParsingResult SGA_ParsingResult_error(SGA_ParsingCursor* cursor, String message, SGA_SourceCodeReference src_ref);

/**
 * @brief Sets the current header at the given level in the parsing cursor, useful for giving error context.
 * Also moves the cursor to that header.
 *
 * @param cursor The parsing cursor to modify.
 * @param level The header level (0 for header, 1 for sub-header, 2 for sub-sub-header).
 * @param header The header string to set.
 */
SGA_ParsingResult SGA_ParsingCursor_set_current_header(SGA_ParsingCursor* cursor, size_t level, char* header,
						       SGA_SourceCodeReference src_ref);

void SGA_ParsingCursor_print_current_line(const SGA_ParsingCursor* cursor);

SGA_ParsingResult SGA_ParsingCursor_move_to_next_line(SGA_ParsingCursor* cursor, SGA_SourceCodeReference src_ref);

bool SGA_ParsingCursor_line_starts_with(const SGA_ParsingCursor* cursor, const char* str_to_check);

bool SGA_ParsingCursor_line_is_empty(const SGA_ParsingCursor* cursor);

SGA_ParsingResult SGA_ParsingCursor_expect_sequence_and_move(SGA_ParsingCursor* cursor, const char* sequence,
							     SGA_SourceCodeReference src_ref);

SGA_ParsingResult SGA_ParsingCursor_expect_and_move(SGA_ParsingCursor* cursor, char expected, SGA_SourceCodeReference src_ref);

SGA_ParsingResult SGA_ParsingCursor_get_number_and_move(SGA_ParsingCursor* cursor, size_t* out_number, SGA_SourceCodeReference src_ref);

#define SGA_ParsingCursor_scan_and_move(cursor, src_ref, format, ...)                                                                      \
	({                                                                                                                                 \
		int __nb_chars_read;                                                                                                       \
		SGA_ParsingResult __result = SGA_ParsingCursor_scan(cursor, src_ref, format "%n", __VA_ARGS__, &__nb_chars_read);          \
		cursor->cursor += __nb_chars_read;                                                                                         \
		__result;                                                                                                                  \
	})

void SGA_ParsingCursor_skip_whitespace(SGA_ParsingCursor* cursor);

#endif // SGA_PARSING_CURSOR_H