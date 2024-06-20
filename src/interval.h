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

bool Interval_contains(Interval interval, TimeId time);
size_t Interval_size(Interval interval);
Interval Interval_from(TimeId start, TimeId end);
Interval Interval_intersection(Interval a, Interval b);
bool Interval_equals(Interval a, Interval b);
char* Interval_to_string(Interval* interval);

DefVector(Interval, NO_FREE(Interval));

int Interval_starts_before(const void* a, const void* b);

size_t IntervalsSet_size(IntervalsSet intervals_set);
IntervalsSet IntervalsSet_alloc(size_t nb_intervals);
void IntervalsSet_merge(IntervalsSet* intervals_set);
IntervalsSet IntervalsSet_intersection(IntervalsSet a, IntervalsSet b);
IntervalsSet IntervalsSet_union(IntervalsSet a, IntervalsSet b);
void IntervalsSet_destroy(IntervalsSet intervals_set);
Interval IntervalsSet_last(IntervalsSet* intervals_set);
bool IntervalsSet_contains(IntervalsSet intervals_set, TimeId time);
#endif // INTERVAL_H