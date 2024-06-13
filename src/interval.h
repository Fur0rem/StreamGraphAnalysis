#ifndef SGA_INTERVAL_H
#define SGA_INTERVAL_H

#include "units.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	TimeId start;
	TimeId end;
} SGA_Interval;

typedef struct {
	size_t nb_intervals;
	SGA_Interval* intervals;
} SGA_IntervalsSet;

bool SGA_Interval_contains(SGA_Interval interval, TimeId time);
size_t SGA_Interval_size(SGA_Interval interval);
SGA_Interval SGA_Interval_from(TimeId start, TimeId end);
SGA_Interval SGA_Interval_intersection(SGA_Interval a, SGA_Interval b);
size_t SGA_IntervalsSet_size(SGA_IntervalsSet intervals_set);
SGA_IntervalsSet SGA_IntervalsSet_alloc(size_t nb_intervals);
void SGA_IntervalsSet_merge(SGA_IntervalsSet* intervals_set);
SGA_IntervalsSet SGA_IntervalsSet_intersection(SGA_IntervalsSet a, SGA_IntervalsSet b);
SGA_IntervalsSet SGA_IntervalsSet_union(SGA_IntervalsSet a, SGA_IntervalsSet b);
void SGA_IntervalsSet_destroy(SGA_IntervalsSet intervals_set);

#endif // SGA_INTERVAL_H