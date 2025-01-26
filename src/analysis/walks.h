/**
 * @file src/analysis/walks.h
 * @brief Analysis regarding walks in streams and computations related to it.
 */

#ifndef WALK_H
#define WALK_H

#include "../generic_data_structures/arraylist.h"
#include "../interval.h"
#include "../stream.h"
#include "../units.h"
#include "../utils.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief A step in a Walk
 */
typedef struct {
	SGA_LinkId link;	     ///< Link it went through
	size_t time;		     ///< At which time it arrived on the link
	SGA_Interval interval_taken; ///< The interval of all times at which we can take the next step without compromising the walk
} SGA_WalkStep;

DeclareToString(SGA_WalkStep);
DeclareEquals(SGA_WalkStep);
DeclareArrayList(SGA_WalkStep);
DeclareArrayListDeriveEquals(SGA_WalkStep);
DeclareArrayListDeriveToString(SGA_WalkStep);
DeclareArrayListDeriveRemove(SGA_WalkStep);

/**
 * @brief A walk in a stream
 * A walk is a sequence of steps that goes from a node to another node in a stream.
 * A walk can go through the same node and link multiple times.
 */
typedef struct {
	SGA_NodeId start;	     ///< The node where the walk starts
	SGA_NodeId end;		     ///< The node where the walk ends
	SGA_Interval optimality;     ///< The interval of all times at which the walk is optimal
	SGA_WalkStepArrayList steps; ///< The steps of the walk
	SGA_Stream* stream;	     ///< The stream in which the walk is
} SGA_Walk;

DeclareToString(SGA_Walk);
DeclareEquals(SGA_Walk);
DeclareArrayList(SGA_Walk);
DeclareArrayListDeriveEquals(SGA_Walk);
DeclareArrayListDeriveToString(SGA_Walk);
DeclareArrayListDeriveRemove(SGA_Walk);

/**
 * @brief A reason why a walk is not possible
 * It can be because a node does not exist in the stream at the time of the walk or because the node is simply unreachable.
 */
typedef struct {
	/// The reason why the walk is not possible
	union {
		SGA_Interval node_absent_between; ///< The interval of time at which the node is absent
		SGA_Time unreachable_after;	  ///< The first time at which the node is unreachable
	} reason;
	/// The type of the reason
	enum {
		NODE_ABSENT, ///< The node is absent
		UNREACHABLE  ///< The node is unreachable
	} type;
} SGA_NoWalkReason;

/**
 * @brief May contain a walk or a reason why a walk is not possible
 */
typedef struct {
	/// The walk or the reason why the walk is not possible
	union {
		SGA_Walk walk;			 ///< The walk if it exists
		SGA_NoWalkReason no_walk_reason; ///< The reason why the walk does not exist
	} result;
	/// The type of the result
	enum {
		WALK,	///< The walk exists
		NO_WALK ///< The walk does not exist
	} type;
} SGA_OptionalWalk;

/**
 * @brief Gets the walk contained in an optional walk. Panics if the optional walk does not contain a walk.
 * @param[in] info The optional walk to get the walk from
 * @return The walk contained in the optional walk
 */
SGA_Walk SGA_OptionalWalk_unwrap_checked(SGA_OptionalWalk info);

/**
 * @brief Gets the walk contained in an optional walk. Does not check if the optional walk contains a walk.
 * Produces undefined behavior if the optional walk does not contain a walk.
 * @param[in] info The optional walk to get the walk from
 * @return The walk contained in the optional walk
 */
SGA_Walk SGA_OptionalWalk_unwrap_unchecked(SGA_OptionalWalk info);

/**
 * @brief Calculates the length of a walk
 * The length of a walk is the number of nodes it goes through.
 * @param[in] walk The walk to calculate the length of
 * @return The length of the walk
 */
size_t SGA_Walk_length(SGA_Walk* walk);

/**
 * @brief Calculates the duration of a walk
 * The duration of a walk is the time it takes to go through all the steps of the walk.
 */
size_t SGA_Walk_duration(SGA_Walk* walk);

/**
 * @brief Checks if a walk goes through a node.
 * @param[in] walk The walk to check
 * @param[in] stream The stream in which the walk is
 * @param[in] nb_steps The number of steps to check
 * @param[in] ... The nodes to check if the walk goes through
 * @return True if the walk goes through all the nodes, false otherwise
 */
