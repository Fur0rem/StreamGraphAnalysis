#include "walks.h"
#include "iterators.h"
#include "stream_functions.h"

// For STREAM_FUNCS
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
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

// Breadth-first search for shortest path
typedef struct {
	NodeId node;
	size_t time;
	void* previous; // (QueueInfo*)
} QueueInfo;

bool QueueInfo_equals(QueueInfo a, QueueInfo b) {
	return a.node == b.node && a.time == b.time && a.previous == b.previous;
}

char* QueueInfo_to_string(QueueInfo* info) {
	char* str = malloc(100);
	sprintf(str, "QueueInfo(node:%zu, time:%zu)", info->node, info->time);
	return str;
}

char* PathStep_to_string(PathStep* step) {
	char* str = malloc(100);
	sprintf(str, "PathStep(link:%zu, time:%zu)", step->link, step->time);
	return str;
}

bool PathStep_equals(PathStep a, PathStep b) {
	return a.link == b.link && a.time == b.time;
}

DefVector(QueueInfo, NO_FREE(QueueInfo));

// Minimal number of hops between two nodes
Path Stream_shortest_path_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at) {
	// printf("Shortest path from %zu to %zu at %zu\n", from, to, at);

	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	// Initialize the queue with the starting node
	QueueInfoVector queue = QueueInfoVector_with_capacity(1);
	QueueInfo start = {from, current_time, NULL};
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

		QueueInfo current_path = current_info;
		/*printf("Candidate Path:");
		while (current_path.previous != NULL) {
			printf(" %zu (@ %zu)", current_path.node, current_path.time);
			current_path = *(QueueInfo*)current_path.previous;
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
					QueueInfo neighbor_info = {neighbor_id, current_time, previous};
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
					QueueInfo neighbor_info = {neighbor_id, interval.start, previous};
					QueueInfoVector_push(&queue, neighbor_info);
				}
			}
		}
	}
	/*printf("Shortest path found\n");
	char* str = QueueInfoVector_to_string(&queue);
	printf("Queue: %s\n", str);
	free(str);*/

	// Print the path
	/*QueueInfo* current = &current_info;
	while (current != NULL) {
		char* info_str = QueueInfo_to_string(current);
		printf(" %s\n", info_str);
		free(info_str);
		current = current->previous;
	}
	printf("End of path\n");*/

	// Build the path
	Path path = {
		.start = from,
		.end = to,
		.start_time = at,
		.stream = stream,
		.steps = PathStepVector_with_capacity(1),
	};
	QueueInfo* current_path = &current_info;
	while (current_path != NULL) {
		/*Link link = sg.links.links[current_path->node];
		PathStep step = {current_path->node, current_path->time};
		PathStepVector_push(&path.steps, step);
		current_path = current_path->previous;*/
		NodeId current_node = current_path->node;
		QueueInfo* previous = current_path->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		for (size_t i = 0; i < sg.links.nb_links; i++) {
			Link link = sg.links.links[i];
			if ((link.nodes[0] == current_node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_node && link.nodes[0] == previous_node)) {
				PathStep step = {i, current_path->time};
				PathStepVector_push(&path.steps, step);
				break;
			}
		}

		current_path = previous;
	}

	// reverse the path
	for (size_t i = 0; i < path.steps.size / 2; i++) {
		PathStep tmp = path.steps.array[i];
		path.steps.array[i] = path.steps.array[path.steps.size - i - 1];
		path.steps.array[path.steps.size - i - 1] = tmp;
	}

	return path;
}

typedef char char2;
DEFAULT_COMPARE(char2)
DEFAULT_TO_STRING(char2, "%c")
DefVector(char2, NO_FREE(char2));

#define APPEND_CONST(str) str, sizeof(str) - 1

