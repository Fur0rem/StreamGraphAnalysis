#include "interval.h"

Time interval_size(Interval interval) {
    return interval.end - interval.start;
}

bool interval_contains(Interval interval, Time value) {
    return interval.start <= value && value <= interval.end;
}

typedef struct {
    Time start;
    Time end;
} Interval_t;