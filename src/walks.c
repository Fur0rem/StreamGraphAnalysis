#include "walks.h"
#include "interval.h"
#include "iterators.h"
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
typedef struct {
	NodeId node;
	size_t time;
	Interval optimal_at;
	void* previous; // (QueueInfo*)
} QueueInfo;

bool QueueInfo_equals(QueueInfo a, QueueInfo b) {
	return a.node == b.node && a.time == b.time && a.previous == b.previous;
}

char* QueueInfo_to_string(QueueInfo* info) {
	char* str = malloc(100);
	sprintf(str, "QueueInfo(node:%zu, time:%zu, optimal_at:%s)", info->node, info->time,
			Interval_to_string(&info->optimal_at));
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
// Minimal number of hops between two nodes
Walk Stream_shortest_walk_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at) {
	// printf("Shortest walk from %zu to %zu at %zu\n", from, to, at);

	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	// Initialize the queue with the starting node
	QueueInfoVector queue = QueueInfoVector_with_capacity(1);
	size_t max_lifespan = StreamGraph_lifespan_end(&sg);
	QueueInfo start = {from, current_time, Interval_from(at, max_lifespan), NULL};
	QueueInfoVector_push(&queue, start);

	NodeId current_candidate = from;
	QueueInfo current_info;
	while ((current_candidate != to) && (!QueueInfoVector_is_empty(queue))) {

		/*printf(TEXT_BOLD "Current candidate: %zu\n" TEXT_RESET, current_candidate);

		char* str = QueueInfoVector_to_string(&queue);
		printf("Queue: %s\n", str);
		free(str);*/
		// Get the next node from the queue
		current_info = QueueInfoVector_pop_front(&queue);

		QueueInfo current_walk = current_info;
		/*printf("Candidate Walk:");
		while (current_walk.previous != NULL) {
			printf(" %zu (@ %zu)", current_walk.node, current_walk.time);
			current_walk = *(QueueInfo*)current_walk.previous;
		}
		printf("\n");*/
		current_candidate = current_info.node;
		current_time = current_info.time;

		// FIXME: do with iterators but they segfault for some reason :(
		// Get its neighbors
		/*LinksIterator neighbors = fns.neighbours_of_node(stream, current_candidate);
		FOR_EACH_LINK(neighbor, neighbors) {*/
		TemporalNode n = sg.nodes.nodes[current_candidate];
		for (size_t i = 0; i < n.nb_neighbours; i++) {
			size_t neighbor = n.neighbours[i];
			// TimesIterator times = fns.times_link_present(stream, neighbor);
			// FOR_EACH_TIME(interval, times) {
			Link l = sg.links.links[neighbor];
			for (size_t j = 0; j < l.presence.nb_intervals; j++) {
				Interval interval = l.presence.intervals[j];
				if (interval.start <= current_time && interval.end >= current_time) {
					// Link link = fns.nth_link(stream, neighbor);
					Link link = l;
					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					QueueInfo* previous = malloc(sizeof(QueueInfo));
					*previous = current_info;
					QueueInfo neighbor_info = {neighbor_id, current_time, interval, previous};
					// QueueInfo neighbor_info = {neighbor_id, current_time};
					QueueInfoVector_push(&queue, neighbor_info);
				}
				else if (interval.start > current_time) {
					// Link link = fns.nth_link(stream, neighbor);
					Link link = l;
					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					// QueueInfo neighbor_info = {neighbor_id, interval.start, current_candidate};
					QueueInfo* previous = malloc(sizeof(QueueInfo));
					*previous = current_info;
					QueueInfo neighbor_info = {neighbor_id, interval.start, interval, previous};
					QueueInfoVector_push(&queue, neighbor_info);
				}
			}
		}
	}
	/*printf("Shortest walk found\n");
	char* str = QueueInfoVector_to_string(&queue);
	printf("Queue: %s\n", str);
	free(str);*/

	// Print the walk
	printf("Shortest walk found\n");
	QueueInfo* current = &current_info;
	while (current != NULL) {
		char* info_str = QueueInfo_to_string(current);
		printf(" %s\n", info_str);
		free(info_str);
		current = current->previous;
	}
	printf("End of walk\n");

	// Build the walk
	Walk walk = {
		.start = from,
		.end = to,
		.start_time = at,
		.stream = stream,
		.steps = WalkStepVector_with_capacity(1),
	};
	QueueInfo* current_walk = &current_info;
	while (current_walk != NULL) {
		/*Link link = sg.links.links[current_walk->node];
		WalkStep step = {current_walk->node, current_walk->time};
		WalkStepVector_push(&walk.steps, step);
		current_walk = current_walk->previous;*/
		NodeId current_node = current_walk->node;
		QueueInfo* previous = current_walk->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		for (size_t i = 0; i < sg.links.nb_links; i++) {
			Link link = sg.links.links[i];
			if ((link.nodes[0] == current_node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_node && link.nodes[0] == previous_node)) {
				WalkStep step = {i, current_walk->time, .needs_to_arrive_before = current_walk->optimal_at.end};
				WalkStepVector_push(&walk.steps, step);
				break;
			}
		}

		current_walk = previous;
	}

	// reverse the walk
	for (size_t i = 0; i < walk.steps.size / 2; i++) {
		WalkStep tmp = walk.steps.array[i];
		walk.steps.array[i] = walk.steps.array[walk.steps.size - i - 1];
		walk.steps.array[walk.steps.size - i - 1] = tmp;
	}

	// propagate the optimal intervals
	for (size_t i = 0; i < walk.steps.size - 1; i++) {
		WalkStep* step = &walk.steps.array[i];
		step->needs_to_arrive_before =
			min(step->needs_to_arrive_before, walk.steps.array[i + 1].needs_to_arrive_before);
	}

	char* str = WalkStepVector_to_string(&walk.steps);
	printf("Walk: %s\n", str);
	free(str);

	return walk;
}

