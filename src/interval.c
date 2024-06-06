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

char* interval_to_string(Interval interval) {
	char* str = malloc(100);
	sprintf(str, "[%f, %f]", interval.start, interval.end);
	return str;
}