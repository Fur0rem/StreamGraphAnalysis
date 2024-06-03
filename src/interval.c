#include "interval.h"

double interval_size(Interval interval) {
    return interval.end - interval.start;
}

bool interval_contains(Interval interval, double value) {
    return interval.start <= value && value <= interval.end;
}