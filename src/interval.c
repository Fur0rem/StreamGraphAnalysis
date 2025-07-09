#include "interval.h"
#include "generic_data_structures/arraylist.h"
#include "units.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

bool SGA_Interval_contains(SGA_Interval interval, SGA_Time time) {
	return interval.start <= time && time < interval.end;
}

bool SGA_Interval_contains_interval(SGA_Interval container, SGA_Interval contained) {
	return container.start <= contained.start && contained.end <= container.end;
}

size_t SGA_Interval_duration(SGA_Interval interval) {
	if (interval.start > interval.end) {
		return 0;
	}
	return interval.end - interval.start;
}

SGA_Interval SGA_Interval_from(SGA_Time start, SGA_Time end) {
	SGA_Interval interval;
	interval.start = start;
	interval.end   = end;
	return interval;
}

SGA_Interval SGA_Interval_intersection(SGA_Interval left, SGA_Interval right) {
	SGA_Interval intersection;
	intersection.start = left.start > right.start ? left.start : right.start;
	intersection.end   = left.end < right.end ? left.end : right.end;
	return intersection;
}

size_t SGA_IntervalsSet_size(SGA_IntervalsSet intervals_set) {
	size_t size = 0;
	for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
		size += SGA_Interval_duration(intervals_set.intervals[i]);
	}
	return size;
}

SGA_IntervalsSet SGA_IntervalsSet_alloc(size_t nb_intervals) {
	SGA_IntervalsSet set;
	set.nb_intervals = nb_intervals;
	if (nb_intervals == 0) {
		set.intervals = NULL;
	}
	else {
		set.intervals = MALLOC(nb_intervals * sizeof(SGA_Interval));
	}
	return set;
}

String SGA_Interval_to_string(const SGA_Interval* interval) {
	String string = String_from_duplicate("");
	String_append_formatted(&string, "[%lu, %lu[", interval->start, interval->end);
	return string;
}

bool SGA_Interval_equals(const SGA_Interval* left, const SGA_Interval* right) {
	return left->start == right->start && left->end == right->end;
}

DefineArrayList(SGA_Interval);
NO_FREE(SGA_Interval);
DefineArrayListDeriveRemove(SGA_Interval);
DefineArrayListDeriveEquals(SGA_Interval);
DefineArrayListDeriveToString(SGA_Interval);

void SGA_IntervalsSet_destroy(SGA_IntervalsSet intervals_set) {
	free(intervals_set.intervals);
}

// Puts together all the intervals that are contiguous or overlapping
// Assumes the intervals are sorted
// TODO: Could probably be optimized
void SGA_IntervalsSet_merge(SGA_IntervalsSet* intervals_set) {
	SGA_IntervalArrayList merged = SGA_IntervalArrayList_with_capacity(intervals_set->nb_intervals);
	SGA_Interval current	     = intervals_set->intervals[0];
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
			SGA_IntervalArrayList_push(&merged, current);
			current = next;
		}
	}
	SGA_IntervalArrayList_push(&merged, current);
	SGA_IntervalsSet_destroy(*intervals_set);
	*intervals_set = SGA_IntervalsSet_alloc(merged.length);
	for (size_t i = 0; i < merged.length; i++) {
		intervals_set->intervals[i] = merged.array[i];
	}
	SGA_IntervalArrayList_destroy(merged);
}