typedef char char2;
DEFAULT_COMPARE(char2)
DEFAULT_TO_STRING(char2, "%c")
DefVector(char2, NO_FREE(char2));

#define APPEND_CONST(str) str, sizeof(str) - 1

char* Walk_to_string(Walk* walk) {
	char2Vector str = char2Vector_with_capacity(100);
	char2Vector_append(&str, APPEND_CONST("Walk from "));
	char buf[100];
	sprintf(buf, "%zu", walk->start);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST(" to "));
	sprintf(buf, "%zu", walk->end);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST(" at "));
	sprintf(buf, "%zu", walk->start_time);
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

// Breadth-first search for shortest walk
// typedef struct {
// 	NodeId node;
// 	size_t time;
// 	size_t depth;
// 	void* previous; // (QueueInfoDepth*)
// } QueueInfoDepth;

// bool QueueInfoDepth_equals(QueueInfoDepth a, QueueInfoDepth b) {
// 	return a.node == b.node && a.time == b.time && a.previous == b.previous;
// }

// char* QueueInfoDepth_to_string(QueueInfoDepth* info) {
// 	char* str = malloc(100);
// 	sprintf(str, "QueueInfoDepth(node:%zu, time:%zu)", info->node, info->time);
// 	return str;
// }

// DefVector(QueueInfoDepth, NO_FREE(QueueInfoDepth));

// Walk Stream_fastest_shortest_walk(Stream* stream, NodeId from, NodeId to, TimeId at) {
// 	// printf("Shortest walk from %zu to %zu at %zu\n", from, to, at);

// 	StreamFunctions fns = STREAM_FUNCS(fns, stream);
// 	size_t current_time = at;

// 	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
// 	StreamGraph sg = *fsg->underlying_stream_graph;

// 	// Initialize the queue with the starting node
// 	QueueInfoDepthVector queue = QueueInfoDepthVector_with_capacity(1);
// 	QueueInfoDepth start = {from, current_time, 0, NULL};
// 	QueueInfoDepthVector_push(&queue, start);

// 	NodeId current_candidate = from;
// 	QueueInfoDepth current_info;
// 	size_t best_time_yet = SIZE_MAX;
// 	QueueInfoDepth best_yet;
// 	bool found = false;
// 	size_t depth_found = SIZE_MAX;
// 	while ((!QueueInfoDepthVector_is_empty(queue)) && ((!found) || (current_info.depth <= depth_found))) {
// 		/*printf(TEXT_BOLD "Current candidate: %zu\n" TEXT_RESET, current_candidate);

// 		char* str = QueueInfoDepthVector_to_string(&queue);
// 		printf("Queue: %s\n", str);
// 		free(str);*/
// 		// Get the next node from the queue
// 		current_info = QueueInfoDepthVector_pop_front(&queue);

