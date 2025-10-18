#ifndef SGA_PARSING_GENERAL_H
#define SGA_PARSING_GENERAL_H

#include "../interval.h"
#include "cursor.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	SGA_Interval lifespan; ///< The lifespan of the stream graph.
	size_t time_scale;     ///< The time scale of the stream graph.
	bool is_weighted;      ///< Whether the stream graph is weighted.
} SGA_GeneralHeader;

SGA_GeneralHeader SGA_parse_general_header(SGA_ParsingCursor* cursor);

#endif // SGA_PARSING_GENERAL_H