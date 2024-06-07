#ifndef STREAM_GRAPH_H
#define STREAM_GRAPH_H

#include "interval.h"
#include "utils.h"
#include "vector.h"

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

typedef TemporalNode* TemporalNodePtr;
DeclareGenerics(TemporalNodePtr);
DefVector(TemporalNodePtr, NO_FREE(TemporalNodePtr));

typedef struct {
	Interval lifespan;
	TemporalNodePtrVector temporal_nodes;
	LinkVector links;
} StreamGraph;

bool is_node_present_at(TemporalNode node, Time time);
bool is_link_present_at(Link link, Time time);
Link* find_link(LinkVector links, TemporalNode* node1, TemporalNode* node2);
bool are_nodes_linked_at(StreamGraph* graph, TemporalNode* node1, TemporalNode* node2, Time time);

StreamGraph stream_graph_from(Interval lifespan, TemporalNodePtrVector nodes, LinkVector links);

typedef TemporalNode* TemporalNodeRef;
DeclareGenerics(TemporalNodeRef);
DefVector(TemporalNodeRef, NO_FREE(TemporalNodeRef));

TemporalNodeRefVector get_nodes_present_at(StreamGraph* graph, Time time);
LinkRefVector get_links_present_at(StreamGraph* graph, Time time);

char* StreamGraph_to_string(StreamGraph* graph);
TemporalNode TemporalNode_new(const char* label, IntervalVector present_at);
TemporalNode* TemporalNodePtr_new(const char* label, IntervalVector present_at);

Link link_from_labels(TemporalNodeVector* nodes_set, const char* label1, const char* label2);

#endif // STREAM_GRAPH_H