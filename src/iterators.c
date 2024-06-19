#include "iterators.h"

size_t total_time_of(TimesIterator times) {
	size_t total_time = 0;
	FOR_EACH_TIME(times, interval) {
		total_time += Interval_size(interval);
	}
	return total_time;
}