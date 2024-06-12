#include "interval.h"

bool Interval_contains(Interval interval, TimeId time) {
	return interval.start <= time && time <= interval.end;
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