// TODO: Not very efficient, but it works for now (O(n^2)) when we could iterate over the intervals in
// parallel since they are sorted
SGA_IntervalsSet SGA_IntervalsSet_intersection(SGA_IntervalsSet left, SGA_IntervalsSet right) {
	SGA_IntervalArrayList intersection = SGA_IntervalArrayList_with_capacity(left.nb_intervals + right.nb_intervals);
	for (size_t i = 0; i < left.nb_intervals; i++) {
		for (size_t j = 0; j < right.nb_intervals; j++) {
			SGA_Interval a_interval		   = left.intervals[i];
			SGA_Interval b_interval		   = right.intervals[j];
			SGA_Interval intersection_interval = SGA_Interval_intersection(a_interval, b_interval);
			if (SGA_Interval_duration(intersection_interval) > 0) {
				SGA_IntervalArrayList_push(&intersection, intersection_interval);
			}
		}
	}
	if (intersection.length == 0) {
		SGA_IntervalArrayList_destroy(intersection);
		return (SGA_IntervalsSet){
		    .nb_intervals = 0,
		    .intervals	  = NULL,
		};
	}

	SGA_IntervalsSet result = SGA_IntervalsSet_alloc(intersection.length);
	for (size_t i = 0; i < intersection.length; i++) {
		result.intervals[i] = intersection.array[i];
	}
	SGA_IntervalArrayList_destroy(intersection);
	return result;
}

// Doesn't consume the input
SGA_IntervalArrayList SGA_IntervalArrayList_intersection(SGA_IntervalArrayList* left, SGA_IntervalArrayList* right) {
	SGA_IntervalArrayList intersection =
	    SGA_IntervalArrayList_with_capacity((left->length > right->length) ? left->length : right->length);
	size_t min_at_encountered = 0;
	for (size_t i = 0; i < left->length; i++) {
		size_t start	   = min_at_encountered; // we keep track of when at the previous iteration we started finding intersections
		min_at_encountered = SIZE_MAX;
		for (size_t j = start; j < right->length; j++) {
			SGA_Interval a_interval = left->array[i];
			SGA_Interval b_interval = right->array[j];
			if (a_interval.end <= b_interval.start) {
				break;
			}
			SGA_Interval intersection_interval = SGA_Interval_intersection(a_interval, b_interval);
			if (SGA_Interval_duration(intersection_interval) > 0) {
				SGA_IntervalArrayList_push(&intersection, intersection_interval);
				if (min_at_encountered == SIZE_MAX) {
					min_at_encountered = j;
				}
			}
		}
	}

	return intersection;
}

bool is_sorted(const SGA_Interval* intervals, size_t nb_intervals) {
	for (size_t i = 1; i < nb_intervals; i++) {
		if (intervals[i - 1].start > intervals[i].start) {
			return false;
		}
	}
	return true;
}

void SGA_IntervalsSet_self_intersection_with_single(SGA_IntervalsSet* intervals_set, SGA_Interval interval) {
	size_t nb_empty_intervals = 0;
	for (size_t i = 0; i < intervals_set->nb_intervals; i++) {
		SGA_Interval intersection_interval = SGA_Interval_intersection(intervals_set->intervals[i], interval);
		/*if (SGA_Interval_duration(intersection_interval) > 0) {
			intervals_set->intervals[i] = intersection_interval;
		}
		else {
			intervals_set->intervals[i] = SGA_Interval_empty();
		}*/
		if (SGA_Interval_duration(intersection_interval) > 0) {
			intervals_set->intervals[i - nb_empty_intervals] = intersection_interval;
		}
		else {
			nb_empty_intervals++;
		}
	}

	intervals_set->nb_intervals -= nb_empty_intervals;
}

SGA_IntervalsSet SGA_IntervalsSet_intersection_with_single(SGA_IntervalsSet a, SGA_Interval b) {
	SGA_IntervalsSet result = SGA_IntervalsSet_alloc(a.nb_intervals);
	memcpy(result.intervals, a.intervals, a.nb_intervals * sizeof(SGA_Interval));
	SGA_IntervalsSet_self_intersection_with_single(&result, b);
	return result;
}

int SGA_Interval_starts_before(const void* a, const void* b) {
	SGA_Interval* interval_a = (SGA_Interval*)a;
	SGA_Interval* interval_b = (SGA_Interval*)b;
	return interval_a->start - interval_b->start;
}

