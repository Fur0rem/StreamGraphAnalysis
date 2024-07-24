#include "walks.h"
#include "arena.h"
#include "hashset.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream_functions.h"

// For STREAM_FUNCS
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream_graph.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include "utils.h"
#include "vector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool TreeEdge_equals(TreeEdge a, TreeEdge b) {
	return a.weight == b.weight && a.child->id == b.child->id;
}

char* TreeEdge_to_string(TreeEdge* edge) {
	char* str = malloc(100);
	sprintf(str, "TreeEdge(%zu, %zu)", edge->weight, edge->child->id);
	return str;
}

// Breadth-first search for shortest walk

typedef struct QueueInfo QueueInfo;
struct QueueInfo {
	NodeId node;
	size_t time;
	size_t depth;
	Interval interval_taken;
	QueueInfo* previous;
};

bool QueueInfo_equals(QueueInfo a, QueueInfo b) {
	return a.node == b.node && a.time == b.time && a.previous == b.previous &&
		   Interval_equals(a.interval_taken, b.interval_taken) && a.depth == b.depth;
}

char* QueueInfo_to_string(QueueInfo* info) {
	char* str = malloc(100);
	sprintf(str, "QueueInfo(node:%zu, time:%zu, interval taken:%s, depth:%zu)", info->node, info->time,
			Interval_to_string(&info->interval_taken), info->depth);
	return str;
}

char* WalkStep_to_string(WalkStep* step) {
	char* str = malloc(100);
	sprintf(str, "WalkStep(link:%zu, time:%zu, needs_to_arrive_before:%zu)", step->link, step->time,
			step->needs_to_arrive_before);
	return str;
}

bool WalkStep_equals(WalkStep a, WalkStep b) {
	return a.link == b.link && a.time == b.time;
}

DefVector(QueueInfo, NO_FREE(QueueInfo));

size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

typedef struct {
	NodeId node;
	TimeId time;
} ExploredState;

bool ExploredState_equals(ExploredState a, ExploredState b) {
	return a.node == b.node && a.time == b.time;
}

char* ExploredState_to_string(ExploredState* state) {
	char* str = malloc(100);
	sprintf(str, "ExploredState(node:%zu, time:%zu)", state->node, state->time);
	return str;
}

size_t ExploredState_hash(ExploredState state) {
	return state.node * 31 + state.time;
}

DefHashset(ExploredState, ExploredState_hash, NO_FREE(ExploredState));

typedef char char2;
DEFAULT_COMPARE(char2)
DEFAULT_TO_STRING(char2, "%c")
DefVector(char2, NO_FREE(char2));

#define APPEND_CONST(str) str, sizeof(str) - 1

char* Walk_to_string(Walk* walk) {
	if (walk->steps.size == 0) {
		char* str = malloc(100);
		sprintf(str, "No walk from %zu to %zu at %zu\n", walk->start, walk->end, walk->optimality.start);
		return str;
	}
	char2Vector str = char2Vector_with_capacity(100);
	char2Vector_append(&str, APPEND_CONST("Walk from "));
	char buf[100];
	sprintf(buf, "%zu", walk->start);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST(" to "));
	sprintf(buf, "%zu", walk->end);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST(" at "));
	char* time_str = Interval_to_string(&walk->optimality);
	sprintf(buf, "Optimal at %s\n", time_str);
	free(time_str);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST("\n"));

	FullStreamGraph* fsg = (FullStreamGraph*)walk->stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	for (size_t i = 0; i < walk->steps.size; i++) {
		WalkStep step = walk->steps.array[i];
		sprintf(buf, "%zu", step.link);
		char2Vector_append(&str, buf, strlen(buf));
		NodeId from = sg.links.links[step.link].nodes[0];
		NodeId to = sg.links.links[step.link].nodes[1];
		sprintf(buf, " (%zu -> %zu)", from, to);
		char2Vector_append(&str, buf, strlen(buf));
		char2Vector_append(&str, APPEND_CONST(" @ "));
		sprintf(buf, "%zu", step.time);
		char2Vector_append(&str, buf, strlen(buf));
		char2Vector_append(&str, APPEND_CONST("\n"));
	}
	char2Vector_push(&str, '\0');
	return str.array;
}

bool Walk_equals(Walk a, Walk b) {
	for (size_t i = 0; i < a.steps.size; i++) {
		if (!WalkStep_equals(a.steps.array[i], b.steps.array[i])) {
			return false;
		}
	}
	return true;
}