// 		QueueInfoDepth current_walk = current_info;
// 		/*printf("Candidate Walk:");
// 		while (current_walk.previous != NULL) {
// 			printf(" %zu (@ %zu)", current_walk.node, current_walk.time);
// 			current_walk = *(QueueInfoDepth*)current_walk.previous;
// 		}
// 		printf(" Depth: %zu", current_info.depth);
// 		printf("\n");*/
// 		current_candidate = current_info.node;
// 		current_time = current_info.time;

// 		// FIXME: do with iterators but they segfault for some reason :(
// 		// Get its neighbors
// 		/*LinksIterator neighbors = fns.neighbours_of_node(stream, current_candidate);
// 		FOR_EACH_LINK(neighbor, neighbors) {*/
// 		TemporalNode n = sg.nodes.nodes[current_candidate];
// 		for (size_t i = 0; i < n.nb_neighbours; i++) {
// 			size_t neighbor = n.neighbours[i];
// 			// TimesIterator times = fns.times_link_present(stream, neighbor);
// 			// FOR_EACH_TIME(interval, times) {
// 			Link l = sg.links.links[neighbor];
// 			for (size_t j = 0; j < l.presence.nb_intervals; j++) {
// 				Interval interval = l.presence.intervals[j];
// 				if (interval.start <= current_time && interval.end >= current_time) {
// 					// Link link = fns.nth_link(stream, neighbor);
// 					Link link = l;
// 					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
// 					QueueInfoDepth* previous = malloc(sizeof(QueueInfoDepth));
// 					*previous = current_info;
// 					QueueInfoDepth neighbor_info = {neighbor_id, current_time, current_info.depth + 1, previous};
// 					// QueueInfoDepth neighbor_info = {neighbor_id, current_time};
// 					QueueInfoDepthVector_push(&queue, neighbor_info);
// 				}
// 				else if (interval.start > current_time) {
// 					// Link link = fns.nth_link(stream, neighbor);
// 					Link link = l;
// 					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
// 					// QueueInfoDepth neighbor_info = {neighbor_id, interval.start, current_candidate};
// 					QueueInfoDepth* previous = malloc(sizeof(QueueInfoDepth));
// 					*previous = current_info;
// 					QueueInfoDepth neighbor_info = {neighbor_id, interval.start, current_info.depth + 1, previous};
// 					QueueInfoDepthVector_push(&queue, neighbor_info);
// 				}
// 			}
// 		}
// 		if (current_candidate == to) {
// 			found = true;
// 			depth_found = current_info.depth;
// 			if (current_time < best_time_yet) {
// 				best_time_yet = current_time;
// 				best_yet = current_info;
// 			}
// 		}
// 	}
// 	/*printf(" fastest shortest walk found\n");
// 	char* str = QueueInfoDepthVector_to_string(&queue);
// 	printf("Queue: %s\n", str);
// 	free(str);*/

// 	// Print the walk
// 	/*QueueInfoDepth* current = &best_yet;
// 	while (current != NULL) {
// 		char* info_str = QueueInfoDepth_to_string(current);
// 		printf(" %s\n", info_str);
// 		free(info_str);
// 		current = current->previous;
// 	}
// 	printf("End of walk\n");*/

// 	// Build the walk
// 	Walk walk = {
// 		.start = from,
// 		.end = to,
// 		.start_time = at,
// 		.stream = stream,
// 		.steps = WalkStepVector_with_capacity(1),
// 	};
// 	QueueInfoDepth* current_walk = &best_yet;
// 	while (current_walk != NULL) {
// 		/*Link link = sg.links.links[current_walk->node];
// 		WalkStep step = {current_walk->node, current_walk->time};
// 		WalkStepVector_push(&walk.steps, step);
// 		current_walk = current_walk->previous;*/
// 		NodeId current_node = current_walk->node;
// 		QueueInfoDepth* previous = current_walk->previous;
// 		NodeId previous_node = previous == NULL ? from : previous->node;
// 		// Find the link between the current node and the previous node
// 		for (size_t i = 0; i < sg.links.nb_links; i++) {
// 			Link link = sg.links.links[i];
// 			if ((link.nodes[0] == current_node && link.nodes[1] == previous_node) ||
// 				(link.nodes[1] == current_node && link.nodes[0] == previous_node)) {
// 				WalkStep step = {i, current_walk->time};
// 				WalkStepVector_push(&walk.steps, step);
// 				break;
// 			}
// 		}

