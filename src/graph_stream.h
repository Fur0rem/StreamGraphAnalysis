#ifndef GRAPH_STREAM_H
#define GRAPH_STREAM_H

#include "interval.h"

// Node in a graph
typedef struct {
    IntervalVector present_at;
    const char* label;
} Node;

// Undirected edge
typedef struct {
    IntervalVector present_at;
    const size_t node1;
    const size_t node2;
} Edge;

#endif // GRAPH_STREAM_H