char* WalkInfo_to_string(WalkInfo* wi) {
	if (wi->type == WALK) {
		char* str = Walk_to_string(&wi->walk_or_reason.walk);
		char* result = malloc(strlen(str) + 100);
		sprintf(result, "WalkInfo(WALK, %s)", str);
		free(str);
		return result;
	}
	else {
		char* result = malloc(200);
		if (wi->walk_or_reason.no_walk_reason.type == NODE_DOESNT_EXIST) {
			NodeDoesntExistInfo info = wi->walk_or_reason.no_walk_reason.reason.node_doesnt_exist;
			sprintf(result, "WalkInfo(NO_WALK, NODE_DOESNT_EXIST in interval %s)", Interval_to_string(&info.interval));
		}
		else {
			ImpossibleToReachInfo info = wi->walk_or_reason.no_walk_reason.reason.impossible_to_reach;
			sprintf(result, "WalkInfo(NO_WALK, IMPOSSIBLE_TO_REACH after %zu)", info.impossible_after);
		}
		return result;
	}
}
bool WalkInfo_equals(WalkInfo a, WalkInfo b) {
	if (a.type != b.type) {
		return false;
	}
	if (a.type == WALK) {
		return Walk_equals(a.walk_or_reason.walk, b.walk_or_reason.walk);
	}
	else {
		return a.walk_or_reason.no_walk_reason.type == b.walk_or_reason.no_walk_reason.type;
	}
}

WalkInfoVector optimal_walks_between_two_nodes(Stream* stream, NodeId from, NodeId to,
											   WalkInfo (*fn)(Stream*, NodeId, NodeId, TimeId)) {
	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;
	WalkInfoVector walks = WalkInfoVector_with_capacity(1);
	TimeId current_time = 0;
	while (current_time != StreamGraph_lifespan_end(&sg)) {
		printf("Finding optimal walk from %zu to %zu at %zu\n", from, to, current_time);
		WalkInfo optimal = fn(stream, from, to, current_time);
		WalkInfoVector_push(&walks, optimal);
		if (optimal.type == NO_WALK) {
			NoWalkReason error = optimal.walk_or_reason.no_walk_reason;
			if (error.type == NODE_DOESNT_EXIST) {
				current_time = error.reason.node_doesnt_exist.interval.end;
			}
			else if (error.type == IMPOSSIBLE_TO_REACH) {
				return walks;
			}
		}
		else if (optimal.type == WALK) {
			Interval optimality = optimal.walk_or_reason.walk.optimality;
			current_time = optimality.end;
		}
	}
	return walks;
}

// TODO : this doesn't work (why i copy pasted the other one ???)
WalkStepVector WalkStepVector_from_candidates(Stream* stream, QueueInfo* candidates, NodeId from, NodeId to) {
	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;
	WalkStepVector steps = WalkStepVector_with_capacity(1);
	QueueInfo* current_walk = candidates;
	while (current_walk != NULL) {
		NodeId current_node = current_walk->node;
		QueueInfo* previous = current_walk->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		// TODO : make a function that does finds the link between two nodes for a given stream cause i use that a lot
		for (size_t i = 0; i < sg.links.nb_links; i++) {
			Link link = sg.links.links[i];
			if ((link.nodes[0] == current_node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_node && link.nodes[0] == previous_node)) {
				WalkStep step = {i, current_walk->time, .needs_to_arrive_before = current_walk->interval_taken.end};
				WalkStepVector_push(&steps, step);
				break;
			}
		}
		current_walk = previous;
	}

	WalkStepVector_reverse(&steps);
	return steps;
}

WalkInfo unreachable_after(TimeId after) {
	NoWalkReason reason = {
		.type = IMPOSSIBLE_TO_REACH,
		.reason.impossible_to_reach.impossible_after = after, // TODO : rename impossible to reach to unreachable
	};
	return (WalkInfo){
		.type = NO_WALK,
		.walk_or_reason.no_walk_reason = reason,
	};
}

WalkInfo node_doesnt_exist_in_interval(Interval interval) {
	NoWalkReason reason = {
		.type = NODE_DOESNT_EXIST,
		.reason.node_doesnt_exist.interval = interval,
	};
	return (WalkInfo){
		.type = NO_WALK,
		.walk_or_reason.no_walk_reason = reason,
	};
}

WalkInfo walk_exists(Walk walk) {
	return (WalkInfo){
		.type = WALK,
		.walk_or_reason.walk = walk,
	};
}