bool SGA_Walk_goes_through(SGA_Walk* walk, SGA_Stream stream, size_t nb_steps, ...);

/**
 * @brief Gets the time at which a walk arrives at its destination
 * @param walk The walk to get the arrival time of
 * @return The time at which the walk arrives at its destination
 */
SGA_Time SGA_Walk_arrives_at(SGA_Walk* walk);

/**
 * @brief Computes the shortest walk from a node to another node at a given time.
 * A walk is the shortest if it's the one with the least length @see SGA_Walk_length among all walks possible for this query.
 * @param[in] stream The stream in which the walk is
 * @param[in] from The node from which the walk starts
 * @param[in] to The node where the walk ends
 * @param[in] at The time at which the walk starts
 * @return The shortest walk from the node to the other node at the given time if it exists, a reason why it does not exist otherwise.
 */
SGA_OptionalWalk SGA_shortest_walk(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at);

/**
 * @brief Computes the fastest walk from a node to another node at a given time.
 * A walk is the fastest if it's the one with the least duration @see SGA_Walk_duration among all walks possible for this query.
 * The algorithm used make it also the shortest @see SGA_shortest_walkf fastest walk by design.
 * @param[in] stream The stream in which the walk is
 * @param[in] from The node from which the walk starts
 * @param[in] to The node where the walk ends
 * @param[in] at The time at which the walk starts
 * @return The fastest walk from the node to the other node at the given time if it exists, a reason why it does not exist otherwise.
 */
SGA_OptionalWalk SGA_fastest_walk(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at);

/**
 * @brief Computes the fastest of the shortest walks from a node to another node at a given time.
 * A walk is the fastest shortest walk if it's the fastest @see SGA_fastest_walk among all shortest @see SGA_shortest_walk walks possible
 * for this query.
 * @param[in] stream The stream in which the walk is
 * @param[in] from The node from which the walk starts
 * @param[in] to The node where the walk ends
 * @param[in] at The time at which the walk starts
 * @return The fastest and shortest walk from the node to the other node at the given time if it exists, a reason why it does not exist
 * otherwise.
 */
SGA_OptionalWalk Stream_fastest_shortest_walk(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at);

/**
 * @brief Destroys a walk
 * @param[in] walk The walk to destroy
 */
void SGA_Walk_destroy(SGA_Walk walk);

/**
 * @brief Destroys an optional walk
 * @param[in] wi The optional walk to destroy
 */
void SGA_OptionalWalk_destroy(SGA_OptionalWalk wi);

DeclareToString(SGA_OptionalWalk);
DeclareEquals(SGA_OptionalWalk);
DeclareArrayList(SGA_OptionalWalk);
DeclareArrayListDeriveEquals(SGA_OptionalWalk);
DeclareArrayListDeriveToString(SGA_OptionalWalk);
DeclareArrayListDeriveRemove(SGA_OptionalWalk);

/**
 * @brief Computes the optimal walks between two nodes in a stream over the duration of the stream.
 * @param stream The stream in which the walk is
 * @param from The node from which the walk starts
 * @param to The node where the walk ends
 * @param fn The function that computes the optimal walk between two nodes at a given time
 */
SGA_OptionalWalkArrayList SGA_optimal_walks_between_two_nodes(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to,
							      SGA_OptionalWalk (*fn)(SGA_Stream*, SGA_NodeId, SGA_NodeId, SGA_Time));

/**
 * @brief Computes the robustness of a stream by the length of the walks in it.
 * The robustness (by length) of a stream is the average length of all optimal walks between all nodes at all times.
 * It represents how much steps are needed on average to go from a node to another node at a given time in the stream.
 * @param stream The stream to compute the robustness of
 * @return The robustness of the stream by the length of the walks in it
 */
double SGA_Stream_robustness_by_length(SGA_Stream* stream);

/**
 * @brief Computes the robustness of a stream by the duration of the walks in it.
 * The robustness (by duration) of a stream is the average duration of all optimal walks between all nodes at all times.
 * It represents how much time is needed on average to go from a node to another node at a given time in the stream.
 * @param stream The stream to compute the robustness of
 * @return The robustness of the stream by the duration of the walks in it
 */
double SGA_Stream_robustness_by_duration(SGA_Stream* stream);

#endif // WALK_H