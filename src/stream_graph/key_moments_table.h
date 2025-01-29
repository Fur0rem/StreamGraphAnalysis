/**
 * @file src/stream_graph/key_moments_table.h
 * @brief Table of all key moments
 */

#ifndef SGA_KEY_MOMENTS_TABLE_H
#define SGA_KEY_MOMENTS_TABLE_H

#include "../units.h"
#include "../utils.h"
#include <stddef.h>
#include <stdint.h>

typedef uint8_t RelativeMoment;
#define RELATIVE_MOMENT_MAX ((RelativeMoment)~0)
#define SLICE_SIZE	    (((size_t)RELATIVE_MOMENT_MAX) + 1)

typedef struct {
	size_t nb_moments;
	RelativeMoment* moments;
} MomentsSlice;

typedef struct {
	SGA_TimeId current_slice;
	RelativeMoment current_moment;
} KeyMomentsTableIterator;

typedef struct {
	SGA_TimeId nb_slices;
	MomentsSlice* slices;
	KeyMomentsTableIterator fill_info;
} KeyMomentsTable;

#ifdef SGA_INTERNAL

SGA_Time KeyMomentsTable_nth_key_moment(KeyMomentsTable* kmt, SGA_TimeId n);

void KeyMomentsTable_push_in_order(KeyMomentsTable* kmt, SGA_Time key_moment);
KeyMomentsTable KeyMomentsTable_alloc(size_t nb_slices);

void KeyMomentsTable_alloc_slice(KeyMomentsTable* kmt, SGA_TimeId slice, RelativeMoment nb_moments);
SGA_Time KeyMomentsTable_first_moment(KeyMomentsTable* kmt);

SGA_Time KeyMomentsTable_last_moment(KeyMomentsTable* kmt);
void KeyMomentsTable_destroy(KeyMomentsTable kmt);

// Assumes the times are sorted, returns the index of a certain time if it was pushed in that array, all of them were in a single array
SGA_TimeId KeyMomentsTable_find_time_index_if_pushed(KeyMomentsTable* kmt, SGA_Time t);

// Returns the index of the the moment which has equivalent events to time t
SGA_TimeId KeyMomentsTable_find_time_index_equivalent(KeyMomentsTable* kmt, SGA_Time t);

void print_key_moments_table(KeyMomentsTable* kmt);

#endif // SGA_INTERNAL

#endif // SGA_KEY_MOMENTS_TABLE_H