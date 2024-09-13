#include "interval.h"
#include "defaults.h"
#include "generic_data_structures/vector.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

bool Interval_contains(Interval interval, TimeId time) {
	return interval.start <= time && time < interval.end;
}

bool Interval_contains_interval(Interval container, Interval contained) {
	return container.start <= contained.start && contained.end <= container.end;
}

size_t Interval_duration(Interval interval) {
	if (interval.start > interval.end) {
		return 0;
	}
	return interval.end - interval.start;
}

Interval Interval_from(TimeId start, TimeId end) {
	Interval interval;
	interval.start = start;
	interval.end   = end;
	return interval;
}

Interval Interval_intersection(Interval left, Interval right) {
	Interval intersection;
	intersection.start = left.start > right.start ? left.start : right.start;
	intersection.end   = left.end < right.end ? left.end : right.end;
	return intersection;
}

size_t IntervalsSet_size(IntervalsSet intervals_set) {
	size_t size = 0;
	for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
		size += Interval_duration(intervals_set.intervals[i]);
	}
	return size;
}

IntervalsSet IntervalsSet_alloc(size_t nb_intervals) {
	IntervalsSet set;
	set.nb_intervals = nb_intervals;
	if (nb_intervals == 0) {
		set.intervals = NULL;
	}
	else {
		set.intervals = MALLOC(nb_intervals * sizeof(Interval));
	}
	return set;
}

String Interval_to_string(const Interval* interval) {
	String string = String_from_duplicate("");
	String_append_formatted(&string, "[%lu, %lu[", interval->start, interval->end);
	return string;
}

bool Interval_equals(const Interval* left, const Interval* right) {
	return left->start == right->start && left->end == right->end;
}

DefineVector(Interval);
DefineVectorDeriveRemove(Interval, NO_FREE(Interval));
DefineVectorDeriveEquals(Interval);
DefineVectorDeriveToString(Interval);

void IntervalsSet_destroy(IntervalsSet intervals_set) {
	free(intervals_set.intervals);
}

