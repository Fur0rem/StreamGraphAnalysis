#include "walks.h"
#include "../arena.h"
#include "../defaults.h"
#include "../generic_data_structures/hashset.h"
#include "../interval.h"
#include "../iterators.h"
#include "../stream.h"
#include "../stream_functions.h"

// For STREAM_FUNCS
#include "../generic_data_structures/arraylist.h"
#include "../generic_data_structures/binary_heap.h"
#include "../metrics.h"
#include "../stream_wrappers.h"
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

bool TreeEdge_equals(const TreeEdge* a, const TreeEdge* b) {
	return a->weight == b->weight && a->child->id == b->child->id;
}

String TreeEdge_to_string(const TreeEdge* edge) {
	char* str = malloc(100);
	sprintf(str, "TreeEdge(weight:%zu, child:%zu)", edge->weight, edge->child->id);
	return String_from_owned(str);
}

// Breadth-first search for shortest walk

typedef struct QueueInfo QueueInfo;
struct QueueInfo {
	NodeId node;
	size_t time;
	size_t depth;
	size_t previouses;
	Interval interval_taken;
	QueueInfo* previous;
};

bool QueueInfo_equals(const QueueInfo* a, const QueueInfo* b) {
	return a->node == b->node && a->time == b->time && a->previous == b->previous &&
		   Interval_equals(&a->interval_taken, &b->interval_taken) && a->depth == b->depth;
}

String QueueInfo_to_string(const QueueInfo* info) {
	/*char* str = malloc(100);
	sprintf(str, "QueueInfo(node:%zu, time:%zu, interval taken:%s, depth:%zu)", info->node, info->time,
			Interval_to_string(&info->interval_taken), info->depth);
	return str;*/
	String str = String_from_duplicate("");
	char buf[100];
	sprintf(buf, "QueueInfo(node:%zu, time:%zu, interval taken:", info->node, info->time);
	String_push_str(&str, buf);
	String interval_str = Interval_to_string(&info->interval_taken);
	String_concat_copy(&str, &interval_str);
	String_push_str(&str, ", depth:");
	sprintf(buf, "%zu", info->depth);
	String_push_str(&str, buf);
	return str;
}

String WalkStep_to_string(const WalkStep* step) {
	char* str = MALLOC(100);
	sprintf(str, "WalkStep(link:%zu, time:%zu, interval taken:%s)", step->link, step->time, Interval_to_string(&step->interval_taken).data);
	return String_from_owned(str);
}

bool WalkStep_equals(const WalkStep* a, const WalkStep* b) {
	return a->link == b->link && a->time == b->time;
}

// DefArrayList(QueueInfo, NO_FREE(QueueInfo));
DeclareArrayList(QueueInfo);
DefineArrayList(QueueInfo);
NO_FREE(QueueInfo);
DefineArrayListDeriveRemove(QueueInfo);

size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

size_t max(size_t a, size_t b) {
	return a > b ? a : b;
}