// Minimal number of hops between two nodes
WalkInfo Stream_shortest_walk_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at) {
	// printf("Shortest walk from %zu to %zu at %zu\n", from, to, at);
	WalkInfo result;

	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	ArenaVector arena = ArenaVector_init();

	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	// Initialize the queue with the starting node
	QueueInfoVector queue = QueueInfoVector_with_capacity(1);
	size_t max_lifespan = StreamGraph_lifespan_end(&sg);
	QueueInfo start = {from, current_time, .interval_taken = Interval_from(at, max_lifespan), .previous = NULL};
	QueueInfoVector_push(&queue, start);

	NodeId current_candidate = from;
	QueueInfo current_info;
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	while ((current_candidate != to) && (!QueueInfoVector_is_empty(queue))) {
		// Get the next node from the queue
		current_info = QueueInfoVector_pop_front(&queue);

		// TODO : maybe there is a better solution than hashsets to not loop between the same nodes?
		if (ExploredStateHashset_contains(explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(&explored, (ExploredState){current_info.node, current_info.time});
		current_candidate = current_info.node;
		current_time = current_info.time;

		// FIXME: do with iterators but they segfault for some reason :(
		// Get its neighbors
		TemporalNode n = sg.nodes.nodes[current_candidate];
		for (size_t i = 0; i < n.nb_neighbours; i++) {
			size_t neighbor = n.neighbours[i];
			Link l = sg.links.links[neighbor];
			for (size_t j = 0; j < l.presence.nb_intervals; j++) {
				Interval interval = l.presence.intervals[j];
				bool can_cross_now = Interval_contains(interval, current_time);
				// TODO : add verification if node still exists by then
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					Link link = l;
					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous = ArenaVector_alloc(&arena, sizeof(QueueInfo));
					*previous = current_info;
					TimeId time_crossed = can_cross_now ? current_time : interval.start;
					QueueInfo neighbor_info = {neighbor_id, time_crossed, .interval_taken = interval,
											   .previous = previous};
					QueueInfoVector_push(&queue, neighbor_info);
				}
			}
		}
	}

	if (QueueInfoVector_is_empty(queue)) { // No walk found
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	Walk walk = {
		.start = from,
		.end = to,
		.optimality = Interval_from(at, StreamGraph_lifespan_end(&sg)),
		.stream = stream,
		.steps = WalkStepVector_from_candidates(stream, &current_info, from, to),
	};

	// OPTIMISE: dont look through them all and propagate, just do a regular min and cut when you have to wait
	for (size_t i = 0; i < walk.steps.size - 1; i++) {
		WalkStep* step = &walk.steps.array[i];
		step->needs_to_arrive_before =
			min(step->needs_to_arrive_before, walk.steps.array[i + 1].needs_to_arrive_before);
	}
	walk.optimality.end = walk.steps.array[0].needs_to_arrive_before;

	result = walk_exists(walk);

cleanup_and_return:
	QueueInfoVector_destroy(queue);
	ExploredStateHashset_destroy(explored);
	ArenaVector_destroy(arena);
	return result;
}

WalkInfo Stream_fastest_shortest_walk(Stream* stream, NodeId from, NodeId to, TimeId at) {
	WalkInfo result;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	ArenaVector arena = ArenaVector_init();

	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	// Initialize the queue with the starting node
	QueueInfoVector queue = QueueInfoVector_with_capacity(1);
	QueueInfo start = {from, current_time, 0, .interval_taken = Interval_from(at, StreamGraph_lifespan_end(&sg)),
					   .previous = NULL};
	QueueInfoVector_push(&queue, start);

	NodeId current_candidate = from;
	QueueInfo current_info;
	size_t best_time_yet = SIZE_MAX;
	QueueInfo best_yet;
	bool found = false;
	size_t depth_found = SIZE_MAX;
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	while ((!QueueInfoVector_is_empty(queue)) && ((!found) || (current_info.depth <= depth_found))) {
		// Get the next node from the queue
		current_info = QueueInfoVector_pop_front(&queue);
		if (ExploredStateHashset_contains(explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(&explored, (ExploredState){current_info.node, current_info.time});

		current_candidate = current_info.node;
		current_time = current_info.time;

		// FIXME: do with iterators but they segfault for some reason :(
		// Get its neighbors
		TemporalNode n = sg.nodes.nodes[current_candidate];
		for (size_t i = 0; i < n.nb_neighbours; i++) {
			size_t neighbor = n.neighbours[i];
			Link l = sg.links.links[neighbor];
			for (size_t j = 0; j < l.presence.nb_intervals; j++) {
				Interval interval = l.presence.intervals[j];
				bool can_cross_now = Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					Link link = l;
					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous = ArenaVector_alloc(&arena, sizeof(QueueInfo));
					*previous = current_info;
					TimeId time_crossed = can_cross_now ? current_time : interval.start;
					QueueInfo neighbor_info = {neighbor_id, time_crossed, current_info.depth + 1,
											   .interval_taken = interval, .previous = previous};
					QueueInfoVector_push(&queue, neighbor_info);
				}
			}
		}
		if (current_candidate == to) {
			found = true;
			depth_found = current_info.depth;
			if (current_time < best_time_yet) {
				best_time_yet = current_time;
				best_yet = current_info;
			}
		}
	}

	if (QueueInfoVector_is_empty(queue)) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	Walk walk = {
		.start = from,
		.end = to,
		.optimality = Interval_from(at, StreamGraph_lifespan_end(&sg)),
		.stream = stream,
		.steps = WalkStepVector_from_candidates(stream, &best_yet, from, to),
	};

	// propagate the optimal intervals
	if (walk.steps.size == 0) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	for (size_t i = 0; i < walk.steps.size - 1; i++) {
		WalkStep* step = &walk.steps.array[i];
		step->needs_to_arrive_before =
			min(step->needs_to_arrive_before, walk.steps.array[i + 1].needs_to_arrive_before);
	}

	walk.optimality.end = walk.steps.array[0].needs_to_arrive_before;

	result = walk_exists(walk);

cleanup_and_return:
	QueueInfoVector_destroy(queue);
	ExploredStateHashset_destroy(explored);
	ArenaVector_destroy(arena);
	return result;
}

void Walk_destroy(Walk walk) {
	WalkStepVector_destroy(walk.steps);
}

void WalkInfo_destroy(WalkInfo wi) {
	if (wi.type == WALK) {
		Walk_destroy(wi.walk_or_reason.walk);
	}
}