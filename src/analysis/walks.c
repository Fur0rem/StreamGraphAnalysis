#define SGA_INTERNAL

#include "walks.h"
#include "../arena.h"
#include "../generic_data_structures/hashset.h"
#include "../interval.h"
#include "../iterators.h"
#include "../stream.h"
#include "../stream_functions.h"

// For STREAM_FUNCS
#include "../generic_data_structures/arraylist.h"
#include "../generic_data_structures/binary_heap.h"
#include "../streams.h"
#include "../units.h"
#include "../utils.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Breadth-first search for shortest walk
typedef struct QueueInfo QueueInfo;
struct QueueInfo {
	SGA_NodeId node;
	SGA_Time time;
	size_t depth;
	size_t previouses;
	SGA_Interval interval_taken;
	QueueInfo* previous;
};

bool QueueInfo_equals(const QueueInfo* a, const QueueInfo* b) {
	return a->node == b->node && a->time == b->time && a->previous == b->previous &&
	       SGA_Interval_equals(&a->interval_taken, &b->interval_taken) && a->depth == b->depth;
}

String QueueInfo_to_string(const QueueInfo* info) {
	String str	      = String_from_duplicate("");
	const size_t BUF_SIZE = 100;
	char buf[BUF_SIZE];
	snprintf(buf, BUF_SIZE, "QueueInfo(node:%zu, time:%zu, interval taken:", info->node, info->time);
	String_push_str(&str, buf);
	String interval_str = SGA_Interval_to_string(&info->interval_taken);
	String_concat_copy(&str, &interval_str);
	String_push_str(&str, ", depth:");
	sprintf(buf, "%zu", info->depth);
	String_push_str(&str, buf);
	return str;
}

String SGA_WalkStep_to_string(const SGA_WalkStep* step) {
	char* str = MALLOC(100);
	sprintf(str,
		"WalkStep(link:%zu, time:%zu, interval taken:%s)",
		step->link,
		step->time,
		SGA_Interval_to_string(&step->interval_taken).data);
	return String_from_owned(str);
}

bool SGA_WalkStep_equals(const SGA_WalkStep* a, const SGA_WalkStep* b) {
	return a->link == b->link && a->time == b->time;
}

// DefArrayList(QueueInfo, NO_FREE(QueueInfo));
DeclareArrayList(QueueInfo);
DefineArrayList(QueueInfo);
NO_FREE(QueueInfo);
DefineArrayListDeriveRemove(QueueInfo);

// TODO: rename those
size_t size_t_min(size_t a, size_t b) {
	return a < b ? a : b;
}

size_t size_t_max(size_t a, size_t b) {
	return a > b ? a : b;
}

typedef struct {
	SGA_NodeId node;
	SGA_Time time;
} ExploredState;

bool ExploredState_equals(ExploredState* a, ExploredState* b) {
	return a->node == b->node && a->time == b->time;
}

String ExploredState_to_string(ExploredState* state) {
	char* str = malloc(100);
	sprintf(str, "ExploredState(node:%zu, time:%zu)", state->node, state->time);
	return String_from_owned(str);
}

size_t ExploredState_hash(ExploredState* state) {
	return state->node * 31 + state->time;
}

// DefHashset(ExploredState, ExploredState_hash, NO_FREE(ExploredState));
DeclareArrayList(ExploredState);
DefineArrayList(ExploredState);
NO_FREE(ExploredState);
DefineArrayListDeriveRemove(ExploredState);
DeclareHashset(ExploredState);
DefineHashset(ExploredState);
DefineHashsetDeriveRemove(ExploredState);

#define APPEND_CONST(str) str, sizeof(str) - 1

String SGA_Walk_to_string(const SGA_Walk* walk) {
	if (walk->steps.length == 0) {
		char* buf = malloc(100);
		sprintf(buf, "No walk from %zu to %zu at %zu\n", walk->start, walk->end, walk->optimality.start);
		String str = String_from_owned(buf);
		return str;
	}
	String str = String_from_duplicate("");
	String_push_str(&str, "Walk from ");
	char buf[100];
	sprintf(buf, "%zu", walk->start);
	String_push_str(&str, buf);
	String_push_str(&str, " to ");
	sprintf(buf, "%zu", walk->end);
	String_push_str(&str, buf);
	String_push_str(&str, " at ");
	sprintf(buf, "%zu", walk->optimality.start);
	String_push_str(&str, buf);
	String time_str = SGA_Interval_to_string(&walk->optimality);
	String_push_str(&str, " | Optimal at ");
	String_concat_consume(&str, time_str);
	String_push(&str, '\n');

	FullStreamGraph* fsg = (FullStreamGraph*)walk->stream->stream_data;
	SGA_StreamGraph sg   = *fsg->underlying_stream_graph;

	for (size_t i = 0; i < walk->steps.length; i++) {
		SGA_WalkStep step = walk->steps.array[i];
		sprintf(buf, "%zu", step.link);
		String_push_str(&str, buf);
		SGA_NodeId from = sg.links.links[step.link].nodes[0];
		SGA_NodeId to	= sg.links.links[step.link].nodes[1];
		sprintf(buf, " (%zu -> %zu)", from, to);
		String_push_str(&str, buf);
		String_push_str(&str, " @ ");
		sprintf(buf, "%zu", step.time);
		String_push_str(&str, buf);
		SGA_Interval interval = step.interval_taken;
		String interval_str   = SGA_Interval_to_string(&interval);
		String_push_str(&str, " | ");
		String_concat_consume(&str, interval_str);
		String_push(&str, '\n');
	}
	return str;
}

bool SGA_Walk_equals(const SGA_Walk* a, const SGA_Walk* b) {
	for (size_t i = 0; i < a->steps.length; i++) {
		if (!SGA_WalkStep_equals(&a->steps.array[i], &b->steps.array[i])) {
			return false;
		}
	}
	return true;
}

