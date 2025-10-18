#ifndef SGA_PARSING_VERSION_H
#define SGA_PARSING_VERSION_H

#include "cursor.h"
#include <stdint.h>
#include <stdio.h>

typedef struct SGA_Version {
	uint8_t major, minor, patch;
} SGA_Version;

SGA_Version SGA_parse_version(SGA_ParsingCursor* cursor);

#endif // SGA_PARSING_VERSION_H