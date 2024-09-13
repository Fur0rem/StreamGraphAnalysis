#ifndef WALK_H
#define WALK_H

#include "../generic_data_structures/vector.h"
#include "../interval.h"
#include "../stream.h"
#include "../units.h"
#include "../utils.h"
#include <stdbool.h>
#include <stddef.h>

// Forward declaration of TreeNode since it's used in TreeEdge before its full definition.
typedef struct TreeNode TreeNode;

typedef struct {
	size_t weight;
	TreeNode* child; // Use a pointer to TreeNode for the forward-declared type.
} TreeEdge;

// Function declarations that use TreeEdge.
DeclareToString(TreeEdge);
DeclareEquals(TreeEdge);

DeclareVector(TreeEdge);
DeclareVectorDeriveEquals(TreeEdge);
DeclareVectorDeriveToString(TreeEdge);
DeclareVectorDeriveRemove(TreeEdge);

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
	Interval interval_taken;
} WalkStep;

DeclareToString(WalkStep);
DeclareEquals(WalkStep);

DeclareVector(WalkStep);
DeclareVectorDeriveEquals(WalkStep);
DeclareVectorDeriveToString(WalkStep);
DeclareVectorDeriveRemove(WalkStep);

typedef struct {
	NodeId start;
	NodeId end;
	Interval optimality;
	Stream* stream;
	WalkStepVector steps;
} Walk;

typedef struct {
	union {
		Interval node_absent_between;
		TimeId unreachable_after;
	} reason;
	enum {
		NODE_ABSENT,
		UNREACHABLE
	} type;
} NoWalkReason;

typedef struct {
	union {
		Walk walk;
		NoWalkReason no_walk_reason;
	} result;
	enum {
		WALK,
		NO_WALK
	} type;
} WalkInfo;

WalkInfo Stream_shortest_walk_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at);
WalkInfo Stream_fastest_shortest_walk(Stream* stream, NodeId from, NodeId to, TimeId at);
DeclareToString(Walk);
DeclareEquals(Walk);
DeclareToString(WalkInfo);
DeclareEquals(WalkInfo);
void Walk_destroy(Walk walk);
void WalkInfo_destroy(WalkInfo wi);
DeclareVector(WalkInfo);
DeclareVectorDeriveEquals(WalkInfo);
DeclareVectorDeriveToString(WalkInfo);
DeclareVectorDeriveRemove(WalkInfo);

WalkInfoVector optimal_walks_between_two_nodes(Stream* stream, NodeId from, NodeId to,
											   WalkInfo (*fn)(Stream*, NodeId, NodeId, TimeId));
WalkInfo Stream_fastest_walk(Stream* stream, NodeId from, NodeId to, TimeId at);

size_t Walk_length(Walk* walk);
size_t Walk_duration(Walk* walk);

double Stream_robustness_by_length(Stream* stream);

bool Walk_goes_through(Walk* walk, Stream stream, size_t nb_steps, ...);

Walk WalkInfo_unwrap_checked(WalkInfo info);
Walk WalkInfo_unwrap_unchecked(WalkInfo info);

TimeId Walk_arrives_at(Walk* walk);

double Stream_robustness_by_duration(Stream* stream);

#endif // WALK_H