DefineArrayList(SGA_Walk);
DefineArrayListDeriveRemove(SGA_Walk);
DefineArrayListDeriveToString(SGA_Walk);
DefineArrayListDeriveEquals(SGA_Walk);

String SGA_OptionalWalk_to_string(const SGA_OptionalWalk* wi) {
	if (wi->type == WALK) {
		String walk_str = SGA_Walk_to_string(&wi->result.walk);
		String str	= String_from_duplicate("SGA_OptionalWalk(WALK, ");
		String_concat_consume(&str, walk_str);
		String_push(&str, ')');
		return str;
	}
	else {
		String str		= String_from_duplicate("SGA_OptionalWalk(NO_WALK, ");
		SGA_NoWalkReason reason = wi->result.no_walk_reason;
		if (reason.type == NODE_ABSENT) {
			String interval_str = SGA_Interval_to_string(&reason.reason.node_absent_between);
			String_push_str(&str, "NODE_ABSENT in interval ");
			String_concat_copy(&str, &interval_str);
		}
		else {
			char buf[100];
			sprintf(buf, "UNREACHABLE after %zu", reason.reason.unreachable_after);
			String_push_str(&str, buf);
		}
		String_push(&str, ')');
		return str;
	}
}

bool SGA_OptionalWalk_equals(const SGA_OptionalWalk* a, const SGA_OptionalWalk* b) {
	if (a->type != b->type) {
		return false;
	}
	if (a->type == WALK) {
		return SGA_Walk_equals(&a->result.walk, &b->result.walk);
	}
	else {
		return a->result.no_walk_reason.type == b->result.no_walk_reason.type;
	}
}

DefineArrayList(SGA_OptionalWalk);
DefineArrayListDeriveRemove(SGA_OptionalWalk);
DefineArrayListDeriveToString(SGA_OptionalWalk);
DefineArrayListDeriveEquals(SGA_OptionalWalk);

SGA_OptionalWalkArrayList SGA_optimal_walks_between_two_nodes(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to,
							      SGA_OptionalWalk (*fn)(SGA_Stream*, SGA_NodeId, SGA_NodeId, SGA_Time)) {
	SGA_OptionalWalkArrayList walks = SGA_OptionalWalkArrayList_with_capacity(1);
	StreamFunctions fns		= STREAM_FUNCS(fns, stream);

	SGA_Interval lifespan = fns.lifespan(stream->stream_data);
	SGA_Time current_time = lifespan.start;

	while (current_time != lifespan.end) {
		// printf("Current time in func: %zu\n", current_time);
		size_t previous_time	 = current_time;
		SGA_OptionalWalk optimal = fn(stream, from, to, current_time);
		SGA_OptionalWalkArrayList_push(&walks, optimal);
		// printf("Optimal walk type: %s\n", SGA_OptionalWalk_to_string(&optimal).data);
		if (optimal.type == NO_WALK) {
			SGA_NoWalkReason error = optimal.result.no_walk_reason;
			if (error.type == NODE_ABSENT) {
				current_time = error.reason.node_absent_between.end;
			}
			else if (error.type == UNREACHABLE) {
				return walks;
			}
		}
		else if (optimal.type == WALK) {
			// printf("Optimal walk found : %s\n",
			// Walk_to_string(&optimal.result.walk).data);
			SGA_Interval optimality = optimal.result.walk.optimality;
			// printf("Optimality: %zu -> %zu\n", optimality.start, optimality.end);
			ASSERT(optimality.start <= optimality.end);
			current_time = optimality.end;
		}
		if (current_time == previous_time) {
			current_time++;
		}
	}
	return walks;
}

void SGA_OptionalWalk_destroy(SGA_OptionalWalk wi) {
	if (wi.type == WALK) {
		SGA_Walk_destroy(wi.result.walk);
	}
}

// TODO : put it in the right place
DefineArrayList(SGA_WalkStep);
NO_FREE(SGA_WalkStep);
DefineArrayListDeriveRemove(SGA_WalkStep);
DefineArrayListDeriveToString(SGA_WalkStep);

/**
 * @brief Creates a walk from the recorded steps
 * @param[in] stream The stream in which the walk is
 * @param[in] candidates The recorded steps, starting from the last node
 * @param[in] from The node from which the walk starts
 */
SGA_WalkStepArrayList SGA_WalkStepArrayList_from_candidates(SGA_Stream* stream, QueueInfo* candidates, SGA_NodeId from) {
	QueueInfo* current_walk	    = candidates;
	SGA_WalkStepArrayList steps = SGA_WalkStepArrayList_with_capacity(candidates->previouses);
	StreamFunctions fns	    = STREAM_FUNCS(fns, stream);

	while (current_walk != NULL) {
		SGA_NodeId current_node	 = current_walk->node;
		QueueInfo* previous	 = current_walk->previous;
		SGA_NodeId previous_node = previous == NULL ? from : previous->node;

		if (current_node == previous_node) {
			break;
		}

		SGA_LinkId link_id = fns.link_between_nodes(stream->stream_data, current_node, previous_node);
		if (link_id != SIZE_MAX) {
			SGA_WalkStep step = {
			    link_id,
			    current_walk->time,
			    .interval_taken = current_walk->interval_taken,
			};
			SGA_WalkStepArrayList_push(&steps, step);
		}
		current_walk = previous;
	}

	SGA_WalkStepArrayList_reverse(&steps);
	return steps;
}

SGA_OptionalWalk unreachable_after(SGA_Time after) {
	return (SGA_OptionalWalk){
	    .type					    = NO_WALK,
	    .result.no_walk_reason.type			    = UNREACHABLE,
	    .result.no_walk_reason.reason.unreachable_after = after,
	};
}

