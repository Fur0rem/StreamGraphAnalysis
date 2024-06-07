#include "interval.h"

Time interval_size(Interval interval) {
	return interval.end - interval.start;
}

bool interval_contains(Interval interval, Time value) {
	return interval.start <= value && value <= interval.end;
}

Interval interval_from(Time start, Time end) {
	Interval interval = {.start = start, .end = end};
	return interval;
}

Interval* interval_ptr_from(Time start, Time end) {
	Interval* interval = malloc(sizeof(Interval));
	interval->start = start;
	interval->end = end;
	return interval;
}

char* Interval_to_string(Interval* interval) {
	char* str = malloc(100);
	sprintf(str, "[%f, %f]", interval->start, interval->end);
	return str;
}

bool Interval_equals(Interval a, Interval b) {
	return F_EQUALS(a.start, b.start) && F_EQUALS(a.end, b.end);
}