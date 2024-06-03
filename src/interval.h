#ifndef INTERVAL_H
#define INTERVAL_H

#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include "units.h"
#include "vector.h"

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

DefVector(Interval)

#endif