SGA_OptionalWalk node_absent_between(SGA_Interval interval) {
	return (SGA_OptionalWalk){
	    .type					      = NO_WALK,
	    .result.no_walk_reason.type			      = NODE_ABSENT,
	    .result.no_walk_reason.reason.node_absent_between = interval,
	};
}

SGA_OptionalWalk walk_exists(SGA_Walk walk) {
	return (SGA_OptionalWalk){
	    .type	 = WALK,
	    .result.walk = walk,
	};
}

void node_can_still_appear(SGA_Stream* stream, SGA_NodeId node, SGA_Time current_time, SGA_Interval* current_node_present,
			   SGA_Interval* next_appearance, bool* can_still_appear) {
	StreamFunctions fns		     = STREAM_FUNCS(fns, stream);
	SGA_TimesIterator times_node_present = fns.times_node_present(stream->stream_data, node);
	*can_still_appear		     = false;

	SGA_FOR_EACH_TIME(interval, times_node_present) {
		if (SGA_Interval_contains(interval, current_time)) {
			*current_node_present = interval;
			*can_still_appear     = true;
			SGA_BREAK_ITERATOR(times_node_present);
		}
		else if (interval.start > current_time) {
			*next_appearance  = interval;
			*can_still_appear = true;
			SGA_BREAK_ITERATOR(times_node_present);
		}
	}
}

