#include "interval.h"
#include "utils.h"
#include "vector.h"

bool Interval_contains(Interval interval, TimeId time) {
	return interval.start <= time && time < interval.end;
}

size_t Interval_size(Interval interval) {
	if (interval.start > interval.end) {
		return 0;
	}
	return interval.end - interval.start;
}

Interval Interval_from(TimeId start, TimeId end) {
	Interval interval;
	interval.start = start;
	interval.end = end;
	return interval;
}

Interval Interval_intersection(Interval a, Interval b) {
	Interval intersection;
	intersection.start = a.start > b.start ? a.start : b.start;
	intersection.end = a.end < b.end ? a.end : b.end;
	return intersection;
}

size_t IntervalsSet_size(IntervalsSet intervals_set) {
	size_t size = 0;
	for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
		size += Interval_size(intervals_set.intervals[i]);
	}
	return size;
}

IntervalsSet IntervalsSet_alloc(size_t nb_intervals) {
	IntervalsSet set;
	set.nb_intervals = nb_intervals;
	set.intervals = MALLOC(nb_intervals * sizeof(Interval));
	return set;
}

bool Interval_equals(Interval a, Interval b) {
	return a.start == b.start && a.end == b.end;
}

char* Interval_to_string(Interval* interval) {
	char* str = MALLOC(32);
	snprintf(str, 32, "[%lu, %lu]", interval->start, interval->end);
	return str;
}

void IntervalsSet_destroy(IntervalsSet intervals_set) {
	free(intervals_set.intervals);
}

// Puts together all the intervals that are contiguous or overlapping
// Assumes the intervals are sorted
// TODO: Could probably be optimized
void IntervalsSet_merge(IntervalsSet* intervals_set) {
	IntervalVector merged = IntervalVector_with_capacity(intervals_set->nb_intervals);
	Interval current = intervals_set->intervals[0];
	for (size_t i = 1; i < intervals_set->nb_intervals; i++) {
		Interval next = intervals_set->intervals[i];
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
			IntervalVector_push(&merged, current);
			current = next;
		}
	}
	IntervalVector_push(&merged, current);
	IntervalsSet_destroy(*intervals_set);
	*intervals_set = IntervalsSet_alloc(merged.size);
	for (size_t i = 0; i < merged.size; i++) {
		intervals_set->intervals[i] = merged.array[i];
	}
	IntervalVector_destroy(merged);
}

// TODO: Not very efficient, but it works for now (O(n^2)) when we could iterate over the intervals in parallel since
// they are sorted
IntervalsSet IntervalsSet_intersection(IntervalsSet a, IntervalsSet b) {
	IntervalVector intersection = IntervalVector_with_capacity(a.nb_intervals + b.nb_intervals);
	for (size_t i = 0; i < a.nb_intervals; i++) {
		for (size_t j = 0; j < b.nb_intervals; j++) {
			Interval a_interval = a.intervals[i];
			Interval b_interval = b.intervals[j];
			Interval intersection_interval = Interval_intersection(a_interval, b_interval);
			if (Interval_size(intersection_interval) > 0) {
				IntervalVector_push(&intersection, intersection_interval);
			}
		}
	}
	IntervalsSet result = IntervalsSet_alloc(intersection.size);
	for (size_t i = 0; i < intersection.size; i++) {
		result.intervals[i] = intersection.array[i];
	}
	IntervalVector_destroy(intersection);
	return result;
}

int Interval_starts_before(const void* a, const void* b) {
	Interval* interval_a = (Interval*)a;
	Interval* interval_b = (Interval*)b;
	return interval_a->start - interval_b->start;
}

void IntervalsSet_sort(IntervalsSet* intervals_set) {
	qsort(intervals_set->intervals, intervals_set->nb_intervals, sizeof(Interval), Interval_starts_before);
}

// TODO: Not very efficient either, but i prefer to focus on actually important stuff and come back to this later
IntervalsSet IntervalsSet_union(IntervalsSet a, IntervalsSet b) {
	IntervalVector union_intervals = IntervalVector_with_capacity(a.nb_intervals + b.nb_intervals);
	for (size_t i = 0; i < a.nb_intervals; i++) {
		IntervalVector_push(&union_intervals, a.intervals[i]);
	}
	for (size_t i = 0; i < b.nb_intervals; i++) {
		IntervalVector_push(&union_intervals, b.intervals[i]);
	}
	IntervalsSet result = IntervalsSet_alloc(union_intervals.size);
	for (size_t i = 0; i < union_intervals.size; i++) {
		result.intervals[i] = union_intervals.array[i];
	}
	IntervalsSet_sort(&result);
	IntervalsSet_merge(&result);
	IntervalVector_destroy(union_intervals);
	return result;
}

Interval IntervalsSet_last(IntervalsSet* intervals_set) {
	return intervals_set->intervals[intervals_set->nb_intervals - 1];
}

bool IntervalsSet_contains(IntervalsSet intervals_set, TimeId time) {
	for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
		if (Interval_contains(intervals_set.intervals[i], time)) {
			return true;
		}
	}
	return false;
}