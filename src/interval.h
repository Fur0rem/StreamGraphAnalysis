#ifndef INTERVAL_H
#define INTERVAL_H

#include "units.h"
#include "utils.h"
#include "vector.h"
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
bool Interval_contains_interval(Interval a, Interval b);
bool Interval_overlaps_interval(Interval a, Interval b);

size_t Interval_size(Interval interval);
Interval Interval_from(TimeId start, TimeId end);
Interval Interval_intersection(Interval a, Interval b);
Interval Interval_minus(Interval a, Interval b);

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
IntervalsSet IntervalsSet_intersection(IntervalsSet a, IntervalsSet b);
IntervalsSet IntervalsSet_intersection_with_single(IntervalsSet a, Interval interval);
void IntervalsSet_self_intersection_with_single(IntervalsSet* intervals_set, Interval interval);
IntervalsSet IntervalsSet_union(IntervalsSet a, IntervalsSet b);
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
#endif // INTERVAL_H