// Minimal number of hops between two nodes
// Uses buffering to avoid reallocating memory
void Stream_shortest_walk_from_to_at_buffered(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at, SGA_OptionalWalk* result,
					      Arena* arena, QueueInfoArrayList* queue, ExploredStateHashset* explored) {

	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	// Initialize the queue with the starting node
	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	QueueInfo start = {from, current_time, .interval_taken = SGA_Interval_from(at, max_lifespan), .previous = NULL, .previouses = 0};
	QueueInfoArrayList_push(queue, start);

	SGA_NodeId current_candidate = from;
	QueueInfo current_info;
	while ((current_candidate != to) && (!QueueInfoArrayList_is_empty(*queue))) {
		// Get the next node from the queue
		current_info = QueueInfoArrayList_pop_first(queue);

		// TODO : maybe there is a better solution than hashsets to not loop between
		// the same nodes?
		if (ExploredStateHashset_contains(*explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(explored, (ExploredState){current_info.node, current_info.time});
		current_candidate = current_info.node;
		current_time	  = current_info.time;

		// get for how long the node is present
		/*SGA_TimesIterator times_node_present =
		fns.times_node_present(stream->stream_data, current_candidate); SGA_Interval
		current_node_present	 = SGA_Interval_empty(); SGA_Interval next_appearance
		= SGA_Interval_from(current_time, max_lifespan); bool can_still_appear
		= false;

		SGA_FOR_EACH_TIME(interval, times_node_present) {
			if (SGA_Interval_contains(interval, current_time)) {
				current_node_present = interval;
				can_still_appear	 = true;
				SGA_BREAK_ITERATOR(times_node_present);
			}
			else if (interval.start > current_time) {
				next_appearance	 = interval;
				can_still_appear = true;
				SGA_BREAK_ITERATOR(times_node_present);
			}
		}

		if (!can_still_appear) {
			*result = node_absent_between(SGA_Interval_from(current_time,
		max_lifespan)); return;
		}

		if (SGA_Interval_is_empty(current_node_present)) {
			*result = node_absent_between(SGA_Interval_from(current_time,
		next_appearance.start)); return;
		}*/

		SGA_Interval current_node_present = SGA_Interval_empty();
		SGA_Interval next_appearance	  = SGA_Interval_from(current_time, max_lifespan);
		bool can_still_appear		  = false;
		node_can_still_appear(stream, current_candidate, current_time, &current_node_present, &next_appearance, &can_still_appear);

		if (!can_still_appear) {
			*result = node_absent_between(SGA_Interval_from(current_time, max_lifespan));
			return;
		}

		if (SGA_Interval_is_empty(current_node_present)) {
			*result = node_absent_between(SGA_Interval_from(current_time, next_appearance.start));
			return;
		}

		SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		SGA_FOR_EACH_LINK(link_id, neighbours) {
			SGA_IntervalArrayList intervals = SGA_collect_times(fns.times_link_present(stream->stream_data, link_id));
			for (size_t j = intervals.length; j-- > 0;) { // TODO: c'est quoi cette boucle de merde ??????
				SGA_Interval interval = intervals.array[j];

				// Any interval after the next disappearance of the node is unreachable
				if (interval.start > current_node_present.end) {
					break;
				}
				if (interval.end > current_node_present.end) {
					interval.end = current_node_present.end;
				}

				bool can_cross_now    = SGA_Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					SGA_Link link		= fns.link_by_id(stream->stream_data, link_id);
					SGA_NodeId neighbor_id	= link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous	= Arena_alloc(arena, sizeof(QueueInfo));
					*previous		= current_info;
					SGA_Time time_crossed	= can_cross_now ? current_time : interval.start;
					QueueInfo neighbor_info = {neighbor_id,
								   time_crossed,
								   .interval_taken = interval,
								   .previous	   = previous,
								   .previouses	   = current_info.previouses + 1};

					// try to find if the neighbor is already in the queue
					bool found = false;
					if (ExploredStateHashset_contains(*explored,
									  (ExploredState){neighbor_info.node, neighbor_info.time})) {
						continue;
					}
					for (size_t i = 0; i < queue->length; i++) {
						QueueInfo* info = &queue->array[i];
						if (info->node == neighbor_info.node &&
						    info->time == neighbor_info.time && // TODO: verify if we can improve the
											// hypothesis for exploring, maybe its true
											// for link streams but not for other cases)
						    info->previouses <= neighbor_info.previouses) {
							found = true;
							break;
						}
					}
					if (!found) {
						QueueInfoArrayList_push(queue, neighbor_info);
					}
				}
				if (!can_cross_now && !will_cross_later) {
					break;
				}
			}
			SGA_IntervalArrayList_destroy(intervals);
		}
	}

	// No walk found
	if (QueueInfoArrayList_is_empty(*queue) && current_candidate != to) {
		*result = unreachable_after(at);
		return;
	}

	// Build the walk
	SGA_Walk walk = {
	    .start	= from,
	    .end	= to,
	    .optimality = SGA_Interval_from(at, max_lifespan),
	    .stream	= stream,
	    .steps	= SGA_WalkStepArrayList_from_candidates(stream, &current_info, from),
	};

	// OPTIMISE: dont look through them all and propagate, just do a regular min
	// and cut when you have to wait for (size_t i = 0; i < walk.steps.length - 1;
	// i++) { 	WalkStep* step = &walk.steps.array[i];
	// step->needs_to_arrive_before = min(step->needs_to_arrive_before,
	// walk.steps.array[i + 1].needs_to_arrive_before);
	// }
	// walk.optimality.end = walk.steps.array[0].needs_to_arrive_before;
	size_t previous_time = SIZE_MAX;
	for (size_t i = 0; i < walk.steps.length; i++) {
		SGA_WalkStep* step = &walk.steps.array[i];
		previous_time	   = size_t_min(previous_time, step->interval_taken.end);
	}
	walk.optimality.end = previous_time;

	*result = walk_exists(walk);
}

// Minimal number of hops between two nodes
SGA_OptionalWalk SGA_shortest_walk(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at) {
	SGA_OptionalWalk result;
	Arena arena		      = Arena_init();
	QueueInfoArrayList queue      = QueueInfoArrayList_with_capacity(1);
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	Stream_shortest_walk_from_to_at_buffered(stream, from, to, at, &result, &arena, &queue, &explored);
	QueueInfoArrayList_destroy(queue);
	ExploredStateHashset_destroy(explored);
	Arena_destroy(arena);
	return result;
}

SGA_OptionalWalk Stream_fastest_shortest_walk(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at) {
	SGA_OptionalWalk result;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	Arena arena = Arena_init();

	// Initialize the queue with the starting node
	QueueInfoArrayList queue = QueueInfoArrayList_with_capacity(1);
	size_t max_lifespan	 = fns.lifespan(stream->stream_data).end;
	QueueInfo start		 = {
		     .node	     = from,
		     .time	     = current_time,
		     .depth	     = 0,
		     .interval_taken = SGA_Interval_from(at, max_lifespan),
		     .previous	     = NULL,
		     .previouses     = 0,
	 };
	QueueInfoArrayList_push(&queue, start);

	SGA_NodeId current_candidate = from;
	QueueInfo current_info;
	size_t best_time_yet = SIZE_MAX;
	QueueInfo best_yet;
	bool found		      = false;
	size_t depth_found	      = SIZE_MAX;
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	while ((!QueueInfoArrayList_is_empty(queue)) && ((!found) || (current_info.depth <= depth_found))) {
		// Get the next node from the queue
		current_info = QueueInfoArrayList_pop_first(&queue);
		if (ExploredStateHashset_contains(explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(&explored, (ExploredState){current_info.node, current_info.time});

		current_candidate = current_info.node;
		current_time	  = current_info.time;

		// Get for how long the node is present
		SGA_Interval current_node_present = SGA_Interval_empty();
		SGA_Interval next_appearance	  = SGA_Interval_from(current_time, max_lifespan);
		bool can_still_appear		  = false;
		node_can_still_appear(stream, current_candidate, current_time, &current_node_present, &next_appearance, &can_still_appear);

		if (!can_still_appear) {
			result = node_absent_between(SGA_Interval_from(current_time, max_lifespan));
			return result;
		}

		if (SGA_Interval_is_empty(current_node_present)) {
			result = node_absent_between(SGA_Interval_from(current_time, next_appearance.start));
			return result;
		}

		SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		SGA_FOR_EACH_LINK(link_id, neighbours) {
			SGA_Link link			     = fns.link_by_id(stream->stream_data, link_id);
			SGA_TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			SGA_FOR_EACH_TIME(interval, times_link_present) {

				// Any interval after the next disappearance of the node is unreachable
				if (interval.start > current_node_present.end) {
					SGA_BREAK_ITERATOR(times_link_present);
				}
				if (interval.end > current_node_present.end) {
					interval.end = current_node_present.end;
				}

				bool can_cross_now    = SGA_Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					SGA_NodeId neighbor_id	= link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous	= Arena_alloc(&arena, sizeof(QueueInfo));
					*previous		= current_info;
					SGA_Time time_crossed	= can_cross_now ? current_time : interval.start;
					QueueInfo neighbor_info = {
					    .node	    = neighbor_id,
					    .time	    = time_crossed,
					    .depth	    = current_info.depth + 1,
					    .interval_taken = interval,
					    .previous	    = previous,
					    .previouses	    = current_info.previouses + 1,
					};
					QueueInfoArrayList_push(&queue, neighbor_info);
				}
			}
		}
		if (current_candidate == to) {
			found	    = true;
			depth_found = current_info.depth;
			if (current_time < best_time_yet) {
				best_time_yet = current_time;
				best_yet      = current_info;
			}
		}
	}

	if (QueueInfoArrayList_is_empty(queue) && !found) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	SGA_Walk walk = {
	    .start	= from,
	    .end	= to,
	    .optimality = SGA_Interval_from(at, max_lifespan),
	    .stream	= stream,
	    .steps	= SGA_WalkStepArrayList_from_candidates(stream, &best_yet, from),
	};

	// propagate the optimal intervals
	if (walk.steps.length == 0) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	size_t previous_time = SIZE_MAX;
	for (size_t i = 0; i < walk.steps.length; i++) {
		SGA_WalkStep* step = &walk.steps.array[i];
		previous_time	   = size_t_min(previous_time, step->interval_taken.end);
	}
	walk.optimality.end = previous_time;

	result = walk_exists(walk);

cleanup_and_return:
	QueueInfoArrayList_destroy(queue);
	ExploredStateHashset_destroy(explored);
	Arena_destroy(arena);
	return result;
}

void SGA_Walk_destroy(SGA_Walk walk) {
	SGA_WalkStepArrayList_destroy(walk.steps);
}

typedef struct DijkstraState DijkstraState;
struct DijkstraState {
	SGA_NodeId node;
	size_t time;
	SGA_Interval interval_taken;
	size_t number_of_jumps;
	DijkstraState* previous;
};

bool DijkstraState_equals(DijkstraState* a, DijkstraState* b) {
	return a->node == b->node && a->time == b->time;
}

String DijkstraState_to_string(DijkstraState* state) {
	char* str = malloc(100);
	sprintf(str,
		"DijkstraState(node:%zu, time:%zu, number_of_jumps:%zu, "
		"interval_taken:%s)",
		state->node,
		state->time,
		state->number_of_jumps,
		SGA_Interval_to_string(&state->interval_taken).data);
	return String_from_owned(str);
}

size_t DijkstraState_hash(const DijkstraState* state) {
	return state->node * 32 ^ state->time;
}

int DijkstraState_compare(const DijkstraState* a, const DijkstraState* b) {
	if (a->time < b->time) {
		return -1;
	}
	if (a->time > b->time) {
		return 1;
	}
	if (a->number_of_jumps < b->number_of_jumps) {
		return -1;
	}
	if (a->number_of_jumps > b->number_of_jumps) {
		return 1;
	}
	return 0;
}

NO_FREE(DijkstraState);

// DefHashset(DijkstraState, DijkstraState_hash, NO_FREE(DijkstraState));
DeclareArrayList(DijkstraState);
DefineArrayList(DijkstraState);
DefineArrayListDeriveRemove(DijkstraState);
DefineArrayListDeriveToString(DijkstraState);
DefineArrayListDeriveOrdered(DijkstraState);

DeclareHashset(DijkstraState);
DefineHashset(DijkstraState);
DefineHashsetDeriveRemove(DijkstraState);
DefineHashsetDeriveEquals(DijkstraState);
DefineHashsetDeriveToString(DijkstraState);

DeclareBinaryHeap(DijkstraState);
DefineBinaryHeap(DijkstraState);

// Dijkstra's algorithm to find the minimum delay between two nodes
// Its also the shortest fastest
// OPTIMISE: preallocated memory of previouses like the other funcs
SGA_OptionalWalk SGA_fastest_walk(SGA_Stream* stream, SGA_NodeId from, SGA_NodeId to, SGA_Time at) {
	ASSERT(from != to);

	SGA_OptionalWalk result;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;

	// ArenaArrayList arena = ArenaArrayList_init();
	Arena arena = Arena_init();

	// Initialize the queue with the starting node
	DijkstraStateBinaryHeap queue = DijkstraStateBinaryHeap_with_capacity(10);
	DijkstraState start	      = {from, at, .interval_taken = SGA_Interval_from(at, max_lifespan), .previous = NULL};
	DijkstraStateBinaryHeap_insert(&queue, start);

	SGA_NodeId current_candidate = from;
	DijkstraState current_info;
	size_t best_time_yet = SIZE_MAX;
	DijkstraState best_yet;
	// bool found = false;
	// size_t depth_found = SIZE_MAX;
	DijkstraStateHashset explored = DijkstraStateHashset_with_capacity(10);

	while ((!DijkstraStateBinaryHeap_is_empty(&queue)) && (current_candidate != to)) {

		current_info = DijkstraStateBinaryHeap_pop(&queue);

		// printf("Current info : %s\n",
		// DijkstraState_to_string(&current_info).data);

		bool inserted = DijkstraStateHashset_insert(
		    &explored,
		    (DijkstraState){.node = current_info.node, .number_of_jumps = current_info.number_of_jumps, .time = current_info.time});

		if (!inserted) {
			continue;
		}
		// printf("Inserted : %d\n", inserted);
		// printf("Inserting %s\n", DijkstraState_to_string(&current_info).data);
		// printf("Explored : %s\n",
		// DijkstraStateHashset_to_string(&explored).data);

		current_candidate = current_info.node;
		current_time	  = current_info.time;

		// Get for how long the node is present
		SGA_Interval current_node_present = SGA_Interval_empty();
		SGA_Interval next_appearance	  = SGA_Interval_from(current_time, max_lifespan);
		bool can_still_appear		  = false;
		node_can_still_appear(stream, current_candidate, current_time, &current_node_present, &next_appearance, &can_still_appear);

		if (!can_still_appear) {
			result = node_absent_between(SGA_Interval_from(current_time, max_lifespan));
			goto cleanup_and_return;
		}

		if (SGA_Interval_is_empty(current_node_present)) {
			result = node_absent_between(SGA_Interval_from(current_time, next_appearance.start));
			goto cleanup_and_return;
		}
		// Get its neighbors
		SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		SGA_FOR_EACH_LINK(link_id, neighbours) {
			SGA_Link link			     = fns.link_by_id(stream->stream_data, link_id);
			SGA_TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			SGA_FOR_EACH_TIME(interval, times_link_present) {

				// Any interval after the next disappearance of the node is unreachable
				if (interval.start > current_node_present.end) {
					SGA_BREAK_ITERATOR(times_link_present);
				}
				if (interval.end > current_node_present.end) {
					interval.end = current_node_present.end;
				}

				bool can_cross_now    = SGA_Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					SGA_NodeId neighbor_id	= link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					DijkstraState* previous = Arena_alloc(&arena, sizeof(DijkstraState));
					*previous		= current_info;
					SGA_Time time_crossed	= can_cross_now ? current_time : interval.start;
					// DijkstraState neighbor_info = {neighbor_id, time_crossed,
					// current_info.number_of_jumps
					// + 1, .previous = previous};
					DijkstraState neighbor_info = {
					    .node	     = neighbor_id,
					    .interval_taken  = interval,
					    .number_of_jumps = current_info.number_of_jumps + 1,
					    .previous	     = previous,
					    .time	     = time_crossed,
					};

					DijkstraStateBinaryHeap_insert(&queue, neighbor_info);
					// printf("Inserting neighbor info : %s\n",
					// DijkstraState_to_string(&neighbor_info).data);
				}
			}
		}
		/*if (current_candidate == to) {
			found = true;
			depth_found = current_info.number_of_jumps;
			if (current_time < best_time_yet) {
				best_time_yet = current_time;
				best_yet = current_info;
			}
		}*/
		if (current_candidate == to && current_time < best_time_yet) {
			best_time_yet = current_time;
			best_yet      = current_info;
		}
	}

	if (DijkstraStateBinaryHeap_is_empty(&queue)) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	SGA_Walk walk = {
	    .start	= from,
	    .end	= to,
	    .optimality = SGA_Interval_from(at, best_time_yet),
	    .stream	= stream,
	    .steps	= SGA_WalkStepArrayList_with_capacity(1),
	};

	// Build the steps
	DijkstraState* current_walk = &best_yet;
	// printf("building walk\n");
	while (current_walk != NULL) {
		// char* str = DijkstraState_to_string(current_walk);
		// printf("Current walk : %s\n", str);
		// free(str);
		// String str = DijkstraState_to_string(current_walk);
		// printf("Current walk : %s\n", str.data);
		// String_destroy(str);
		DijkstraState* previous	 = current_walk->previous;
		SGA_NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		SGA_LinksIterator links = fns.links_set(stream->stream_data);
		SGA_FOR_EACH_LINK(i, links) {
			SGA_Link link = fns.link_by_id(stream->stream_data, i);
			if ((link.nodes[0] == current_walk->node && link.nodes[1] == previous_node) ||
			    (link.nodes[1] == current_walk->node && link.nodes[0] == previous_node)) {
				SGA_WalkStep step = {
				    .link	    = i,
				    .time	    = current_walk->time,
				    .interval_taken = current_walk->interval_taken,
				};
				SGA_WalkStepArrayList_push(&walk.steps, step);
				SGA_BREAK_ITERATOR(links);
			}
		}
		current_walk = previous;
	}

	// propagate the optimal intervals
	if (walk.steps.length == 0) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// for (size_t i = 0; i < walk.steps.length; i++) {
	// 	WalkStep* step = &walk.steps.array[i];
	// 	printf("Step %zu : %s\n", i, WalkStep_to_string(step).data);
	// }

	// for (size_t i = 0; i < walk.steps.length - 1; i++) {
	// 	WalkStep* step = &walk.steps.array[i];
	// 	step->needs_to_arrive_before =
	// 		min(step->needs_to_arrive_before, walk.steps.array[i +
	// 1].needs_to_arrive_before);
	// }
	// walk.optimality.end = walk.steps.array[0].needs_to_arrive_before;
	// printf("walk optimality : [%zu, %zu]\n", walk.optimality.start,
	// walk.optimality.end);

	size_t previous_time = SIZE_MAX;
	for (size_t i = 0; i < walk.steps.length; i++) {
		SGA_WalkStep* step = &walk.steps.array[i];
		previous_time	   = size_t_min(previous_time, step->interval_taken.end);
	}
	walk.optimality.end   = previous_time;
	walk.optimality.start = at;

	SGA_WalkStepArrayList_reverse(&walk.steps);
	result = walk_exists(walk);

	// printf("Walk : %s\n", Walk_to_string(&result.result.walk).data);

cleanup_and_return:
	DijkstraStateBinaryHeap_destroy(queue);
	DijkstraStateHashset_destroy(explored);
	Arena_destroy(arena);
	return result;
}

size_t Walk_length(SGA_Walk* walk) {
	return walk->steps.length;
}

size_t Walk_duration(SGA_Walk* walk) {
	if (walk->steps.length == 0) {
		return 0;
	}
	size_t start_time = walk->steps.array[0].time;
	size_t end_time	  = walk->steps.array[walk->steps.length - 1].time;
	return end_time - start_time;
}

double Walk_duration_integral(SGA_Interval waiting_period, SGA_Interval instantaneous_perioud, SGA_Time reached_at) {
	size_t a			 = waiting_period.start;
	size_t b			 = waiting_period.end;
	__attribute__((unused)) size_t c = instantaneous_perioud.start;
	__attribute__((unused)) size_t d = instantaneous_perioud.end;
	// integral of S(x = a -> b) reached_at - x dx + integral of S(x = c -> d) 0
	// dx
	size_t integral = ((reached_at - a) * (b - a)) / 2;

	return (double)integral;
}

double Walk_duration_integral_1_over_x(SGA_Walk* walk) {
	SGA_Time started_moving_at = walk->optimality.start;
	SGA_Time reached_at	   = walk->optimality.start;

	for (size_t i = 0; i < walk->steps.length; i++) {
		SGA_WalkStep step = walk->steps.array[i];
		if (step.time > reached_at) {
			reached_at = step.time;
		}
		if (step.interval_taken.start > reached_at) {
			reached_at = step.interval_taken.start;
		}
	}

	size_t stopped_waiting = reached_at;
	if (stopped_waiting > walk->optimality.end) {
		stopped_waiting = walk->optimality.end;
	}
	return Walk_duration_integral(
	    SGA_Interval_from(started_moving_at, stopped_waiting), SGA_Interval_from(stopped_waiting, walk->optimality.end), reached_at);
}

size_t Walk_length_integral_doubled(SGA_Walk* walk) {
	// Area under the curve from [start, end] of the walk
	// The area under the constant function (length) over the interval [start,
	// end] = t is (length * t) we double it to match with the duration integral
	size_t optimality_begin = walk->optimality.start;
	size_t optimality_end	= walk->optimality.end;
	size_t length		= Walk_length(walk);
	size_t time_frame	= optimality_end - optimality_begin;
	// size_t integral = length * time_frame;
	size_t integral = length * time_frame * 2;
	return integral;
}

bool SGA_Walk_involves_node_at_time(SGA_Walk* walk, SGA_NodeId node, SGA_Time time) {
	for (size_t i = 0; i < walk->steps.length; i++) {
		SGA_WalkStep step   = walk->steps.array[i];
		StreamFunctions fns = STREAM_FUNCS(fns, walk->stream);
		SGA_Link link	    = fns.link_by_id(walk->stream->stream_data, step.link);
		if (link.nodes[0] == node || link.nodes[1] == node) {
			// if ((double)step.time <= time && time <=
			// (double)step.needs_to_arrive_before) { 	return true;
			// }
			if (SGA_Interval_contains(step.interval_taken, time)) {
				return true;
			}
		}
	}
	return false;
}

// TODO: ca marchait ou pas je sais plus
double betweenness_of_node_at_time(SGA_Stream* stream, SGA_NodeId node, SGA_Time time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	// size_t number_of_walks = 0;
	// size_t number_of_walks_involving_node = 0;
	// Compute all the walks between all the nodes
	double betweenness	= 0.0;
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(from, nodes) {
		SGA_NodesIterator nodes2 = fns.nodes_set(stream->stream_data);
		SGA_FOR_EACH_NODE(to, nodes2) {
			if (from != to) {
				SGA_OptionalWalkArrayList optimal_walks =
				    SGA_optimal_walks_between_two_nodes(stream, from, to, SGA_fastest_walk);
				for (size_t i = 0; i < optimal_walks.length; i++) {
					SGA_OptionalWalk walk = optimal_walks.array[i];
					if (walk.type == WALK) {
						// size_t walk_optimality =
						// 	walk.result.walk.optimality.end -
						// walk.result.walk.optimality.start;
						size_t walk_optimality		      = 1;
						size_t number_of_walks		      = walk_optimality;
						size_t number_of_walks_involving_node = 0;
						if (SGA_Walk_involves_node_at_time(&walk.result.walk, node, time)) {
							number_of_walks_involving_node += walk_optimality;
						}
						if (number_of_walks == 0) {
							continue;
						}
						betweenness += (double)number_of_walks_involving_node / (double)number_of_walks;
					}
				}
				String str = SGA_OptionalWalkArrayList_to_string(&optimal_walks);
				String_destroy(str);
				SGA_OptionalWalkArrayList_destroy(optimal_walks);
			}
		}
	}
	return betweenness;
}

double Walk_length_integral(SGA_Walk* walk) {
	return (double)Walk_length(walk);
}

// TODO: merge it with robustness by duration
double SGA_Stream_robustness_by_length(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	size_t begin	    = fns.lifespan(stream->stream_data).start;

	QueueInfoArrayList queue      = QueueInfoArrayList_with_capacity(1);
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	Arena arena		      = Arena_init();

	double robustness = 0.0;

	SGA_NodeIdArrayList nodes = SGA_collect_node_ids(fns.nodes_set(stream->stream_data));

	// printf("Number of nodes : %zu\n", nodes.length);
	for (size_t f = 0; f < nodes.length; f++) {
		SGA_NodeId from = nodes.array[f];
		// printf("From %zu\n", from);
		for (size_t t = 0; t < nodes.length; t++) {
			// printf("To %zu\n", t);
			SGA_NodeId to = nodes.array[t];
			if (from == to) {
				SGA_TimesIterator presence = fns.times_node_present(stream->stream_data, from);
				SGA_FOR_EACH_TIME(p, presence) {
					robustness += (double)(p.end - p.start); // 1 if exists, 0 if not
				}
			}
			else {
				SGA_Time current_time = 0;
				while (current_time != max_lifespan) {
					// printf("Current time %zu\n", current_time);
					SGA_OptionalWalk optimal;
					// ArenaArrayList_clear(&arena);
					QueueInfoArrayList_clear(&queue);
					ExploredStateHashset_clear(&explored);
					Stream_shortest_walk_from_to_at_buffered(
					    stream, from, to, current_time, &optimal, &arena, &queue, &explored);
					size_t previous_time = current_time;
					if (optimal.type == NO_WALK) {
						SGA_NoWalkReason error = optimal.result.no_walk_reason;
						if (error.type == NODE_ABSENT) {
							current_time = error.reason.node_absent_between.end;
							continue;
						}
						if (error.type == UNREACHABLE) {
							SGA_OptionalWalk_destroy(optimal);
							Arena_clear(&arena);
							goto after_while;
						}
					}
					else if (optimal.type == WALK) {
						SGA_Interval optimality = optimal.result.walk.optimality;
						current_time		= optimality.end;
						robustness +=
						    (double)SGA_Interval_duration(optimality) / Walk_length_integral(&optimal.result.walk);
					}
					if (current_time == previous_time) {
						current_time++;
					}

					SGA_OptionalWalk_destroy(optimal);
					Arena_clear(&arena); // OPTIMISE: reuse it
				}
			after_while:
			}
		}
	}
	SGA_NodeIdArrayList_destroy(nodes);
	ExploredStateHashset_destroy(explored);
	QueueInfoArrayList_destroy(queue);
	Arena_destroy(arena);

	return robustness / (double)(nodes.length * nodes.length * (max_lifespan - begin));
}

double SGA_Stream_robustness_by_duration(SGA_Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	size_t begin	    = fns.lifespan(stream->stream_data).start;

	QueueInfoArrayList queue      = QueueInfoArrayList_with_capacity(1);
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	Arena arena		      = Arena_init();

	double robustness = 0.0;

	SGA_NodeIdArrayList nodes = SGA_collect_node_ids(fns.nodes_set(stream->stream_data));

	// printf("Number of nodes : %zu\n", nodes.length);
	for (size_t f = 0; f < nodes.length; f++) {
		SGA_NodeId from = nodes.array[f];
		// printf("From %zu\n", from);
		for (size_t t = 0; t < nodes.length; t++) {
			// printf("To %zu\n", t);
			SGA_NodeId to = nodes.array[t];
			if (from == to) {
				SGA_TimesIterator presence = fns.times_node_present(stream->stream_data, from);
				SGA_FOR_EACH_TIME(p, presence) {
					robustness += (double)(p.end - p.start); // 1 if exists, 0 if not
				}
			}
			else {
				SGA_Time current_time = 0;
				while (current_time != max_lifespan) {
					// printf("Current time %zu\n", current_time);
					SGA_OptionalWalk optimal;
					// ArenaArrayList_clear(&arena);
					QueueInfoArrayList_clear(&queue);
					ExploredStateHashset_clear(&explored);
					// Stream_shortest_walk_from_to_at_buffered(stream, from, to,
					// current_time, &optimal, &arena, &queue,
					// &explored);
					optimal = SGA_fastest_walk(stream, from, to, current_time);
					// printf("Optimal : %s\n", SGA_OptionalWalk_to_string(&optimal).data);
					size_t previous_time = current_time;
					if (optimal.type == NO_WALK) {
						SGA_NoWalkReason error = optimal.result.no_walk_reason;
						if (error.type == NODE_ABSENT) {
							current_time = error.reason.node_absent_between.end;
							continue;
						}
						if (error.type == UNREACHABLE) {
							SGA_OptionalWalk_destroy(optimal);
							Arena_clear(&arena);
							goto after_while;
						}
					}
					else if (optimal.type == WALK) {
						SGA_Interval optimality = optimal.result.walk.optimality;
						current_time		= optimality.end;
						robustness += (double)(SGA_Interval_duration(optimality)) /
							      (1.0 + Walk_duration_integral_1_over_x(&optimal.result.walk));
					}
					if (current_time == previous_time) {
						current_time++;
					}

					// TODO : shit name

					SGA_OptionalWalk_destroy(optimal);
					Arena_clear(&arena); // OPTIMISE: reuse it
				}
			after_while:
			}
		}
	}
	SGA_NodeIdArrayList_destroy(nodes);
	ExploredStateHashset_destroy(explored);
	QueueInfoArrayList_destroy(queue);
	Arena_destroy(arena);

	return robustness / (double)(nodes.length * nodes.length * (max_lifespan - begin));
}

bool SGA_Walk_goes_through(SGA_Walk* walk, SGA_Stream stream, size_t nb_steps, ...) {

	if (walk->steps.length != nb_steps) {
		// printf("Walk size %zu != nb_steps %zu\n", walk->steps.length, nb_steps);
		return false;
	}

	if (nb_steps == 0) {
		// printf("Nb steps 0\n");
		return true;
	}

	StreamFunctions fns = STREAM_FUNCS(fns, &stream);
	va_list args;
	va_start(args, nb_steps);

	// Check the first node
	SGA_NodeId first_node = va_arg(args, SGA_NodeId);
	SGA_Link first_link   = fns.link_by_id(stream.stream_data, walk->steps.array[0].link);
	// printf("Checking first node %zu\n", first_node);
	if (first_link.nodes[0] != first_node && first_link.nodes[1] != first_node) {
		// printf("First node %zu FALSE\n", first_node);
		return false;
	}

	// Check the intermediate nodes
	for (size_t i = 1; i < nb_steps; i++) {
		SGA_NodeId node = va_arg(args, SGA_NodeId);
		// printf("Checking node %zu\n", node);
		SGA_Link link = fns.link_by_id(stream.stream_data, walk->steps.array[i].link);
		if (link.nodes[0] != node && link.nodes[1] != node) {
			// printf("Node %zu FALSE\n", node);
			return false;
		}
	}

	// Check the last node
	SGA_NodeId last_node = va_arg(args, SGA_NodeId);
	SGA_Link last_link   = fns.link_by_id(stream.stream_data, walk->steps.array[nb_steps - 1].link);
	// printf("Checking last node %zu\n", last_node);
	if (last_link.nodes[0] != last_node && last_link.nodes[1] != last_node) {
		// printf("Last node %zu FALSE\n", last_node);
		return false;
	}

	va_end(args);
	return true;
}

SGA_Walk SGA_OptionalWalk_unwrap_unchecked(SGA_OptionalWalk info) {
	return info.result.walk;
}

SGA_Walk SGA_OptionalWalk_unwrap_checked(SGA_OptionalWalk info) {
	ASSERT(info.type == WALK);
	return SGA_OptionalWalk_unwrap_unchecked(info);
}

SGA_Time SGA_Walk_arrives_at(SGA_Walk* walk) {
	// printf("Walk size : %zu\n", walk->steps.length);
	// printf("Walk arrival time : %zu\n", walk->steps.array[walk->steps.length -
	// 1].time);
	return walk->steps.array[walk->steps.length - 1].time;
}