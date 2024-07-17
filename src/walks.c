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

#include <stdbool.h>
#include <stddef.h>
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

DefVector(QueueInfo, NO_FREE(QueueInfo));

// Minimal number of hops between two nodes
void Stream_shortest_path_from_to_at(Stream* stream, NodeId from, NodeId to, TimeId at) {
	printf("Shortest path from %zu to %zu at %zu\n", from, to, at);

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

		printf(TEXT_BOLD "Current candidate: %zu\n" TEXT_RESET, current_candidate);

		char* str = QueueInfoVector_to_string(&queue);
		printf("Queue: %s\n", str);
		free(str);
		// Get the next node from the queue
		current_info = QueueInfoVector_pop_front(&queue);

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
	printf("Shortest path found\n");
	char* str = QueueInfoVector_to_string(&queue);
	printf("Queue: %s\n", str);
	free(str);

	// Print the path
	QueueInfo* current = &current_info;
	while (current != NULL) {
		char* info_str = QueueInfo_to_string(current);
		printf(" %s\n", info_str);
		free(info_str);
		current = current->previous;
	}
	printf("End of path\n");
}