char* Path_to_string(Path* path) {
	char2Vector str = char2Vector_with_capacity(100);
	char2Vector_append(&str, APPEND_CONST("Path from "));
	char buf[100];
	sprintf(buf, "%zu", path->start);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST(" to "));
	sprintf(buf, "%zu", path->end);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST(" at "));
	sprintf(buf, "%zu", path->start_time);
	char2Vector_append(&str, buf, strlen(buf));
	char2Vector_append(&str, APPEND_CONST("\n"));

	FullStreamGraph* fsg = (FullStreamGraph*)path->stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	for (size_t i = 0; i < path->steps.size; i++) {
		PathStep step = path->steps.array[i];
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

// Breadth-first search for shortest path
typedef struct {
	NodeId node;
	size_t time;
	size_t depth;
	void* previous; // (QueueInfoDepth*)
} QueueInfoDepth;

bool QueueInfoDepth_equals(QueueInfoDepth a, QueueInfoDepth b) {
	return a.node == b.node && a.time == b.time && a.previous == b.previous;
}

char* QueueInfoDepth_to_string(QueueInfoDepth* info) {
	char* str = malloc(100);
	sprintf(str, "QueueInfoDepth(node:%zu, time:%zu)", info->node, info->time);
	return str;
}

DefVector(QueueInfoDepth, NO_FREE(QueueInfoDepth));

Path Stream_fastest_shortest_path(Stream* stream, NodeId from, NodeId to, TimeId at) {
	// printf("Shortest path from %zu to %zu at %zu\n", from, to, at);

	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t current_time = at;

	FullStreamGraph* fsg = (FullStreamGraph*)stream->stream;
	StreamGraph sg = *fsg->underlying_stream_graph;

	// Initialize the queue with the starting node
	QueueInfoDepthVector queue = QueueInfoDepthVector_with_capacity(1);
	QueueInfoDepth start = {from, current_time, 0, NULL};
	QueueInfoDepthVector_push(&queue, start);

	NodeId current_candidate = from;
	QueueInfoDepth current_info;
	size_t best_time_yet = SIZE_MAX;
	QueueInfoDepth best_yet;
	bool found = false;
	size_t depth_found = SIZE_MAX;
	while ((!QueueInfoDepthVector_is_empty(queue)) && ((!found) || (current_info.depth <= depth_found))) {
		/*printf(TEXT_BOLD "Current candidate: %zu\n" TEXT_RESET, current_candidate);

		char* str = QueueInfoDepthVector_to_string(&queue);
		printf("Queue: %s\n", str);
		free(str);*/
		// Get the next node from the queue
		current_info = QueueInfoDepthVector_pop_front(&queue);

		QueueInfoDepth current_path = current_info;
		/*printf("Candidate Path:");
		while (current_path.previous != NULL) {
			printf(" %zu (@ %zu)", current_path.node, current_path.time);
			current_path = *(QueueInfoDepth*)current_path.previous;
		}
		printf(" Depth: %zu", current_info.depth);
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
					QueueInfoDepth* previous = malloc(sizeof(QueueInfoDepth));
					*previous = current_info;
					QueueInfoDepth neighbor_info = {neighbor_id, current_time, current_info.depth + 1, previous};
					// QueueInfoDepth neighbor_info = {neighbor_id, current_time};
					QueueInfoDepthVector_push(&queue, neighbor_info);
				}
				else if (interval.start > current_time) {
					// Link link = fns.nth_link(stream, neighbor);
					Link link = l;
					NodeId neighbor_id = link.nodes[0] == current_candidate ? link.nodes[1] : link.nodes[0];
					// QueueInfoDepth neighbor_info = {neighbor_id, interval.start, current_candidate};
					QueueInfoDepth* previous = malloc(sizeof(QueueInfoDepth));
					*previous = current_info;
					QueueInfoDepth neighbor_info = {neighbor_id, interval.start, current_info.depth + 1, previous};
					QueueInfoDepthVector_push(&queue, neighbor_info);
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
	/*printf(" fastest shortest path found\n");
	char* str = QueueInfoDepthVector_to_string(&queue);
	printf("Queue: %s\n", str);
	free(str);*/

	// Print the path
	/*QueueInfoDepth* current = &best_yet;
	while (current != NULL) {
		char* info_str = QueueInfoDepth_to_string(current);
		printf(" %s\n", info_str);
		free(info_str);
		current = current->previous;
	}
	printf("End of path\n");*/

	// Build the path
	Path path = {
		.start = from,
		.end = to,
		.start_time = at,
		.stream = stream,
		.steps = PathStepVector_with_capacity(1),
	};
	QueueInfoDepth* current_path = &best_yet;
	while (current_path != NULL) {
		/*Link link = sg.links.links[current_path->node];
		PathStep step = {current_path->node, current_path->time};
		PathStepVector_push(&path.steps, step);
		current_path = current_path->previous;*/
		NodeId current_node = current_path->node;
		QueueInfoDepth* previous = current_path->previous;
		NodeId previous_node = previous == NULL ? from : previous->node;
		// Find the link between the current node and the previous node
		for (size_t i = 0; i < sg.links.nb_links; i++) {
			Link link = sg.links.links[i];
			if ((link.nodes[0] == current_node && link.nodes[1] == previous_node) ||
				(link.nodes[1] == current_node && link.nodes[0] == previous_node)) {
				PathStep step = {i, current_path->time};
				PathStepVector_push(&path.steps, step);
				break;
			}
		}

		current_path = previous;
	}

	// reverse the path
	for (size_t i = 0; i < path.steps.size / 2; i++) {
		PathStep tmp = path.steps.array[i];
		path.steps.array[i] = path.steps.array[path.steps.size - i - 1];
		path.steps.array[path.steps.size - i - 1] = tmp;
	}

	return path;
}