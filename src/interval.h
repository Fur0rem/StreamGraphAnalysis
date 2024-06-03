#ifndef INTERVAL_H
#define INTERVAL_H

#include <stdbool.h>

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
//             double start;
//             double end;
//         } continuous;
//     } value;
//     TimeMeasurement measurement;
// } Interval;

typedef struct {
    double start;
    double end;
} Interval;

double interval_size(Interval interval);
bool interval_contains(Interval interval, double value);

#endif