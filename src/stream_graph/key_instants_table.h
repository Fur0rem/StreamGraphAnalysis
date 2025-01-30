/**
 * @file src/stream_graph/key_instants_table.h
 * @brief Table of all key instants
 */

#ifndef SGA_KEY_INSTANTS_TABLE_H
#define SGA_KEY_INSTANTS_TABLE_H

#include "../units.h"
#include "../utils.h"
#include <stddef.h>
#include <stdint.h>

typedef uint8_t RelativeInstant;
#define RELATIVE_INSTANT_MAX ((RelativeInstant)~0)
#define SLICE_SIZE	     (((size_t)RELATIVE_INSTANT_MAX) + 1)

typedef struct {
	size_t nb_instants;
	RelativeInstant* instants;
} InstantsSlice;

typedef struct {
	SGA_TimeId current_slice;
	RelativeInstant current_instant;
} KeyInstantsTableIterator;

typedef struct {
	SGA_TimeId nb_slices;
	InstantsSlice* slices;
	KeyInstantsTableIterator fill_info;
} KeyInstantsTable;

#ifdef SGA_INTERNAL

SGA_Time KeyInstantsTable_nth_key_instant(KeyInstantsTable* kmt, SGA_TimeId n);

void KeyInstantsTable_push_in_order(KeyInstantsTable* kmt, SGA_Time key_instant);
KeyInstantsTable KeyInstantsTable_alloc(size_t nb_slices);

void KeyInstantsTable_alloc_slice(KeyInstantsTable* kmt, SGA_TimeId slice, RelativeInstant nb_instants);
SGA_Time KeyInstantsTable_first_instant(KeyInstantsTable* kmt);

SGA_Time KeyInstantsTable_last_instant(KeyInstantsTable* kmt);
void KeyInstantsTable_destroy(KeyInstantsTable kmt);

// Assumes the times are sorted, returns the index of a certain time if it was pushed in that array, all of them were in a single array
SGA_TimeId KeyInstantsTable_find_time_index_if_pushed(KeyInstantsTable* kmt, SGA_Time t);

// Returns the index of the the instant which has equivalent events to time t
SGA_TimeId KeyInstantsTable_find_time_index_equivalent(KeyInstantsTable* kmt, SGA_Time t);

void print_key_instants_table(KeyInstantsTable* kmt);

#endif // SGA_INTERNAL

#endif // SGA_KEY_INSTANTS_TABLE_H