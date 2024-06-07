#ifndef INTERVAL_H
#define INTERVAL_H

#include "units.h"
#include "utils.h"
#include "vector.h"
#include <stdbool.h>
#include <stdio.h>

// typedef enum {
//     DISCRETE,
//     CONTINUOUS
// } TimeMeasurement;

// typedef struct {
//     union {
//         struct {
//             int start;
//             int end;
//         } discrete;
//         struct {
//             time start;
//             time end;
//         } continuous;
//     } value;
//     TimeMeasurement measurement;
// } Interval;

typedef struct {
	Time start;
	Time end;
} Interval;

Time interval_size(Interval interval);
bool interval_contains(Interval interval, Time value);
Interval* interval_ptr_from(Time start, Time end);
DeclareGenerics(Interval);
DefVector(Interval, NO_FREE(Interval));

Interval interval_from(Time start, Time end);

#endif