typedef struct {
	NodeId node;
	TimeId time;
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

String Walk_to_string(const Walk* walk) {
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
	String time_str = Interval_to_string(&walk->optimality);
	String_push_str(&str, " | Optimal at ");
	String_concat_consume(&str, time_str);
	String_push(&str, '\n');

	FullStreamGraph* fsg = (FullStreamGraph*)walk->stream->stream_data;
	StreamGraph sg		 = *fsg->underlying_stream_graph;

	for (size_t i = 0; i < walk->steps.length; i++) {
		WalkStep step = walk->steps.array[i];
		sprintf(buf, "%zu", step.link);
		String_push_str(&str, buf);
		NodeId from = sg.links.links[step.link].nodes[0];
		NodeId to	= sg.links.links[step.link].nodes[1];
		sprintf(buf, " (%zu -> %zu)", from, to);
		String_push_str(&str, buf);
		String_push_str(&str, " @ ");
		sprintf(buf, "%zu", step.time);
		String_push_str(&str, buf);
		Interval interval	= step.interval_taken;
		String interval_str = Interval_to_string(&interval);
		String_push_str(&str, " | ");
		String_concat_consume(&str, interval_str);
		String_push(&str, '\n');
	}
	return str;
}

bool Walk_equals(const Walk* a, const Walk* b) {
	for (size_t i = 0; i < a->steps.length; i++) {
		if (!WalkStep_equals(&a->steps.array[i], &b->steps.array[i])) {
			return false;
		}
	}
	return true;
}

DeclareArrayList(Walk);
DefineArrayList(Walk);
DefineArrayListDeriveRemove(Walk);
DefineArrayListDeriveToString(Walk);
DefineArrayListDeriveEquals(Walk);

String WalkInfo_to_string(const WalkInfo* wi) {
	if (wi->type == WALK) {
		String walk_str = Walk_to_string(&wi->result.walk);
		String str		= String_from_duplicate("WalkInfo(WALK, ");
		String_concat_consume(&str, walk_str);
		String_push(&str, ')');
		return str;
	}
	else {
		String str			= String_from_duplicate("WalkInfo(NO_WALK, ");
		NoWalkReason reason = wi->result.no_walk_reason;
		if (reason.type == NODE_ABSENT) {
			String interval_str = Interval_to_string(&reason.reason.node_absent_between);
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
bool WalkInfo_equals(const WalkInfo* a, const WalkInfo* b) {
	if (a->type != b->type) {
		return false;
	}
	if (a->type == WALK) {
		return Walk_equals(&a->result.walk, &b->result.walk);
	}
	else {
		return a->result.no_walk_reason.type == b->result.no_walk_reason.type;
	}
}

WalkInfoArrayList optimal_walks_between_two_nodes(Stream* stream, NodeId from, NodeId to, WalkInfo (*fn)(Stream*, NodeId, NodeId, TimeId)) {
	WalkInfoArrayList walks = WalkInfoArrayList_with_capacity(1);
	StreamFunctions fns		= STREAM_FUNCS(fns, stream);

	Interval lifespan	= fns.lifespan(stream->stream_data);
	TimeId current_time = lifespan.start;

	while (current_time != lifespan.end) {
		printf("Current time in func: %zu\n", current_time);
		size_t previous_time = current_time;
		WalkInfo optimal	 = fn(stream, from, to, current_time);
		WalkInfoArrayList_push(&walks, optimal);
		printf("Optimal walk type: %s\n", WalkInfo_to_string(&optimal).data);
		if (optimal.type == NO_WALK) {
			NoWalkReason error = optimal.result.no_walk_reason;
			if (error.type == NODE_ABSENT) {
				current_time = error.reason.node_absent_between.end;
			}
			else if (error.type == UNREACHABLE) {
				return walks;
			}
		}
		else if (optimal.type == WALK) {
			printf("Optimal walk found : %s\n", Walk_to_string(&optimal.result.walk).data);
			Interval optimality = optimal.result.walk.optimality;
			printf("Optimality: %zu -> %zu\n", optimality.start, optimality.end);
			ASSERT(optimality.start <= optimality.end);
			current_time = optimality.end;
		}
		if (current_time == previous_time) {
			current_time++;
		}
	}
	return walks;
}

void WalkInfo_destroy(WalkInfo wi) {
	if (wi.type == WALK) {
		Walk_destroy(wi.result.walk);
	}
}

// TODO : put it in the right place
DefineArrayList(WalkStep);
NO_FREE(WalkStep);
DefineArrayListDeriveRemove(WalkStep);
DefineArrayListDeriveToString(WalkStep);

DefineArrayList(WalkInfo);
DefineArrayListDeriveRemove(WalkInfo);
DefineArrayListDeriveEquals(WalkInfo);
DefineArrayListDeriveToString(WalkInfo);

WalkStepArrayList WalkStepArrayList_from_candidates(Stream* stream, QueueInfo* candidates, NodeId from) {
	QueueInfo* current_walk = candidates;
	WalkStepArrayList steps = WalkStepArrayList_with_capacity(candidates->previouses);
	StreamFunctions fns		= STREAM_FUNCS(fns, stream);

	while (current_walk != NULL) {
		NodeId current_node	 = current_walk->node;
		QueueInfo* previous	 = current_walk->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;

		if (current_node == previous_node) {
			break;
		}

		LinkId link_id = fns.link_between_nodes(stream->stream_data, current_node, previous_node);
		if (link_id != SIZE_MAX) {
			WalkStep step = {link_id, current_walk->time, .interval_taken = current_walk->interval_taken};
			WalkStepArrayList_push(&steps, step);
		}
		current_walk = previous;
	}

	WalkStepArrayList_reverse(&steps);
	return steps;
}

WalkInfo unreachable_after(TimeId after) {
	return (WalkInfo){
		.type											= NO_WALK,
		.result.no_walk_reason.type						= UNREACHABLE,
		.result.no_walk_reason.reason.unreachable_after = after,
	};
}

WalkInfo node_absent_between(Interval interval) {
	return (WalkInfo){
		.type											  = NO_WALK,
		.result.no_walk_reason.type						  = NODE_ABSENT,
		.result.no_walk_reason.reason.node_absent_between = interval,
	};
}

WalkInfo walk_exists(Walk walk) {
	return (WalkInfo){
		.type		 = WALK,
		.result.walk = walk,
	};
}

void node_can_still_appear(Stream* stream, NodeId node, TimeId current_time, Interval* current_node_present, Interval* next_appearance,
						   bool* can_still_appear) {
	StreamFunctions fns				 = STREAM_FUNCS(fns, stream);
	TimesIterator times_node_present = fns.times_node_present(stream->stream_data, node);
	*can_still_appear				 = false;

	FOR_EACH_TIME(interval, times_node_present) {
		if (Interval_contains(interval, current_time)) {
			*current_node_present = interval;
			*can_still_appear	  = true;
			BREAK_ITER(times_node_present);
		}
		else if (interval.start > current_time) {
			*next_appearance  = interval;
			*can_still_appear = true;
			BREAK_ITER(times_node_present);
		}
	}
}

// Minimal number of hops between two nodes
// Uses buffering to avoid reallocating memory
void Stream_shortest_walk_from_to_at_buffered(Stream* stream, NodeId from, NodeId to, TimeId at, WalkInfo* result, Arena* arena,
											  QueueInfoArrayList* queue, ExploredStateHashset* explored) {

	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	// Initialize the queue with the starting node
	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	QueueInfo start		= {from, current_time, .interval_taken = Interval_from(at, max_lifespan), .previous = NULL, .previouses = 0};
	QueueInfoArrayList_push(queue, start);

	NodeId current_candidate = from;
	QueueInfo current_info;
	while ((current_candidate != to) && (!QueueInfoArrayList_is_empty(*queue))) {
		// Get the next node from the queue
		current_info = QueueInfoArrayList_pop_first(queue);

		// TODO : maybe there is a better solution than hashsets to not loop between the same nodes?
		if (ExploredStateHashset_contains(*explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(explored, (ExploredState){current_info.node, current_info.time});
		current_candidate = current_info.node;
		current_time	  = current_info.time;

		// get for how long the node is present
		/*TimesIterator times_node_present = fns.times_node_present(stream->stream_data, current_candidate);
		Interval current_node_present	 = Interval_empty();
		Interval next_appearance		 = Interval_from(current_time, max_lifespan);
		bool can_still_appear			 = false;

		FOR_EACH_TIME(interval, times_node_present) {
			if (Interval_contains(interval, current_time)) {
				current_node_present = interval;
				can_still_appear	 = true;
				BREAK_ITER(times_node_present);
			}
			else if (interval.start > current_time) {
				next_appearance	 = interval;
				can_still_appear = true;
				BREAK_ITER(times_node_present);
			}
		}

		if (!can_still_appear) {
			*result = node_absent_between(Interval_from(current_time, max_lifespan));
			return;
		}

		if (Interval_is_empty(current_node_present)) {
			*result = node_absent_between(Interval_from(current_time, next_appearance.start));
			return;
		}*/

		Interval current_node_present = Interval_empty();
		Interval next_appearance	  = Interval_from(current_time, max_lifespan);
		bool can_still_appear		  = false;
		node_can_still_appear(stream, current_candidate, current_time, &current_node_present, &next_appearance, &can_still_appear);

		if (!can_still_appear) {
			*result = node_absent_between(Interval_from(current_time, max_lifespan));
			return;
		}

		if (Interval_is_empty(current_node_present)) {
			*result = node_absent_between(Interval_from(current_time, next_appearance.start));
			return;
		}

		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		FOR_EACH_LINK(link_id, neighbours) {
			IntervalArrayList intervals = SGA_collect_times(fns.times_link_present(stream->stream_data, link_id));
			for (size_t j = intervals.length; j-- > 0;) { // TODO: c'est quoi cette boucle de merde ??????
				Interval interval = intervals.array[j];

				// Any interval after the next disappearance of the node is unreachable
				if (interval.start > current_node_present.end) {
					break;
				}
				if (interval.end > current_node_present.end) {
					interval.end = current_node_present.end;
				}

				bool can_cross_now	  = Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					Link link				= fns.link_by_id(stream->stream_data, link_id);
					NodeId neighbor_id		= link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous		= Arena_alloc(arena, sizeof(QueueInfo));
					*previous				= current_info;
					TimeId time_crossed		= can_cross_now ? current_time : interval.start;
					QueueInfo neighbor_info = {neighbor_id,
											   time_crossed,
											   .interval_taken = interval,
											   .previous	   = previous,
											   .previouses	   = current_info.previouses + 1};

					// try to find if the neighbor is already in the queue
					bool found = false;
					if (ExploredStateHashset_contains(*explored, (ExploredState){neighbor_info.node, neighbor_info.time})) {
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
			IntervalArrayList_destroy(intervals);
		}
	}

	// No walk found
	if (QueueInfoArrayList_is_empty(*queue) && current_candidate != to) {
		*result = unreachable_after(at);
		return;
	}

	// Build the walk
	Walk walk = {
		.start		= from,
		.end		= to,
		.optimality = Interval_from(at, max_lifespan),
		.stream		= stream,
		.steps		= WalkStepArrayList_from_candidates(stream, &current_info, from),
	};

	// OPTIMISE: dont look through them all and propagate, just do a regular min and cut when you have to wait
	// for (size_t i = 0; i < walk.steps.length - 1; i++) {
	// 	WalkStep* step = &walk.steps.array[i];
	// 	step->needs_to_arrive_before =
	// 		min(step->needs_to_arrive_before, walk.steps.array[i + 1].needs_to_arrive_before);
	// }
	// walk.optimality.end = walk.steps.array[0].needs_to_arrive_before;
	size_t previous_time = SIZE_MAX;
	for (size_t i = 0; i < walk.steps.length; i++) {
		WalkStep* step = &walk.steps.array[i];
		previous_time  = min(previous_time, step->interval_taken.end);
	}
	walk.optimality.end = previous_time;

	*result = walk_exists(walk);
}

// Minimal number of hops between two nodes
WalkInfo Stream_shortest_walk_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at) {
	WalkInfo result;
	Arena arena					  = Arena_init();
	QueueInfoArrayList queue	  = QueueInfoArrayList_with_capacity(1);
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	Stream_shortest_walk_from_to_at_buffered(stream, from, to, at, &result, &arena, &queue, &explored);
	QueueInfoArrayList_destroy(queue);
	ExploredStateHashset_destroy(explored);
	Arena_destroy(arena);
	return result;
}

WalkInfo Stream_fastest_shortest_walk(Stream* stream, NodeId from, NodeId to, TimeId at) {
	WalkInfo result;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	Arena arena = Arena_init();

	// Initialize the queue with the starting node
	QueueInfoArrayList queue = QueueInfoArrayList_with_capacity(1);
	size_t max_lifespan		 = fns.lifespan(stream->stream_data).end;
	QueueInfo start			 = {
				 .node			 = from,
				 .time			 = current_time,
				 .depth			 = 0,
				 .interval_taken = Interval_from(at, max_lifespan),
				 .previous		 = NULL,
				 .previouses	 = 0,
	 };
	QueueInfoArrayList_push(&queue, start);

	NodeId current_candidate = from;
	QueueInfo current_info;
	size_t best_time_yet = SIZE_MAX;
	QueueInfo best_yet;
	bool found					  = false;
	size_t depth_found			  = SIZE_MAX;
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
		Interval current_node_present = Interval_empty();
		Interval next_appearance	  = Interval_from(current_time, max_lifespan);
		bool can_still_appear		  = false;
		node_can_still_appear(stream, current_candidate, current_time, &current_node_present, &next_appearance, &can_still_appear);

		if (!can_still_appear) {
			result = node_absent_between(Interval_from(current_time, max_lifespan));
			return result;
		}

		if (Interval_is_empty(current_node_present)) {
			result = node_absent_between(Interval_from(current_time, next_appearance.start));
			return result;
		}

		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		FOR_EACH_LINK(link_id, neighbours) {
			Link link						 = fns.link_by_id(stream->stream_data, link_id);
			TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times_link_present) {

				// Any interval after the next disappearance of the node is unreachable
				if (interval.start > current_node_present.end) {
					BREAK_ITER(times_link_present);
				}
				if (interval.end > current_node_present.end) {
					interval.end = current_node_present.end;
				}

				bool can_cross_now	  = Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					NodeId neighbor_id		= link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous		= Arena_alloc(&arena, sizeof(QueueInfo));
					*previous				= current_info;
					TimeId time_crossed		= can_cross_now ? current_time : interval.start;
					QueueInfo neighbor_info = {
						neighbor_id,
						time_crossed,
						current_info.depth + 1,
						.interval_taken = interval,
						.previous		= previous,
						.previouses		= current_info.previouses + 1,
					};
					QueueInfoArrayList_push(&queue, neighbor_info);
				}
			}
		}
		if (current_candidate == to) {
			found		= true;
			depth_found = current_info.depth;
			if (current_time < best_time_yet) {
				best_time_yet = current_time;
				best_yet	  = current_info;
			}
		}
	}

	if (QueueInfoArrayList_is_empty(queue) && !found) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	Walk walk = {
		.start		= from,
		.end		= to,
		.optimality = Interval_from(at, max_lifespan),
		.stream		= stream,
		.steps		= WalkStepArrayList_from_candidates(stream, &best_yet, from),
	};

	// propagate the optimal intervals
	if (walk.steps.length == 0) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	size_t previous_time = SIZE_MAX;
	for (size_t i = 0; i < walk.steps.length; i++) {
		WalkStep* step = &walk.steps.array[i];
		previous_time  = min(previous_time, step->interval_taken.end);
	}
	walk.optimality.end = previous_time;

	result = walk_exists(walk);

cleanup_and_return:
	QueueInfoArrayList_destroy(queue);
	ExploredStateHashset_destroy(explored);
	Arena_destroy(arena);
	return result;
}

void Walk_destroy(Walk walk) {
	WalkStepArrayList_destroy(walk.steps);
}

typedef struct DijkstraState DijkstraState;
struct DijkstraState {
	NodeId node;
	size_t time;
	Interval interval_taken;
	size_t number_of_jumps;
	DijkstraState* previous;
};

bool DijkstraState_equals(DijkstraState* a, DijkstraState* b) {
	return a->node == b->node && a->time == b->time;
}

String DijkstraState_to_string(DijkstraState* state) {
	char* str = malloc(100);
	sprintf(str,
			"DijkstraState(node:%zu, time:%zu, number_of_jumps:%zu, interval_taken:%s)",
			state->node,
			state->time,
			state->number_of_jumps,
			Interval_to_string(&state->interval_taken).data);
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
WalkInfo Stream_fastest_walk(Stream* stream, NodeId from, NodeId to, TimeId at) {
	ASSERT(from != to);

	WalkInfo result;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;

	// ArenaArrayList arena = ArenaArrayList_init();
	Arena arena = Arena_init();

	// Initialize the queue with the starting node
	DijkstraStateBinaryHeap queue = DijkstraStateBinaryHeap_with_capacity(10);
	DijkstraState start			  = {from, at, .interval_taken = Interval_from(at, max_lifespan), .previous = NULL};
	DijkstraStateBinaryHeap_insert(&queue, start);

	NodeId current_candidate = from;
	DijkstraState current_info;
	size_t best_time_yet = SIZE_MAX;
	DijkstraState best_yet;
	// bool found = false;
	// size_t depth_found = SIZE_MAX;
	DijkstraStateHashset explored = DijkstraStateHashset_with_capacity(10);

	while ((!DijkstraStateBinaryHeap_is_empty(&queue)) && (current_candidate != to)) {

		current_info = DijkstraStateBinaryHeap_pop(&queue);

		// printf("Current info : %s\n", DijkstraState_to_string(&current_info).data);

		bool inserted = DijkstraStateHashset_insert(
			&explored,
			(DijkstraState){.node = current_info.node, .number_of_jumps = current_info.number_of_jumps, .time = current_info.time});

		if (!inserted) {
			continue;
		}
		// printf("Inserted : %d\n", inserted);
		// printf("Inserting %s\n", DijkstraState_to_string(&current_info).data);
		// printf("Explored : %s\n", DijkstraStateHashset_to_string(&explored).data);

		current_candidate = current_info.node;
		current_time	  = current_info.time;

		// Get for how long the node is present
		Interval current_node_present = Interval_empty();
		Interval next_appearance	  = Interval_from(current_time, max_lifespan);
		bool can_still_appear		  = false;
		node_can_still_appear(stream, current_candidate, current_time, &current_node_present, &next_appearance, &can_still_appear);

		if (!can_still_appear) {
			result = node_absent_between(Interval_from(current_time, max_lifespan));
			goto cleanup_and_return;
		}

		if (Interval_is_empty(current_node_present)) {
			result = node_absent_between(Interval_from(current_time, next_appearance.start));
			goto cleanup_and_return;
		}
		// Get its neighbors
		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		FOR_EACH_LINK(link_id, neighbours) {
			Link link						 = fns.link_by_id(stream->stream_data, link_id);
			TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times_link_present) {

				// Any interval after the next disappearance of the node is unreachable
				if (interval.start > current_node_present.end) {
					BREAK_ITER(times_link_present);
				}
				if (interval.end > current_node_present.end) {
					interval.end = current_node_present.end;
				}

				bool can_cross_now	  = Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					NodeId neighbor_id		= link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					DijkstraState* previous = Arena_alloc(&arena, sizeof(DijkstraState));
					*previous				= current_info;
					TimeId time_crossed		= can_cross_now ? current_time : interval.start;
					// DijkstraState neighbor_info = {neighbor_id, time_crossed, current_info.number_of_jumps
					// + 1, 							   .previous = previous};
					DijkstraState neighbor_info = {
						.node			 = neighbor_id,
						.interval_taken	 = interval,
						.number_of_jumps = current_info.number_of_jumps + 1,
						.previous		 = previous,
						.time			 = time_crossed,
					};

					DijkstraStateBinaryHeap_insert(&queue, neighbor_info);
					// printf("Inserting neighbor info : %s\n", DijkstraState_to_string(&neighbor_info).data);
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
			best_yet	  = current_info;
		}
	}

	if (DijkstraStateBinaryHeap_is_empty(&queue)) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	Walk walk = {
		.start		= from,
		.end		= to,
		.optimality = Interval_from(at, best_time_yet),
		.stream		= stream,
		.steps		= WalkStepArrayList_with_capacity(1),
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
		DijkstraState* previous = current_walk->previous;
		NodeId previous_node	= previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		LinksIterator links = fns.links_set(stream->stream_data);
		FOR_EACH_LINK(i, links) {
			Link link = fns.link_by_id(stream->stream_data, i);
			if ((link.nodes[0] == current_walk->node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_walk->node && link.nodes[0] == previous_node)) {
				WalkStep step = {i, current_walk->time, .interval_taken = current_walk->interval_taken};
				WalkStepArrayList_push(&walk.steps, step);
				BREAK_ITER(links);
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
	// 		min(step->needs_to_arrive_before, walk.steps.array[i + 1].needs_to_arrive_before);
	// }
	// walk.optimality.end = walk.steps.array[0].needs_to_arrive_before;
	// printf("walk optimality : [%zu, %zu]\n", walk.optimality.start, walk.optimality.end);

	size_t previous_time = SIZE_MAX;
	for (size_t i = 0; i < walk.steps.length; i++) {
		WalkStep* step = &walk.steps.array[i];
		previous_time  = min(previous_time, step->interval_taken.end);
	}
	walk.optimality.end	  = previous_time;
	walk.optimality.start = at;

	WalkStepArrayList_reverse(&walk.steps);
	result = walk_exists(walk);

	// printf("Walk : %s\n", Walk_to_string(&result.result.walk).data);

cleanup_and_return:
	DijkstraStateBinaryHeap_destroy(queue);
	DijkstraStateHashset_destroy(explored);
	Arena_destroy(arena);
	return result;
}

size_t Walk_length(Walk* walk) {
	return walk->steps.length;
}

size_t Walk_duration(Walk* walk) {
	if (walk->steps.length == 0) {
		return 0;
	}
	size_t start_time = walk->steps.array[0].time;
	size_t end_time	  = walk->steps.array[walk->steps.length - 1].time;
	return end_time - start_time;
}

double Walk_duration_integral(Interval waiting_period, Interval instantaneous_perioud, TimeId reached_at) {
	size_t a = waiting_period.start;
	size_t b = waiting_period.end;
	size_t c = instantaneous_perioud.start;
	size_t d = instantaneous_perioud.end;
	// integral of S(x = a -> b) reached_at - x dx + integral of S(x = c -> d) 0 dx
	double integral = (reached_at - a) * (b - a) / 2;

	return integral;
}

double Walk_duration_integral_1_over_x(Walk* walk) {
	TimeId started_moving_at = walk->optimality.start;
	TimeId reached_at		 = walk->optimality.start;

	for (size_t i = 0; i < walk->steps.length; i++) {
		WalkStep step = walk->steps.array[i];
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
		Interval_from(walk->optimality.start, stopped_waiting), Interval_from(stopped_waiting, walk->optimality.end), reached_at);
}

size_t Walk_length_integral_doubled(Walk* walk) {
	// Area under the curve from [start, end] of the walk
	// The area under the constant function (length) over the interval [start, end] = t
	// is (length * t)
	// we double it to match with the duration integral
	size_t optimality_begin = walk->optimality.start;
	size_t optimality_end	= walk->optimality.end;
	size_t length			= Walk_length(walk);
	size_t time_frame		= optimality_end - optimality_begin;
	// size_t integral = length * time_frame;
	size_t integral = length * time_frame * 2;
	return integral;
}

bool Walk_involves_node_at_time(Walk* walk, NodeId node, double time) {
	for (size_t i = 0; i < walk->steps.length; i++) {
		WalkStep step		= walk->steps.array[i];
		StreamFunctions fns = STREAM_FUNCS(fns, walk->stream);
		Link link			= fns.link_by_id(walk->stream->stream_data, step.link);
		if (link.nodes[0] == node || link.nodes[1] == node) {
			// if ((double)step.time <= time && time <= (double)step.needs_to_arrive_before) {
			// 	return true;
			// }
			if ((double)step.interval_taken.start <= time && time <= (double)step.interval_taken.end) {
				return true;
			}
		}
	}
	return false;
}

double betweenness_of_node_at_time(Stream* stream, NodeId node, double time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	// size_t number_of_walks = 0;
	// size_t number_of_walks_involving_node = 0;
	// Compute all the walks between all the nodes
	double betweenness	= 0.0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(from, nodes) {
		NodesIterator nodes2 = fns.nodes_set(stream->stream_data);
		FOR_EACH_NODE(to, nodes2) {
			if (from != to) {
				WalkInfoArrayList optimal_walks = optimal_walks_between_two_nodes(stream, from, to, Stream_fastest_walk);
				for (size_t i = 0; i < optimal_walks.length; i++) {
					WalkInfo walk = optimal_walks.array[i];
					if (walk.type == WALK) {
						// size_t walk_optimality =
						// 	walk.result.walk.optimality.end - walk.result.walk.optimality.start;
						size_t walk_optimality				  = 1;
						size_t number_of_walks				  = walk_optimality;
						size_t number_of_walks_involving_node = 0;
						if (Walk_involves_node_at_time(&walk.result.walk, node, time)) {
							number_of_walks_involving_node += walk_optimality;
						}
						if (number_of_walks == 0) {
							continue;
						}
						betweenness += (double)number_of_walks_involving_node / (double)number_of_walks;
					}
				}
				String str = WalkInfoArrayList_to_string(&optimal_walks);
				String_destroy(str);
				WalkInfoArrayList_destroy(optimal_walks);
			}
		}
	}
	return betweenness;
}

double Walk_length_integral(Walk* walk) {
	return (double)Walk_length(walk);
}

// TODO: merge it with robustness by duration
double Stream_robustness_by_length(Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	size_t begin		= fns.lifespan(stream->stream_data).start;

	QueueInfoArrayList queue	  = QueueInfoArrayList_with_capacity(1);
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	Arena arena					  = Arena_init();

	double robustness = 0.0;

	NodeIdArrayList nodes = SGA_collect_node_ids(fns.nodes_set(stream->stream_data));

	// printf("Number of nodes : %zu\n", nodes.length);
	for (size_t f = 0; f < nodes.length; f++) {
		NodeId from = nodes.array[f];
		printf("From %zu\n", from);
		for (size_t t = 0; t < nodes.length; t++) {
			printf("To %zu\n", t);
			NodeId to = nodes.array[t];
			if (from == to) {
				TimesIterator presence = fns.times_node_present(stream->stream_data, from);
				FOR_EACH_TIME(p, presence) {
					robustness += (double)(p.end - p.start); // 1 if exists, 0 if not
				}
			}
			else {
				TimeId current_time = 0;
				while (current_time != max_lifespan) {
					// printf("Current time %zu\n", current_time);
					WalkInfo optimal;
					// ArenaArrayList_clear(&arena);
					QueueInfoArrayList_clear(&queue);
					ExploredStateHashset_clear(&explored);
					Stream_shortest_walk_from_to_at_buffered(stream, from, to, current_time, &optimal, &arena, &queue, &explored);
					size_t previous_time = current_time;
					if (optimal.type == NO_WALK) {
						NoWalkReason error = optimal.result.no_walk_reason;
						if (error.type == NODE_ABSENT) {
							current_time = error.reason.node_absent_between.end;
							continue;
						}
						else if (error.type == UNREACHABLE) {
							WalkInfo_destroy(optimal);
							Arena_clear(&arena);
							goto after_while;
						}
					}
					else if (optimal.type == WALK) {
						Interval optimality = optimal.result.walk.optimality;
						current_time		= optimality.end;
						robustness += (double)Interval_duration(optimality) / Walk_length_integral(&optimal.result.walk);
					}
					if (current_time == previous_time) {
						current_time++;
					}

					WalkInfo_destroy(optimal);
					Arena_clear(&arena); // OPTIMISE: reuse it
				}
			after_while:
			}
		}
	}
	NodeIdArrayList_destroy(nodes);
	ExploredStateHashset_destroy(explored);
	QueueInfoArrayList_destroy(queue);
	Arena_destroy(arena);

	return robustness / (double)(nodes.length * nodes.length * (max_lifespan - begin));
}

double Stream_robustness_by_duration(Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	size_t begin		= fns.lifespan(stream->stream_data).start;

	QueueInfoArrayList queue	  = QueueInfoArrayList_with_capacity(1);
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	Arena arena					  = Arena_init();

	double robustness = 0.0;

	NodeIdArrayList nodes = SGA_collect_node_ids(fns.nodes_set(stream->stream_data));

	// printf("Number of nodes : %zu\n", nodes.length);
	for (size_t f = 0; f < nodes.length; f++) {
		NodeId from = nodes.array[f];
		printf("From %zu\n", from);
		for (size_t t = 0; t < nodes.length; t++) {
			printf("To %zu\n", t);
			NodeId to = nodes.array[t];
			if (from == to) {
				TimesIterator presence = fns.times_node_present(stream->stream_data, from);
				FOR_EACH_TIME(p, presence) {
					robustness += (double)(p.end - p.start); // 1 if exists, 0 if not
				}
			}
			else {
				TimeId current_time = 0;
				while (current_time != max_lifespan) {
					// printf("Current time %zu\n", current_time);
					WalkInfo optimal;
					// ArenaArrayList_clear(&arena);
					QueueInfoArrayList_clear(&queue);
					ExploredStateHashset_clear(&explored);
					// Stream_shortest_walk_from_to_at_buffered(stream, from, to, current_time, &optimal,
					// &arena, &queue, 										 &explored);
					optimal = Stream_fastest_walk(stream, from, to, current_time);
					// printf("Optimal : %s\n", WalkInfo_to_string(&optimal).data);
					size_t previous_time = current_time;
					if (optimal.type == NO_WALK) {
						NoWalkReason error = optimal.result.no_walk_reason;
						if (error.type == NODE_ABSENT) {
							current_time = error.reason.node_absent_between.end;
							continue;
						}
						else if (error.type == UNREACHABLE) {
							WalkInfo_destroy(optimal);
							Arena_clear(&arena);
							goto after_while;
						}
					}
					else if (optimal.type == WALK) {
						Interval optimality = optimal.result.walk.optimality;
						current_time		= optimality.end;
						robustness +=
							(double)(Interval_duration(optimality)) / (1.0 + Walk_duration_integral_1_over_x(&optimal.result.walk));
					}
					if (current_time == previous_time) {
						current_time++;
					}

					// TODO : shit name

					WalkInfo_destroy(optimal);
					Arena_clear(&arena); // OPTIMISE: reuse it
				}
			after_while:
			}
		}
	}
	NodeIdArrayList_destroy(nodes);
	ExploredStateHashset_destroy(explored);
	QueueInfoArrayList_destroy(queue);
	Arena_destroy(arena);

	return robustness / (double)(nodes.length * nodes.length * (max_lifespan - begin));
}

bool Walk_goes_through(Walk* walk, Stream stream, size_t nb_steps, ...) {

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
	NodeId first_node = va_arg(args, NodeId);
	Link first_link	  = fns.link_by_id(stream.stream_data, walk->steps.array[0].link);
	// printf("Checking first node %zu\n", first_node);
	if (first_link.nodes[0] != first_node && first_link.nodes[1] != first_node) {
		// printf("First node %zu FALSE\n", first_node);
		return false;
	}

	// Check the intermediate nodes
	for (size_t i = 1; i < nb_steps; i++) {
		NodeId node = va_arg(args, NodeId);
		// printf("Checking node %zu\n", node);
		Link link = fns.link_by_id(stream.stream_data, walk->steps.array[i].link);
		if (link.nodes[0] != node && link.nodes[1] != node) {
			// printf("Node %zu FALSE\n", node);
			return false;
		}
	}

	// Check the last node
	NodeId last_node = va_arg(args, NodeId);
	Link last_link	 = fns.link_by_id(stream.stream_data, walk->steps.array[nb_steps - 1].link);
	// printf("Checking last node %zu\n", last_node);
	if (last_link.nodes[0] != last_node && last_link.nodes[1] != last_node) {
		// printf("Last node %zu FALSE\n", last_node);
		return false;
	}

	va_end(args);
	return true;
}

Walk WalkInfo_unwrap_unchecked(WalkInfo info) {
	return info.result.walk;
}

Walk WalkInfo_unwrap_checked(WalkInfo info) {
	assert(info.type == WALK);
	return WalkInfo_unwrap_unchecked(info);
}

TimeId Walk_arrives_at(Walk* walk) {
	// printf("Walk size : %zu\n", walk->steps.length);
	// printf("Walk arrival time : %zu\n", walk->steps.array[walk->steps.length - 1].time);
	return walk->steps.array[walk->steps.length - 1].time;
}