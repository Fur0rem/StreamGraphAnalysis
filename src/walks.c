#include "walks.h"
#include "arena.h"
#include "hashset.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream_functions.h"

// For STREAM_FUNCS
#include "metrics.h"
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream/snapshot_stream.h"
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
	sprintf(str, "WalkStep(link:%zu, time:%zu, needs_to_arrive_before:%zu)", step->link, step->time,
			step->needs_to_arrive_before);
	return String_from_owned(str);
}

bool WalkStep_equals(const WalkStep* a, const WalkStep* b) {
	return a->link == b->link && a->time == b->time;
}

// DefVector(QueueInfo, NO_FREE(QueueInfo));
DeclareVector(QueueInfo);
DefineVector(QueueInfo);
DefineVectorDeriveRemove(QueueInfo, NO_FREE(QueueInfo));

size_t min(size_t a, size_t b) {
	return a < b ? a : b;
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
DeclareVector(ExploredState);
DefineVector(ExploredState);
DefineVectorDeriveRemove(ExploredState, NO_FREE(ExploredState));
DeclareHashset(ExploredState);
DefineHashset(ExploredState);
DefineHashsetDeriveRemove(ExploredState, NO_FREE(ExploredState));

#define APPEND_CONST(str) str, sizeof(str) - 1

String Walk_to_string(const Walk* walk) {
	if (walk->steps.size == 0) {
		char* buf = malloc(100);
		sprintf(buf, "No walk from %zu to %zu at %zu\n", walk->start, walk->end, walk->optimality.start);
		String str = String_from_owned(buf);
		return str;
	}
	String str = String_from_duplicate("");
	String_push_str(&str, "Walk from ");
	char buf[100];
	sprintf(buf, "%zu", walk->start);
	// char2Vector_append(&str, buf, strlen(buf));
	String_push_str(&str, buf);
	// char2Vector_append(&str, APPEND_CONST(" to "));
	String_push_str(&str, " to ");
	sprintf(buf, "%zu", walk->end);
	// char2Vector_append(&str, buf, strlen(buf));
	String_push_str(&str, buf);
	// char2Vector_append(&str, APPEND_CONST(" at "));
	String_push_str(&str, " at ");
	sprintf(buf, "%zu", walk->optimality.start);
	// char2Vector_append(&str, buf, strlen(buf));
	String_push_str(&str, buf);
	// char* time_str = Interval_to_string(&walk->optimality);
	String time_str = Interval_to_string(&walk->optimality);
	// sprintf(buf, "Optimal at %s\n", time_str.data);
	// free(time_str);
	// char2Vector_append(&str, buf, strlen(buf));
	// char2Vector_append(&str, APPEND_CONST("\n"));
	String_push_str(&str, "Optimal at ");
	String_concat_consume(&str, &time_str);
	String_push(&str, '\n');

	FullStreamGraph* fsg = (FullStreamGraph*)walk->stream->stream_data;
	StreamGraph sg = *fsg->underlying_stream_graph;

	for (size_t i = 0; i < walk->steps.size; i++) {
		WalkStep step = walk->steps.array[i];
		sprintf(buf, "%zu", step.link);
		// char2Vector_append(&str, buf, strlen(buf));
		String_push_str(&str, buf);
		NodeId from = sg.links.links[step.link].nodes[0];
		NodeId to = sg.links.links[step.link].nodes[1];
		sprintf(buf, " (%zu -> %zu)", from, to);
		String_push_str(&str, buf);
		String_push_str(&str, " @ ");
		// char2Vector_append(&str, buf, strlen(buf));
		// char2Vector_append(&str, APPEND_CONST(" @ "));
		sprintf(buf, "%zu", step.time);
		// char2Vector_append(&str, buf, strlen(buf));
		String_push_str(&str, buf);
		// char2Vector_append(&str, APPEND_CONST("\n"));
		String_push(&str, '\n');
	}
	// char2Vector_push(&str, '\0');
	return str;
}

bool Walk_equals(const Walk* a, const Walk* b) {
	for (size_t i = 0; i < a->steps.size; i++) {
		if (!WalkStep_equals(&a->steps.array[i], &b->steps.array[i])) {
			return false;
		}
	}
	return true;
}

DeclareVector(Walk);
DefineVector(Walk);
DefineVectorDeriveRemove(Walk, Walk_destroy);
DefineVectorDeriveToString(Walk);
DefineVectorDeriveEquals(Walk);

String WalkInfo_to_string(const WalkInfo* wi) {
	if (wi->type == WALK) {
		// char* result = malloc(strlen(str) + 100);
		// sprintf(result, "WalkInfo(WALK, %s)", str);
		// free(str);
		String walk_str = Walk_to_string(&wi->walk_or_reason.walk);
		String str = String_from_duplicate("WalkInfo(WALK, ");
		String_concat_consume(&str, &walk_str);
		String_push(&str, ')');
		return str;
	}
	else {
		// char* result = malloc(200);
		// if (wi->walk_or_reason.no_walk_reason.type == NODE_DOESNT_EXIST) {
		// 	NodeDoesntExistInfo info = wi->walk_or_reason.no_walk_reason.reason.node_doesnt_exist;
		// 	sprintf(result, "WalkInfo(NO_WALK, NODE_DOESNT_EXIST in interval %s)", Interval_to_string(&info.interval));
		// }
		// else {
		// 	ImpossibleToReachInfo info = wi->walk_or_reason.no_walk_reason.reason.impossible_to_reach;
		// 	sprintf(result, "WalkInfo(NO_WALK, IMPOSSIBLE_TO_REACH after %zu)", info.impossible_after);
		// }
		// return result;
		String str = String_from_duplicate("WalkInfo(NO_WALK, ");
		if (wi->walk_or_reason.no_walk_reason.type == NODE_DOESNT_EXIST) {
			NodeDoesntExistInfo info = wi->walk_or_reason.no_walk_reason.reason.node_doesnt_exist;
			String interval_str = Interval_to_string(&info.interval);
			String_push_str(&str, "NODE_DOESNT_EXIST in interval ");
			String_concat_copy(&str, &interval_str);
		}
		else {
			ImpossibleToReachInfo info = wi->walk_or_reason.no_walk_reason.reason.impossible_to_reach;
			char buf[100];
			sprintf(buf, "IMPOSSIBLE_TO_REACH after %zu", info.impossible_after);
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
		return Walk_equals(&a->walk_or_reason.walk, &b->walk_or_reason.walk);
	}
	else {
		return a->walk_or_reason.no_walk_reason.type == b->walk_or_reason.no_walk_reason.type;
	}
}

WalkInfoVector optimal_walks_between_two_nodes(Stream* stream, NodeId from, NodeId to,
											   WalkInfo (*fn)(Stream*, NodeId, NodeId, TimeId)) {
	WalkInfoVector walks = WalkInfoVector_with_capacity(1);
	TimeId current_time = 0;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	while (current_time != max_lifespan) {
		size_t previous_time = current_time;
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
		if (current_time == previous_time) {
			current_time++;
		}
	}
	return walks;
}

void WalkInfo_destroy(WalkInfo wi) {
	if (wi.type == WALK) {
		Walk_destroy(wi.walk_or_reason.walk);
	}
}

// TODO : put it in the right place
DefineVector(WalkStep);
DefineVectorDeriveRemove(WalkStep, NO_FREE(WalkStep));
DefineVectorDeriveToString(WalkStep);

DefineVector(WalkInfo);
DefineVectorDeriveRemove(WalkInfo, WalkInfo_destroy);
DefineVectorDeriveToString(WalkInfo);

// TODO : this doesn't work (why i copy pasted the other one ???)
WalkStepVector WalkStepVector_from_candidates(Stream* stream, QueueInfo* candidates, NodeId from, NodeId to) {
	WalkStepVector steps = WalkStepVector_with_capacity(1);
	QueueInfo* current_walk = candidates;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	while (current_walk != NULL) {
		NodeId current_node = current_walk->node;
		QueueInfo* previous = current_walk->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		// TODO : make a function that does finds the link between two nodes for a given stream cause i use that a lot
		/*for (size_t i = 0; i < sg.links.nb_links; i++) {
			Link link = sg.links.links[i];*/
		LinksIterator links = fns.links_set(stream->stream_data);
		FOR_EACH_LINK(i, links) {
			Link link = fns.link_by_id(stream->stream_data, i);
			if ((link.nodes[0] == current_node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_node && link.nodes[0] == previous_node)) {
				WalkStep step = {i, current_walk->time, .needs_to_arrive_before = current_walk->interval_taken.end};
				WalkStepVector_push(&steps, step);
				links.destroy(&links);
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

	// Initialize the queue with the starting node
	QueueInfoVector queue = QueueInfoVector_with_capacity(1);
	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	QueueInfo start = {from, current_time, .interval_taken = Interval_from(at, max_lifespan), .previous = NULL};
	QueueInfoVector_push(&queue, start);

	NodeId current_candidate = from;
	QueueInfo current_info;
	ExploredStateHashset explored = ExploredStateHashset_with_capacity(50);
	while ((current_candidate != to) && (!QueueInfoVector_is_empty(queue))) {
		// Get the next node from the queue
		current_info = QueueInfoVector_pop_first(&queue);

		// TODO : maybe there is a better solution than hashsets to not loop between the same nodes?
		if (ExploredStateHashset_contains(explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(&explored, (ExploredState){current_info.node, current_info.time});
		current_candidate = current_info.node;
		current_time = current_info.time;

		// FIXME: do with iterators but they segfault for some reason :(
		// Get its neighbors
		// TemporalNode n = sg.nodes.nodes[current_candidate];
		/*for (size_t i = 0; i < n.nb_neighbours; i++) {
			size_t neighbor = n.neighbours[i];
			Link l = sg.links.links[neighbor];*/
		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		// printf("Neighbours of %zu\n", current_candidate);
		// printf("linksiterators : %p", neighbours.stream_graph.stream);
		FOR_EACH_LINK(link_id, neighbours) {
			// Link l = sg.links.links[link_id];
			TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			// for (size_t j = 0; j < l.presence.nb_intervals; j++) {
			FOR_EACH_TIME(interval, times_link_present) {
				// Interval interval = l.presence.intervals[j];
				bool can_cross_now = Interval_contains(interval, current_time);
				// TODO : add verification if node still exists by then
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					Link link = fns.link_by_id(stream->stream_data, link_id);
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
		.optimality = Interval_from(at, max_lifespan),
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

	// Initialize the queue with the starting node
	QueueInfoVector queue = QueueInfoVector_with_capacity(1);
	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	QueueInfo start = {from, current_time, 0, .interval_taken = Interval_from(at, max_lifespan), .previous = NULL};
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
		current_info = QueueInfoVector_pop_first(&queue);
		if (ExploredStateHashset_contains(explored, (ExploredState){current_info.node, current_info.time})) {
			continue;
		}
		ExploredStateHashset_insert(&explored, (ExploredState){current_info.node, current_info.time});

		current_candidate = current_info.node;
		current_time = current_info.time;

		// Get its neighbors
		// TemporalNode n = sg.nodes.nodes[current_candidate];
		// for (size_t i = 0; i < n.nb_neighbours; i++) {
		// 	size_t neighbor = n.neighbours[i];
		// 	Link l = sg.links.links[neighbor];
		// 	for (size_t j = 0; j < l.presence.nb_intervals; j++) {
		// 		Interval interval = l.presence.intervals[j];
		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		FOR_EACH_LINK(link_id, neighbours) {
			Link link = fns.link_by_id(stream->stream_data, link_id);
			TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times_link_present) {
				bool can_cross_now = Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
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
		.optimality = Interval_from(at, max_lifespan),
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

/*typedef struct {
	NodeId node;
	size_t distance;
	size_t number_of_jumps;
} DijkstraState;

bool DijkstraState_equals(DijkstraState a, DijkstraState b) {
	return a.node == b.node && a.distance == b.distance;
}

char* DijkstraState_to_string(DijkstraState* state) {
	char* str = malloc(100);
	sprintf(str, "DijkstraState(node:%zu, distance:%zu, number_of_jumps:%zu)", state->node, state->distance,
			state->number_of_jumps);
	return str;
}

size_t DijkstraState_hash(DijkstraState state) {
	return state.node * 31 + state.distance;
}

DefHashset(DijkstraState, DijkstraState_hash, NO_FREE(DijkstraState));*/

typedef struct DijkstraState DijkstraState;
struct DijkstraState {
	NodeId node;
	size_t time;
	size_t number_of_jumps;
	DijkstraState* previous;
};

bool DijkstraState_equals(DijkstraState* a, DijkstraState* b) {
	return a->node == b->node && a->time == b->time && a->number_of_jumps == b->number_of_jumps;
}

String DijkstraState_to_string(DijkstraState* state) {
	char* str = malloc(100);
	sprintf(str, "DijkstraState(node:%zu, time:%zu, number_of_jumps:%zu)", state->node, state->time,
			state->number_of_jumps);
	return String_from_owned(str);
}

size_t DijkstraState_hash(const DijkstraState* state) {
	return state->node * 31 + state->time;
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

// DefHashset(DijkstraState, DijkstraState_hash, NO_FREE(DijkstraState));
DeclareVector(DijkstraState);
DefineVector(DijkstraState);
DefineVectorDeriveRemove(DijkstraState, NO_FREE(DijkstraState));
DefineVectorDeriveToString(DijkstraState);
DefineVectorDeriveOrdered(DijkstraState);

DeclareHashset(DijkstraState);
DefineHashset(DijkstraState);
DefineHashsetDeriveRemove(DijkstraState, NO_FREE(DijkstraState));
DefineHashsetDeriveEquals(DijkstraState);
DefineHashsetDeriveToString(DijkstraState);

typedef struct {
	DijkstraStateVector nodes;
} BinaryHeap;

BinaryHeap BinaryHeap_init() {
	return (BinaryHeap){
		.nodes = DijkstraStateVector_with_capacity(1),
	};
}

/*void BinaryHeap_push(BinaryHeap* heap, DijkstraState state) {
	DijkstraStateVector_push(&heap->nodes, state);
	// if you find any dijkstra state with the same node and a lower time or number of jumps, replace it
	for (size_t i = 0; i < heap->nodes.size; i++) {
		DijkstraState current = heap->nodes.array[i];
		if (current.node == state.node) {
			if (DijkstraState_compare(state, current) < 0) {
				heap->nodes.array[i] = state;
				break;
			}
		}
	}
	size_t current_index = heap->nodes.size - 1;
	while (current_index != 0) {
		size_t parent_index = (current_index - 1) / 2;
		DijkstraState current = heap->nodes.array[current_index];
		DijkstraState parent = heap->nodes.array[parent_index];
		if (DijkstraState_compare(current, parent) < 0) {
			heap->nodes.array[current_index] = parent;
			heap->nodes.array[parent_index] = current;
			current_index = parent_index;
		}
		else {
			break;
		}
	}
}

DijkstraState BinaryHeap_pop(BinaryHeap* heap) {
	DijkstraState result = heap->nodes.array[0];
	heap->nodes.array[0] = heap->nodes.array[heap->nodes.size - 1];
	heap->nodes.size--;
	size_t current_index = 0;
	while (true) {
		size_t left_child_index = 2 * current_index + 1;
		size_t right_child_index = 2 * current_index + 2;
		if (left_child_index >= heap->nodes.size) {
			break;
		}
		size_t smallest_child_index = left_child_index;
		if (right_child_index < heap->nodes.size &&
			heap->nodes.array[right_child_index].time < heap->nodes.array[left_child_index].time) {
			smallest_child_index = right_child_index;
		}
		DijkstraState current = heap->nodes.array[current_index];
		DijkstraState smallest_child = heap->nodes.array[smallest_child_index];
		if (current.time > smallest_child.time) {
			heap->nodes.array[current_index] = smallest_child;
			heap->nodes.array[smallest_child_index] = current;
			current_index = smallest_child_index;
		}
		else {
			break;
		}
	}
	return result;
}*/

// OPTIMISE: do an actual heap dumbass
void BinaryHeap_push(BinaryHeap* heap, DijkstraState state) {
	DijkstraStateVector_push(&heap->nodes, state);
	// sort the heap
	DijkstraStateVector_sort(&heap->nodes);
}

DijkstraState BinaryHeap_pop(BinaryHeap* heap) {
	DijkstraState result = heap->nodes.array[0];
	DijkstraStateVector_remove(&heap->nodes, 0);
	return result;
}

bool BinaryHeap_is_empty(BinaryHeap* heap) {
	return heap->nodes.size == 0;
}

void BinaryHeap_destroy(BinaryHeap heap) {
	DijkstraStateVector_destroy(heap.nodes);
}

String BinaryHeap_to_string(BinaryHeap* heap) {
	return DijkstraStateVector_to_string(&heap->nodes);
}

// Dijkstra's algorithm to find the minimum delay between two nodes
// Its also the shortest fastest
WalkInfo Stream_fastest_walk(Stream* stream, NodeId from, NodeId to, TimeId at) {
	WalkInfo result;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	ArenaVector arena = ArenaVector_init();

	// Initialize the queue with the starting node
	BinaryHeap queue = BinaryHeap_init();
	DijkstraState start = {from, at, 0, .previous = NULL};
	BinaryHeap_push(&queue, start);

	NodeId current_candidate = from;
	DijkstraState current_info;
	size_t best_time_yet = SIZE_MAX;
	DijkstraState best_yet;
	// bool found = false;
	// size_t depth_found = SIZE_MAX;
	DijkstraStateHashset explored = DijkstraStateHashset_with_capacity(50);

	while ((!BinaryHeap_is_empty(&queue)) && (current_candidate != to)) {
		// Get the next node from the queue
		String heap_str = BinaryHeap_to_string(&queue);
		// printf("Heap : %s\n", heap_str.data);
		String_destroy(heap_str);

		current_info = BinaryHeap_pop(&queue);
		if (DijkstraStateHashset_contains(explored, (DijkstraState){current_info.node, current_info.time})) {
			continue;
		}
		DijkstraStateHashset_insert(&explored, (DijkstraState){current_info.node, current_info.time});

		current_candidate = current_info.node;
		current_time = current_info.time;

		// Get its neighbors
		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, current_candidate);
		FOR_EACH_LINK(link_id, neighbours) {
			Link link = fns.link_by_id(stream->stream_data, link_id);
			TimesIterator times_link_present = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times_link_present) {
				bool can_cross_now = Interval_contains(interval, current_time);
				bool will_cross_later = (interval.start > current_time);
				if (can_cross_now || will_cross_later) {
					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					DijkstraState* previous = ArenaVector_alloc(&arena, sizeof(DijkstraState));
					*previous = current_info;
					TimeId time_crossed = can_cross_now ? current_time : interval.start;
					DijkstraState neighbor_info = {neighbor_id, time_crossed, current_info.number_of_jumps + 1,
												   .previous = previous};
					BinaryHeap_push(&queue, neighbor_info);
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
			best_yet = current_info;
		}
	}

	if (BinaryHeap_is_empty(&queue)) {
		result = unreachable_after(at);
		goto cleanup_and_return;
	}

	// Build the walk
	Walk walk = {
		.start = from,
		.end = to,
		.optimality = Interval_from(at, best_time_yet),
		.stream = stream,
		.steps = WalkStepVector_with_capacity(1),
	};

	// Build the steps
	DijkstraState* current_walk = &best_yet;
	// printf("building walk\n");
	while (current_walk != NULL) {
		// char* str = DijkstraState_to_string(current_walk);
		// printf("Current walk : %s\n", str);
		// free(str);
		String str = DijkstraState_to_string(current_walk);
		// printf("Current walk : %s\n", str.data);
		String_destroy(str);
		DijkstraState* previous = current_walk->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		LinksIterator links = fns.links_set(stream->stream_data);
		FOR_EACH_LINK(i, links) {
			Link link = fns.link_by_id(stream->stream_data, i);
			if ((link.nodes[0] == current_walk->node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_walk->node && link.nodes[0] == previous_node)) {
				WalkStep step = {i, current_walk->time, .needs_to_arrive_before = current_walk->time};
				WalkStepVector_push(&walk.steps, step);
				links.destroy(&links);
				break;
			}
		}
		current_walk = previous;
	}

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
	// printf("walk optimality : [%zu, %zu]\n", walk.optimality.start, walk.optimality.end);

	result = walk_exists(walk);

	// printf("Walk : %s\n", Walk_to_string(&result.walk_or_reason.walk).data);

cleanup_and_return:
	BinaryHeap_destroy(queue);
	DijkstraStateHashset_destroy(explored);
	ArenaVector_destroy(arena);
	return result;
}

size_t Walk_length(Walk* walk) {
	return walk->steps.size;
}

size_t Walk_duration(Walk* walk) {
	if (walk->steps.size == 0) {
		return 0;
	}
	size_t start_time = walk->steps.array[0].time;
	size_t end_time = walk->steps.array[walk->steps.size - 1].time;
	return end_time - start_time;
}

// TODO: make better name idk ?
size_t Walk_duration_integral_doubled(Walk* walk) {
	// Area under the curve from [start, end] of the walk
	// The area under the affine function (duration - x) over the interval [start, end] = t
	// is ((d - t) * t) + (t^2 / 2), which can be simplified to (d * t) - (t^2 / 2)
	// we double it instead to get back to integers
	size_t optimality_begin = walk->optimality.start;
	size_t optimality_end = walk->optimality.end;
	size_t duration = Walk_duration(walk);
	size_t time_frame = optimality_end - optimality_begin;
	// size_t integral = (duration * time_frame) - (time_frame * time_frame / 2);
	size_t integral = (duration * time_frame * 2) - (time_frame * time_frame);
	return integral;
}

size_t Walk_length_integral_doubled(Walk* walk) {
	// Area under the curve from [start, end] of the walk
	// The area under the constant function (length) over the interval [start, end] = t
	// is (length * t)
	// we double it to match with the duration integral
	size_t optimality_begin = walk->optimality.start;
	size_t optimality_end = walk->optimality.end;
	size_t length = Walk_length(walk);
	size_t time_frame = optimality_end - optimality_begin;
	// size_t integral = length * time_frame;
	size_t integral = length * time_frame * 2;
	return integral;
}

bool Walk_involves_node_at_time(Walk* walk, NodeId node, double time) {
	for (size_t i = 0; i < walk->steps.size; i++) {
		WalkStep step = walk->steps.array[i];
		StreamFunctions fns = STREAM_FUNCS(fns, walk->stream);
		Link link = fns.link_by_id(walk->stream->stream_data, step.link);
		if (link.nodes[0] == node || link.nodes[1] == node) {
			if ((double)step.time <= time && time <= (double)step.needs_to_arrive_before) {
				return true;
			}
		}
	}
	return false;
}

double betweenness_of_node_at_time(Stream* stream, NodeId node, double time) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t max_lifespan = fns.lifespan(stream->stream_data).end;
	// size_t number_of_walks = 0;
	// size_t number_of_walks_involving_node = 0;
	// Compute all the walks between all the nodes
	double betweenness = 0.0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(from, nodes) {
		NodesIterator nodes2 = fns.nodes_set(stream->stream_data);
		FOR_EACH_NODE(to, nodes2) {
			if (from != to) {
				WalkInfoVector optimal_walks = optimal_walks_between_two_nodes(stream, from, to, Stream_fastest_walk);
				for (size_t i = 0; i < optimal_walks.size; i++) {
					WalkInfo walk = optimal_walks.array[i];
					if (walk.type == WALK) {
						// size_t walk_optimality =
						// 	walk.walk_or_reason.walk.optimality.end - walk.walk_or_reason.walk.optimality.start;
						size_t walk_optimality = 1;
						size_t number_of_walks = walk_optimality;
						printf("Walk from %zu to %zu : %s\n", from, to, Walk_to_string(&walk.walk_or_reason.walk).data);
						printf("Walk involves node %zu at time %f : %d\n", node, time,
							   Walk_involves_node_at_time(&walk.walk_or_reason.walk, node, time));
						size_t number_of_walks_involving_node = 0;
						if (Walk_involves_node_at_time(&walk.walk_or_reason.walk, node, time)) {
							number_of_walks_involving_node += walk_optimality;
						}
						if (number_of_walks == 0) {
							continue;
						}
						betweenness += (double)number_of_walks_involving_node / (double)number_of_walks;
					}
				}
				String str = WalkInfoVector_to_string(&optimal_walks);
				printf("Optimal walks from %zu to %zu : %s\n", from, to, str.data);
				String_destroy(str);
				WalkInfoVector_destroy(optimal_walks);
			}
		}
	}
	// double betweenness = (double)number_of_walks_involving_node / (double)number_of_walks;
	// printf("Number of walks : %zu, Number of walks involving node : %zu\n", number_of_walks,
	//    number_of_walks_involving_node);
	return betweenness;
}