void SGA_IntervalsSet_sort(SGA_IntervalsSet* intervals_set) {
	qsort(intervals_set->intervals, intervals_set->nb_intervals, sizeof(SGA_Interval), SGA_Interval_starts_before);
}

// TODO: Not very efficient either, but i prefer to focus on actually important stuff and come back to this
// later
SGA_IntervalsSet SGA_IntervalsSet_union(SGA_IntervalsSet a, SGA_IntervalsSet b) {
	SGA_IntervalArrayList union_intervals = SGA_IntervalArrayList_with_capacity(a.nb_intervals + b.nb_intervals);
	for (size_t i = 0; i < a.nb_intervals; i++) {
		SGA_IntervalArrayList_push(&union_intervals, a.intervals[i]);
	}
	for (size_t i = 0; i < b.nb_intervals; i++) {
		SGA_IntervalArrayList_push(&union_intervals, b.intervals[i]);
	}
	SGA_IntervalsSet result = SGA_IntervalsSet_alloc(union_intervals.length);
	for (size_t i = 0; i < union_intervals.length; i++) {
		result.intervals[i] = union_intervals.array[i];
	}
	SGA_IntervalsSet_sort(&result);
	SGA_IntervalsSet_merge(&result);
	SGA_IntervalArrayList_destroy(union_intervals);
	return result;
}

SGA_Interval SGA_IntervalsSet_last(SGA_IntervalsSet* intervals_set) {
	return intervals_set->intervals[intervals_set->nb_intervals - 1];
}

bool SGA_IntervalsSet_contains(SGA_IntervalsSet intervals_set, SGA_Time time) {
	for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
		if (SGA_Interval_contains(intervals_set.intervals[i], time)) {
			return true;
		}
	}
	return false;
}

bool SGA_IntervalsSet_contains_sorted(SGA_IntervalsSet intervals_set, SGA_Time time) {
	// for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
	// 	if (intervals_set.intervals[i].start > time) {
	// 		return false;
	// 	}
	// 	if (SGA_Interval_contains(intervals_set.intervals[i], time)) {
	// 		return true;
	// 	}
	// }
	// return false;

	size_t left  = 0;
	size_t right = intervals_set.nb_intervals;
	while (left < right) {
		size_t mid = left + (right - left) / 2;
		if (SGA_Interval_contains(intervals_set.intervals[mid], time)) {
			return true;
		}
		if (intervals_set.intervals[mid].start > time) {
			right = mid;
		}
		else {
			left = mid + 1;
		}
	}
	return false;
}

SGA_Interval SGA_Interval_empty() {
	SGA_Interval interval = {SIZE_MAX, 0};
	return interval;
}

bool SGA_Interval_is_empty(SGA_Interval interval) {
	return interval.start >= interval.end;
}

bool SGA_Interval_overlaps_interval(SGA_Interval a, SGA_Interval b) {
	return a.start < b.end && b.start < a.end;
}

SGA_Interval SGA_Interval_minus(SGA_Interval a, SGA_Interval b) {
	if (a.start >= b.end || b.start >= a.end) {
		return a;
	}
	if (a.start < b.start && a.end > b.end) {
		SGA_Interval one = SGA_Interval_from(a.start, b.start);
		SGA_Interval two = SGA_Interval_from(b.end, a.end);
		ASSERT(SGA_Interval_is_empty(one) || SGA_Interval_is_empty(two));
		if (SGA_Interval_is_empty(one)) {
			return two;
		}
		else {
			return one;
		}
	}
	if (a.start < b.start) {
		return SGA_Interval_from(a.start, b.start);
	}
	if (a.end > b.end) {
		return SGA_Interval_from(b.end, a.end);
	}
	return SGA_Interval_empty();
}

void SGA_IntervalsSet_add_at(SGA_IntervalsSet* intervals_set, SGA_Interval interval, size_t index) {
	intervals_set->intervals[index] = interval;
}

