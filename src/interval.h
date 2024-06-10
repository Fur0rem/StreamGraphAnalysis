#include "units.h"
#include <stdbool.h>

typedef struct {
	TimeId start;
	TimeId end;
} Interval;

bool Interval_contains(Interval interval, TimeId time);
size_t Interval_size(Interval interval);
Interval Interval_from(TimeId start, TimeId end);
Interval Interval_intersection(Interval a, Interval b);