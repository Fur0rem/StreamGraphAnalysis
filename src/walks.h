#ifndef WALK_H
#define WALK_H

#include "interval.h"
#include "stream.h"
#include "units.h"
#include "utils.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>

// Forward declaration of TreeNode since it's used in TreeEdge before its full definition.
typedef struct TreeNode TreeNode;

typedef struct {
	size_t weight;
	TreeNode* child; // Use a pointer to TreeNode for the forward-declared type.
} TreeEdge;

// Function declarations that use TreeEdge.
bool TreeEdge_equals(TreeEdge a, TreeEdge b);
char* TreeEdge_to_string(TreeEdge* edge);

// Assuming DefVector is a macro for defining a vector type for TreeEdge,
// and NO_FREE is a macro used within that definition.
DefVector(TreeEdge, NO_FREE(TreeEdge));

// Now fully define TreeNode, after TreeEdge has been defined.
struct TreeNode {
	size_t id;
	TreeEdgeVector children; // Assuming TreeEdgeVector is defined by DefVector macro.
};

TreeNode TreeNode_with_id(size_t id);
void TreeNode_push_child(TreeNode* node, TreeEdge child);
TreeNode exploration_tree_from_stream(Stream* stream);

typedef struct {
	LinkId link;
	size_t time;
	size_t needs_to_arrive_before;
} WalkStep;

char* WalkStep_to_string(WalkStep* step);
bool WalkStep_equals(WalkStep a, WalkStep b);

DefVector(WalkStep, NO_FREE(WalkStep));

typedef struct {
	NodeId start;
	NodeId end;
	TimeId start_time;
	Stream* stream;
	WalkStepVector steps;
} Walk;

Walk Stream_shortest_walk_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at);
// Walk Stream_fastest_shortest_walk(Stream* stream, NodeId from, NodeId to, TimeId at);
char* Walk_to_string(Walk* walk);
// Interval Walk_is_still_optimal_between(Walk* walk);

// typedef struct {
// 	Walk walk;
// 	Interval optimality;
// } WalkOptimal;

// char* WalkOptimal_to_string(WalkOptimal* wo);

// bool Walk_equals(Walk a, Walk b);

// bool WalkOptimal_equals(WalkOptimal a, WalkOptimal b);

// DefVector(WalkOptimal, NO_FREE(WalkOptimal));

// WalkOptimalVector optimals_between_two_nodes(Stream* stream, NodeId from, NodeId to);

#endif // WALK_H