SGA_IntervalsSet SGA_IntervalsSet_from_arraylist(SGA_IntervalArrayList intervals) {
	return (SGA_IntervalsSet){
	    .nb_intervals = intervals.length,
	    .intervals	  = intervals.array,
	};
}

SGA_IntervalArrayList SGA_IntervalArrayList_from_intervals_set(SGA_IntervalsSet intervals_set) {
	return (SGA_IntervalArrayList){
	    .length   = intervals_set.nb_intervals,
	    .capacity = intervals_set.nb_intervals,
	    .array    = intervals_set.intervals,
	};
}

SGA_Offset SGA_Offset_does_not_match() {
	return (SGA_Offset){
	    .result = NO_MATCHING_OFFSET,
	};
}

SGA_Offset SGA_Offset_ok(size_t offset) {
	return (SGA_Offset){
	    .result = OK,
	    .offset = offset,
	};
}

SGA_Offset SGA_Offset_empty() {
	return (SGA_Offset){
	    .result = EMPTY_INTERVALSET,
	};
}

bool SGA_Offset_is_ok(SGA_Offset offset) {
	return offset.result == OK;
}

bool SGA_Offset_is_empty(SGA_Offset offset) {
	return offset.result == EMPTY_INTERVALSET;
}

bool SGA_Offset_is_not_matching(SGA_Offset offset) {
	return offset.result == NO_MATCHING_OFFSET;
}

size_t SGA_Offset_unwrap(SGA_Offset offset) {
	if (SGA_Offset_is_ok(offset)) {
		return offset.offset;
	}
	else {
		// TODO: print stack trace
		ASSERT(false);
		return 0;
	}
}

SGA_Offset SGA_IntervalArrayList_offset_of(const SGA_IntervalArrayList* self, const SGA_IntervalArrayList* other) {
	if (self->length != other->length) {
		return SGA_Offset_does_not_match();
	}
	if (self->length == 0) {
		return SGA_Offset_empty();
	}
	size_t offset = other->array[0].start - self->array[0].start;
	for (size_t i = 1; i < self->length; i++) {
		// printf("offset = %lu\n", offset);
		if ((other->array[i].start - self->array[i].start != offset) || (other->array[i].end - self->array[i].end != offset)) {
			// printf("offset mismatch, found %zu || %zu\n", other->array[i].start - self->array[i].start,
			//    other->array[i].end - self->array[i].end);
			return SGA_Offset_does_not_match();
		}
	}
	// printf("we gud offset = %lu\n", offset);
	return SGA_Offset_ok(offset);
}

SGA_Offset SGA_Interval_offset_of(const SGA_Interval* self, const SGA_Interval* other) {

	bool self_is_empty  = SGA_Interval_is_empty(*self);
	bool other_is_empty = SGA_Interval_is_empty(*other);
	if (self_is_empty && other_is_empty) {
		return SGA_Offset_empty();
	}
	// If one is empty and the other is not
	else if (self_is_empty ^ other_is_empty) {
		return SGA_Offset_does_not_match();
	}

	size_t offset_start = other->start - self->start;
	size_t offset_end   = other->end - self->end;
	// printf("self = [%lu, %lu[, other = [%lu, %lu[\n", self->start, self->end, other->start, other->end);
	// printf("offset_start = %zu, offset_end = %zu\n", offset_start, offset_end);

	if (offset_start != offset_end) {
		return SGA_Offset_does_not_match();
	}
	return SGA_Offset_ok(offset_start);
}

String SGA_IntervalsSet_to_string(const SGA_IntervalsSet* self) {
	String str = String_from_duplicate("");
	String_push_str(&str, "{");
	for (size_t i = 0; i < self->nb_intervals; i++) {
		String_concat_consume(&str, SGA_Interval_to_string(&self->intervals[i]));
		if (i != self->nb_intervals - 1) {
			String_push_str(&str, " U ");
		}
	}
	String_push_str(&str, "}");
	return str;
}