// 		current_walk = previous;
// 	}

// 	// reverse the walk
// 	for (size_t i = 0; i < walk.steps.size / 2; i++) {
// 		WalkStep tmp = walk.steps.array[i];
// 		walk.steps.array[i] = walk.steps.array[walk.steps.size - i - 1];
// 		walk.steps.array[walk.steps.size - i - 1] = tmp;
// 	}

// 	return walk;
// }

// Interval Walk_is_still_optimal_between(Walk* walk) {
// 	FullStreamGraph* fsg = (FullStreamGraph*)walk->stream->stream;
// 	StreamGraph sg = *fsg->underlying_stream_graph;

// 	Interval still_optimal = {walk->start_time, walk->start_time};
// 	// Loop through all the events of the stream
// 	size_t2Vector moments = KeyMomentsTable_all_moments(&sg.key_moments);
// 	for (size_t i = 0; i < moments.size; i++) {
// 		size_t moment = moments.array[i];
// 		size_t next_moment = i == moments.size - 1 ? SIZE_MAX : moments.array[i + 1];
// 		printf("Moment: %zu\n", moment);
// 		if ((moment < walk->start_time) && (next_moment > walk->start_time)) {
// 			still_optimal.start = moment;
// 			still_optimal.end = next_moment;
// 			break;
// 		}
// 	}
// 	if (still_optimal.end == still_optimal.start) {
// 		still_optimal.end++;
// 	}

// 	return still_optimal;
// }

// char* WalkOptimal_to_string(WalkOptimal* wo) {
// 	char* str = Walk_to_string(&wo->walk);
// 	char* str2 = Interval_to_string(&wo->optimality);
// 	char* result = malloc(strlen(str) + strlen(str2) + 2);
// 	strcpy(result, str2);
// 	strcat(result, "\n");
// 	strcat(result, str);

// 	free(str);
// 	free(str2);
// 	return result;
// }

// bool Walk_equals(Walk a, Walk b) {
// 	if (a.start != b.start || a.end != b.end || a.start_time != b.start_time || a.steps.size != b.steps.size) {
// 		return false;
// 	}
// 	for (size_t i = 0; i < a.steps.size; i++) {
// 		if (a.steps.array[i].link != b.steps.array[i].link || a.steps.array[i].time != b.steps.array[i].time) {
// 			return false;
// 		}
// 	}
// 	return true;
// }

// bool WalkOptimal_equals(WalkOptimal a, WalkOptimal b) {
// 	return Walk_equals(a.walk, b.walk) && Interval_equals(a.optimality, b.optimality);
// }

// WalkOptimalVector optimals_between_two_nodes(Stream* stream, NodeId from, NodeId to) {
// 	// start by the last event of the from node
// 	TimeId current_time = 0;
// 	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
// 	StreamGraph sg = *fsg->underlying_stream_graph;
// 	TemporalNode from_node = sg.nodes.nodes[from];
// 	TemporalNode to_node = sg.nodes.nodes[to];
// 	WalkOptimalVector optimals = WalkOptimalVector_with_capacity(from_node.nb_neighbours);
// 	for (size_t i = 0; i < from_node.nb_neighbours; i++) {
// 		LinkId link = from_node.neighbours[i];
// 		Link l = sg.links.links[link];
// 		for (size_t j = 0; j < l.presence.nb_intervals; j++) {
// 			Interval interval = l.presence.intervals[j];
// 			if (interval.end > current_time) {
// 				current_time = interval.end;
// 			}
// 		}
// 	}
// 	current_time--;
// 	printf("Last event of from node: %zu\n", current_time);

// 	// size_t previous_time = current_time;
// 	while (current_time != SIZE_MAX) {
// 		// Find the shortest walk from from to to at current_time
// 		Walk shortest = Stream_shortest_walk_from_to_at(stream, from, to, current_time);
// 		Interval shortest_optimality = Walk_is_still_optimal_between(&shortest);
// 		WalkOptimal wo = {shortest, shortest_optimality};
// 		WalkOptimalVector_push(&optimals, wo);
// 		current_time = shortest_optimality.start;
// 		// if (current_time == previous_time) {
// 		current_time--;
// 		//}
// 		// previous_time = current_time;
// 	}

// 	return optimals;
// }