// Puts together all the intervals that are contiguous or overlapping
// Assumes the intervals are sorted
// TODO: Could probably be optimized
void IntervalsSet_merge(IntervalsSet* intervals_set) {
	IntervalVector merged = IntervalVector_with_capacity(intervals_set->nb_intervals);
	Interval current	  = intervals_set->intervals[0];
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

// TODO: Not very efficient, but it works for now (O(n^2)) when we could iterate over the intervals in
// parallel since they are sorted
IntervalsSet IntervalsSet_intersection(IntervalsSet left, IntervalsSet right) {
	IntervalVector intersection = IntervalVector_with_capacity(left.nb_intervals + right.nb_intervals);
	for (size_t i = 0; i < left.nb_intervals; i++) {
		for (size_t j = 0; j < right.nb_intervals; j++) {
			Interval a_interval			   = left.intervals[i];
			Interval b_interval			   = right.intervals[j];
			Interval intersection_interval = Interval_intersection(a_interval, b_interval);
			if (Interval_duration(intersection_interval) > 0) {
				IntervalVector_push(&intersection, intersection_interval);
			}
		}
	}
	if (intersection.size == 0) {
		return (IntervalsSet){
			.nb_intervals = 0,
			.intervals	  = NULL,
		};
	}

	IntervalsSet result = IntervalsSet_alloc(intersection.size);
	for (size_t i = 0; i < intersection.size; i++) {
		result.intervals[i] = intersection.array[i];
	}
	IntervalVector_destroy(intersection);
	return result;
}

bool is_sorted(const Interval* intervals, size_t nb_intervals) {
	for (size_t i = 1; i < nb_intervals; i++) {
		if (intervals[i - 1].start > intervals[i].start) {
			return false;
		}
	}
	return true;
}

void IntervalsSet_self_intersection_with_single(IntervalsSet* intervals_set, Interval interval) {
	for (size_t i = 0; i < intervals_set->nb_intervals; i++) {
		Interval intersection_interval = Interval_intersection(intervals_set->intervals[i], interval);
		if (Interval_duration(intersection_interval) > 0) {
			intervals_set->intervals[i] = intersection_interval;
		}
		else {
			intervals_set->intervals[i] = Interval_empty();
		}
	}

	// swap empty intervals to the end
	size_t empty_intervals = 0;
	for (size_t i = 0; i < intervals_set->nb_intervals; i++) {
		if (Interval_is_empty(intervals_set->intervals[i])) {
			empty_intervals++;
		}
		else {
			if (empty_intervals > 0) {
				intervals_set->intervals[i - empty_intervals] = intervals_set->intervals[i];
				intervals_set->intervals[i]					  = Interval_empty();
			}
		}
	}
	intervals_set->nb_intervals -= empty_intervals;
}

IntervalsSet IntervalsSet_intersection_with_single(IntervalsSet a, Interval b) {
	IntervalVector intersection = IntervalVector_with_capacity(a.nb_intervals);
	for (size_t i = 0; i < a.nb_intervals; i++) {
		Interval a_interval			   = a.intervals[i];
		Interval intersection_interval = Interval_intersection(a_interval, b);
		if (Interval_duration(intersection_interval) > 0) {
			IntervalVector_push(&intersection, intersection_interval);
		}
	}
	if (intersection.size == 0) {
		return (IntervalsSet){
			.nb_intervals = 0,
			.intervals	  = NULL,
		};
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

// TODO: Not very efficient either, but i prefer to focus on actually important stuff and come back to this
// later
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

bool IntervalsSet_contains_sorted(IntervalsSet intervals_set, TimeId time) {
	// for (size_t i = 0; i < intervals_set.nb_intervals; i++) {
	// 	if (intervals_set.intervals[i].start > time) {
	// 		return false;
	// 	}
	// 	if (Interval_contains(intervals_set.intervals[i], time)) {
	// 		return true;
	// 	}
	// }
	// return false;

	size_t left	 = 0;
	size_t right = intervals_set.nb_intervals;
	while (left < right) {
		size_t mid = left + (right - left) / 2;
		if (Interval_contains(intervals_set.intervals[mid], time)) {
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

Interval Interval_empty() {
	Interval interval = {SIZE_MAX, 0};
	return interval;
}

bool Interval_is_empty(Interval interval) {
	return interval.start >= interval.end;
}

bool Interval_overlaps_interval(Interval a, Interval b) {
	return a.start < b.end && b.start < a.end;
}

Interval Interval_minus(Interval a, Interval b) {
	if (a.start >= b.end || b.start >= a.end) {
		return a;
	}
	if (a.start < b.start && a.end > b.end) {
		Interval one = Interval_from(a.start, b.start);
		Interval two = Interval_from(b.end, a.end);
		ASSERT(Interval_is_empty(one) || Interval_is_empty(two));
		if (Interval_is_empty(one)) {
			return two;
		}
		else {
			return one;
		}
	}
	if (a.start < b.start) {
		return Interval_from(a.start, b.start);
	}
	if (a.end > b.end) {
		return Interval_from(b.end, a.end);
	}
	return Interval_empty();
}

void IntervalsSet_add_at(IntervalsSet* intervals_set, Interval interval, size_t index) {
	intervals_set->intervals[index] = interval;
}

IntervalsSet IntervalsSet_from_interval_vector(IntervalVector intervals) {
	return (IntervalsSet){
		.nb_intervals = intervals.size,
		.intervals	  = intervals.array,
	};
}

IntervalVector IntervalVector_from_intervals_set(IntervalsSet intervals_set) {
	return (IntervalVector){
		.size	  = intervals_set.nb_intervals,
		.capacity = intervals_set.nb_intervals,
		.array	  = intervals_set.intervals,
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

SGA_Offset IntervalVector_offset_of(const IntervalVector* self, const IntervalVector* other) {
	if (self->size != other->size) {
		return SGA_Offset_does_not_match();
	}
	if (self->size == 0) {
		return SGA_Offset_empty();
	}
	size_t offset = other->array[0].start - self->array[0].start;
	for (size_t i = 1; i < self->size; i++) {
		// printf("offset = %lu\n", offset);
		if ((other->array[i].start - self->array[i].start != offset) ||
			(other->array[i].end - self->array[i].end != offset)) {
			// printf("offset mismatch, found %zu || %zu\n", other->array[i].start - self->array[i].start,
			//    other->array[i].end - self->array[i].end);
			return SGA_Offset_does_not_match();
		}
	}
	// printf("we gud offset = %lu\n", offset);
	return SGA_Offset_ok(offset);
}

SGA_Offset Interval_offset_of(const Interval* self, const Interval* other) {

	bool self_is_empty	= Interval_is_empty(*self);
	bool other_is_empty = Interval_is_empty(*other);
	if (self_is_empty && other_is_empty) {
		return SGA_Offset_empty();
	}
	// If one is empty and the other is not
	else if (self_is_empty ^ other_is_empty) {
		return SGA_Offset_does_not_match();
	}

	size_t offset_start = other->start - self->start;
	size_t offset_end	= other->end - self->end;
	printf("self = [%lu, %lu[, other = [%lu, %lu[\n", self->start, self->end, other->start, other->end);
	printf("offset_start = %zu, offset_end = %zu\n", offset_start, offset_end);

	if (offset_start != offset_end) {
		return SGA_Offset_does_not_match();
	}
	return SGA_Offset_ok(offset_start);
}