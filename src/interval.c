#include "interval.h"
#include "utils.h"
#include "vector.h"

bool SGA_Interval_contains(SGA_Interval interval, TimeId time) {
	return interval.start <= time && time <= interval.end;
}

size_t SGA_Interval_size(SGA_Interval interval) {
	if (interval.start > interval.end) {
		return 0;
	}
	return interval.end - interval.start;
}

SGA_Interval SGA_Interval_from(TimeId start, TimeId end) {
	SGA_Interval interval;
	interval.start = start;
	interval.end = end;
	return interval;
}

SGA_Interval SGA_Interval_intersection(SGA_Interval a, SGA_Interval b) {
	SGA_Interval intersection;
	intersection.start = a.start > b.start ? a.start : b.start;
	intersection.end = a.end < b.end ? a.end : b.end;
	return intersection;
}

size_t SGA_IntervalsSet_size(SGA_IntervalsSet intervals_set) {
	size_t size = 0;
	for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
		size += SGA_Interval_size(intervals_set.intervals[i]);
	}
	return size;
}

SGA_IntervalsSet SGA_IntervalsSet_alloc(size_t nb_intervals) {
	SGA_IntervalsSet set;
	set.nb_intervals = nb_intervals;
	set.intervals = MALLOC(nb_intervals * sizeof(SGA_Interval));
	return set;
}

bool SGA_Interval_equals(SGA_Interval a, SGA_Interval b) {
	return a.start == b.start && a.end == b.end;
}

char* SGA_Interval_to_string(SGA_Interval* interval) {
	char* str = MALLOC(32);
	snprintf(str, 32, "[%lu, %lu]", interval->start, interval->end);
	return str;
}

DefVector(SGA_Interval, NO_FREE(SGA_Interval));

void SGA_IntervalsSet_destroy(SGA_IntervalsSet intervals_set) {
	free(intervals_set.intervals);
}

// Puts together all the intervals that are contiguous or overlapping
// Assumes the intervals are sorted
// TODO: Could probably be optimized
void SGA_IntervalsSet_merge(SGA_IntervalsSet* intervals_set) {
	SGA_IntervalVector merged = SGA_IntervalVector_with_capacity(intervals_set->nb_intervals);
	SGA_Interval current = intervals_set->intervals[0];
	for (size_t i = 1; i < intervals_set->nb_intervals; i++) {
		SGA_Interval next = intervals_set->intervals[i];
		if (current.start == next.start) {
			if (current.end < next.end) {
				current.end = next.end;
			}
			i++;
			next = intervals_set->intervals[i];
		}
		if (current.end >= next.end) {
			continue;
		}
		if (current.end >= next.start) {
			current.end = next.end;
		}
		else {
			SGA_IntervalVector_push(&merged, current);
			current = next;
		}
	}
	SGA_IntervalVector_push(&merged, current);
	SGA_IntervalsSet_destroy(*intervals_set);
	*intervals_set = SGA_IntervalsSet_alloc(merged.size);
	for (size_t i = 0; i < merged.size; i++) {
		intervals_set->intervals[i] = merged.array[i];
	}
	SGA_IntervalVector_destroy(merged);
}

// TODO: Not very efficient, but it works for now (O(n^2)) when we could iterate over the intervals in parallel since
// they are sorted
SGA_IntervalsSet SGA_IntervalsSet_intersection(SGA_IntervalsSet a, SGA_IntervalsSet b) {
	SGA_IntervalVector intersection = SGA_IntervalVector_with_capacity(a.nb_intervals + b.nb_intervals);
	for (size_t i = 0; i < a.nb_intervals; i++) {
		for (size_t j = 0; j < b.nb_intervals; j++) {
			SGA_Interval a_interval = a.intervals[i];
			SGA_Interval b_interval = b.intervals[j];
			SGA_Interval intersection_interval = SGA_Interval_intersection(a_interval, b_interval);
			if (SGA_Interval_size(intersection_interval) > 0) {
				SGA_IntervalVector_push(&intersection, intersection_interval);
			}
		}
	}
	SGA_IntervalsSet result = SGA_IntervalsSet_alloc(intersection.size);
	for (size_t i = 0; i < intersection.size; i++) {
		result.intervals[i] = intersection.array[i];
	}
	SGA_IntervalVector_destroy(intersection);
	return result;
}

int Interval_starts_before(const void* a, const void* b) {
	SGA_Interval* interval_a = (SGA_Interval*)a;
	SGA_Interval* interval_b = (SGA_Interval*)b;
	return interval_a->start - interval_b->start;
}

void SGA_IntervalsSet_sort(SGA_IntervalsSet* intervals_set) {
	qsort(intervals_set->intervals, intervals_set->nb_intervals, sizeof(SGA_Interval), Interval_starts_before);
}

// TODO: Not very efficient either, but i prefer to focus on actually important stuff and come back to this later
SGA_IntervalsSet SGA_IntervalsSet_union(SGA_IntervalsSet a, SGA_IntervalsSet b) {
	SGA_IntervalVector union_intervals = SGA_IntervalVector_with_capacity(a.nb_intervals + b.nb_intervals);
	for (size_t i = 0; i < a.nb_intervals; i++) {
		SGA_IntervalVector_push(&union_intervals, a.intervals[i]);
	}
	for (size_t i = 0; i < b.nb_intervals; i++) {
		SGA_IntervalVector_push(&union_intervals, b.intervals[i]);
	}
	SGA_IntervalsSet result = SGA_IntervalsSet_alloc(union_intervals.size);
	for (size_t i = 0; i < union_intervals.size; i++) {
		result.intervals[i] = union_intervals.array[i];
	}
	SGA_IntervalsSet_sort(&result);
	SGA_IntervalsSet_merge(&result);
	SGA_IntervalVector_destroy(union_intervals);
	return result;
}