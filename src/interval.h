#ifndef INTERVAL_H
#define INTERVAL_H

#include "generic_data_structures/vector.h"
#include "units.h"
#include "utils.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	TimeId start;
	TimeId end;
} Interval;

typedef struct {
	size_t nb_intervals;
	Interval* intervals;
} IntervalsSet;

// TODO: maybe change the name of contains ?
bool Interval_contains(Interval interval, TimeId time);
bool Interval_contains_interval(Interval container, Interval contained);
bool Interval_overlaps_interval(Interval left, Interval right);

size_t Interval_duration(Interval interval);
Interval Interval_from(TimeId start, TimeId end);
Interval Interval_intersection(Interval left, Interval right);
Interval Interval_minus(Interval left, Interval right);

DeclareToString(Interval);
DeclareEquals(Interval);

DeclareVector(Interval);
DeclareVectorDeriveRemove(Interval);
DeclareVectorDeriveEquals(Interval);
DeclareVectorDeriveToString(Interval);

// int Interval_starts_before(const void* a, const void* b);

size_t IntervalsSet_size(IntervalsSet intervals_set);
IntervalsSet IntervalsSet_alloc(size_t nb_intervals);
void IntervalsSet_merge(IntervalsSet* intervals_set);
IntervalsSet IntervalsSet_intersection(IntervalsSet left, IntervalsSet right);
IntervalVector IntervalVector_intersection(IntervalVector* left, IntervalVector* right);
IntervalsSet IntervalsSet_intersection_with_single(IntervalsSet set, Interval interval);
void IntervalsSet_self_intersection_with_single(IntervalsSet* intervals_set, Interval interval);
IntervalsSet IntervalsSet_union(IntervalsSet left, IntervalsSet right);
void IntervalsSet_destroy(IntervalsSet intervals_set);
Interval IntervalsSet_last(IntervalsSet* intervals_set);
bool IntervalsSet_contains(IntervalsSet intervals_set, TimeId time);
bool IntervalsSet_contains_sorted(IntervalsSet intervals_set, TimeId time);

void IntervalsSet_add_at(IntervalsSet* intervals_set, Interval interval, size_t index);

IntervalsSet IntervalsSet_from_interval_vector(IntervalVector intervals);
IntervalVector IntervalVector_from_intervals_set(IntervalsSet intervals_set);

void IntervalsSet_sort(IntervalsSet* intervals_set);

Interval Interval_empty();
bool Interval_is_empty(Interval interval);

typedef struct {
	enum {
		OK,
		EMPTY_INTERVALSET,
		NO_MATCHING_OFFSET,
	} result;
	size_t offset;
} SGA_Offset;

SGA_Offset IntervalVector_offset_of(const IntervalVector* self, const IntervalVector* other);
SGA_Offset Interval_offset_of(const Interval* self, const Interval* other);

bool SGA_Offset_is_ok(SGA_Offset offset);

bool SGA_Offset_is_empty(SGA_Offset offset);

bool SGA_Offset_is_not_matching(SGA_Offset offset);

size_t SGA_Offset_unwrap(SGA_Offset offset);

#endif // INTERVAL_H