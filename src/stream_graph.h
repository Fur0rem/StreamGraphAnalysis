#ifndef STREAM_GRAPH_H
#define STREAM_GRAPH_H

#include "interval.h"
#include "utils.h"
#include "vector.h"

DeclareGenerics(Interval);
DefVector(Interval, NO_FREE(Interval));

typedef struct TemporalNode TemporalNode;
// Undirected link
typedef struct {
	IntervalVector present_at;
	TemporalNode* nodes[2];
	// Time weight;
} Link;
DeclareGenerics(Link);
DefVector(Link, NO_FREE(Link));

typedef Link* LinkRef;
DeclareGenerics(LinkRef);
DefVector(LinkRef, NO_FREE(LinkRef));

// Node in a graph
struct TemporalNode {
	IntervalVector present_at;
	const char* label;
	LinkRefVector links;
};
DeclareGenerics(TemporalNode);
DefVector(TemporalNode, NO_FREE(TemporalNode));

typedef struct {
	Interval lifespan;
	TemporalNodeVector temporal_nodes;
	LinkVector links;
} StreamGraph;

bool is_node_present_at(TemporalNode node, Time time);
bool is_link_present_at(Link link, Time time);
Link* find_link(LinkVector links, TemporalNode* node1, TemporalNode* node2);
bool are_nodes_linked_at(StreamGraph* graph, TemporalNode* node1, TemporalNode* node2, Time time);

StreamGraph stream_graph_from(Interval lifespan, TemporalNodeVector nodes, LinkVector links);

typedef TemporalNode* TemporalNodeRef;
DeclareGenerics(TemporalNodeRef);
DefVector(TemporalNodeRef, NO_FREE(TemporalNodeRef));

TemporalNodeRefVector get_nodes_present_at(StreamGraph* graph, Time time);
LinkRefVector get_links_present_at(StreamGraph* graph, Time time);

char* StreamGraph_to_string(StreamGraph* graph);
TemporalNode TemporalNode_new(const char* label, IntervalVector present_at);

#endif // STREAM_GRAPH_H