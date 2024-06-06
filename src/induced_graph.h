#ifndef INDUCED_GRAPH_H
#define INDUCED_GRAPH_H

#include "stream_graph.h"
#include "units.h"
#include "vector.h"

typedef struct {
	Time time;
	TemporalNodeRefVector nodes;
	LinkRefVector links;
} InstantInducedGraph;

InstantInducedGraph induced_graph_at_time(StreamGraph* graph, Time time);

typedef struct {
	TemporalNodeRefVector nodes;
	LinkRefVector links;
} InducedGraph;

InducedGraph induced_graph(StreamGraph* graph);

char* InducedGraph_to_string(InducedGraph* induced);

char* InstantInducedGraph_to_string(InstantInducedGraph* induced);

#endif // INDUCED_GRAPH_H
