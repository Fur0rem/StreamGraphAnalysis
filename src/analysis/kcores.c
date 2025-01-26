#define SGA_INTERNAL

#include "kcores.h"
#include "../stream_functions.h"
#include "../streams.h"
#include <stdint.h>

typedef struct {
	SGA_Interval time;
	SGA_NodeIdArrayList neighbours;
} KCoreData;

String KCoreData_to_string(const KCoreData* self) {
	String str = String_with_capacity(80 + self->neighbours.length * 10);
	String_append_formatted(&str, "Time [%zu, %zu], Neighbours : { ", self->time.start, self->time.end);
	for (size_t i = 0; i < self->neighbours.length; i++) {
		String_append_formatted(&str, "%zu, ", self->neighbours.array[i]);
	}
	String_push_str(&str, "}\n");
	return str;
}

bool KCoreData_equals(const KCoreData* left, const KCoreData* right) {
	return SGA_Interval_equals(&left->time, &right->time) && SGA_NodeIdArrayList_equals(&left->neighbours, &right->neighbours);
}

int KCoreData_compare(const KCoreData* left, const KCoreData* right) {
	if (left->time.start < right->time.start) {
		return -1;
	}
	if (left->time.start > right->time.start) {
		return 1;
	}
	return 0;
}

void KCoreData_destroy(KCoreData data) {
	SGA_NodeIdArrayList_destroy(data.neighbours);
}

DeclareArrayList(KCoreData);
DefineArrayList(KCoreData);
DefineArrayListDeriveRemove(KCoreData);
DefineArrayListDeriveOrdered(KCoreData);
DefineArrayListDeriveEquals(KCoreData);
DefineArrayListDeriveToString(KCoreData);
// TODO: that's neighbourhood -> KCoreDataArrayList

DefineArrayList(NodePresence);

void NodePresence_destroy(NodePresence node) {
	SGA_IntervalArrayList_destroy(node.presence);
}

DefineArrayListDeriveRemove(NodePresence);

void SGA_KCore_destroy(SGA_KCore self) {
	NodePresenceArrayList_destroy(self.nodes);
}

/**
 * @brief Simplifies the intervals of all the nodes in the k-core
 * It merges the intervals and sorts them to have a more compact representation
 * @param k_core[in, out] The k-core to simplify
 */
void KCore_simplify(SGA_KCore* k_core) {
	for (size_t i = 0; i < k_core->nodes.length; i++) {
		SGA_IntervalsSet intervals = SGA_IntervalsSet_from_interval_arraylist(k_core->nodes.array[i].presence);
		SGA_IntervalsSet_sort(&intervals);
		SGA_IntervalsSet_merge(&intervals);
		k_core->nodes.array[i].presence = SGA_IntervalArrayList_from_intervals_set(intervals);
	}
}

void KCores_add(KCoreDataArrayList* k_cores, SGA_Interval time, SGA_NodeId neigh) {
	// Find if the node is already in the arraylist and with the same interval
	for (size_t i = 0; i < k_cores->length; i++) {
		if (SGA_Interval_equals(&k_cores->array[i].time, &time)) {
			SGA_NodeIdArrayList_push(&k_cores->array[i].neighbours, neigh);
			return;
		}
	}

	// Find if there exists an interval that contains this one
	for (size_t i = 0; i < k_cores->length; i++) {
		if (SGA_Interval_contains_interval(k_cores->array[i].time, time)) {
			SGA_Interval one   = SGA_Interval_from(k_cores->array[i].time.start, time.start);
			SGA_Interval two   = SGA_Interval_from(time.start, time.end);
			SGA_Interval three = SGA_Interval_from(time.end, k_cores->array[i].time.end);

			KCoreData x		      = KCoreDataArrayList_pop_nth_swap(k_cores, i);
			SGA_NodeIdArrayList old_neigh = x.neighbours;

			// Add the new intervals
			if (!SGA_Interval_is_empty(one)) {
				for (size_t i = 0; i < old_neigh.length; i++) {
					KCores_add(k_cores, one, old_neigh.array[i]);
				}
			}
			if (!SGA_Interval_is_empty(two)) {
				for (size_t i = 0; i < old_neigh.length; i++) {
					KCores_add(k_cores, two, old_neigh.array[i]);
				}
				KCores_add(k_cores, two, neigh);
			}
			if (!SGA_Interval_is_empty(three)) {
				for (size_t i = 0; i < old_neigh.length; i++) {
					KCores_add(k_cores, three, old_neigh.array[i]);
				}
			}

			// Remove the old interval
			KCoreData_destroy(x);
			return;
		}
	}

	// Find if there exists an interval that is contained by this one
	for (size_t i = 0; i < k_cores->length; i++) {
		if (SGA_Interval_contains_interval(time, k_cores->array[i].time)) {
			// split the interval into 3
			SGA_Interval one   = SGA_Interval_from(time.start, k_cores->array[i].time.start);
			SGA_Interval two   = SGA_Interval_from(k_cores->array[i].time.start, k_cores->array[i].time.end);
			SGA_Interval three = SGA_Interval_from(k_cores->array[i].time.end, time.end);

			KCoreData x		      = KCoreDataArrayList_pop_nth_swap(k_cores, i);
			SGA_NodeIdArrayList old_neigh = x.neighbours;

			// Add the new intervals
			if (!SGA_Interval_is_empty(one)) {
				KCores_add(k_cores, one, neigh);
			}
			if (!SGA_Interval_is_empty(two)) {
				for (size_t i = 0; i < old_neigh.length; i++) {
					KCores_add(k_cores, two, old_neigh.array[i]);
				}
				KCores_add(k_cores, two, neigh);
			}
			if (!SGA_Interval_is_empty(three)) {
				KCores_add(k_cores, three, neigh);
			}

			// Remove the old interval
			KCoreData_destroy(x);

			return;
		}
	}

	// Find if one interval overlaps the other
	for (size_t i = 0; i < k_cores->length; i++) {
		if (SGA_Interval_overlaps_interval(k_cores->array[i].time, time)) {
			// split the interval into 3
			SGA_Interval inter	  = SGA_Interval_intersection(k_cores->array[i].time, time);
			SGA_Interval old_excluded = SGA_Interval_minus(k_cores->array[i].time, inter);
			SGA_Interval new_excluded = SGA_Interval_minus(time, inter);

			KCoreData x		      = KCoreDataArrayList_pop_nth_swap(k_cores, i);
			SGA_NodeIdArrayList old_neigh = x.neighbours;

			// Add the new intervals
			if (!SGA_Interval_is_empty(inter)) {
				for (size_t i = 0; i < old_neigh.length; i++) {
					KCores_add(k_cores, inter, old_neigh.array[i]);
				}
				KCores_add(k_cores, inter, neigh);
			}

			if (!SGA_Interval_is_empty(new_excluded)) {
				KCores_add(k_cores, new_excluded, neigh);
			}

			if (!SGA_Interval_is_empty(old_excluded)) {
				for (size_t i = 0; i < old_neigh.length; i++) {
					KCores_add(k_cores, old_excluded, old_neigh.array[i]);
				}
			}

			// Remove the old interval
			KCoreData_destroy(x);

			// KCoreDataArrayList_remove_and_swap(k_cores, i);

			return;
		}
	}

	// Otherwise, add the node
	KCoreData new_data = {time, SGA_NodeIdArrayList_new()};
	SGA_NodeIdArrayList_push(&new_data.neighbours, neigh);
	KCoreDataArrayList_push(k_cores, new_data);
}

void KCoreDataArrayList_merge(KCoreDataArrayList* vec) {

	if (vec->length == 0) {
		return;
	}

	KCoreDataArrayList_sort_unstable(vec);

	// for each node
	for (size_t i = 0; i < vec->length; i++) {
		// see if there are duplicate neighbours (for example {0, 3, 3 })
		SGA_NodeIdArrayList neighbours = vec->array[i].neighbours;
		SGA_NodeIdArrayList new_neigh  = SGA_NodeIdArrayList_new();
		for (size_t j = 0; j < neighbours.length; j++) {
			if (!SGA_NodeIdArrayList_contains(new_neigh, neighbours.array[j])) {
				SGA_NodeIdArrayList_push(&new_neigh, neighbours.array[j]);
			}
		}
		SGA_NodeIdArrayList_destroy(neighbours);
		vec->array[i].neighbours = new_neigh;
	}

	// see for each element in the arraylist that if the next one is the same except for the interval, merge them
	for (size_t i = 0; i < vec->length - 1; i++) {
		if (SGA_NodeIdArrayList_equals(&vec->array[i].neighbours, &vec->array[i + 1].neighbours)) {
			SGA_Interval i1 = vec->array[i].time;
			SGA_Interval i2 = vec->array[i + 1].time;
			if (i1.end == i2.start) {
				SGA_Interval new_interval = SGA_Interval_from(i1.start, i2.end);
				vec->array[i].time	  = new_interval;
				KCoreDataArrayList_remove_and_swap(vec, i + 1);
				i--;
			}
		}
	}

	KCoreDataArrayList_sort_unstable(vec);
}

/// Doesn't work with 0 but who cares about 0-core cause it's the same as the initial graph
SGA_KCore SGA_Stream_k_core(const SGA_Stream* stream, size_t degree) {

	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	size_t nb_nodes		= 0;
	size_t biggest_node_id	= 0;
	SGA_FOR_EACH_NODE(node_id, nodes) {
		nb_nodes++;
		if (node_id > biggest_node_id) {
			biggest_node_id = node_id;
		}
	}
	// TODO: switch (biggest_node_id++, int i=0; i<biggest_node_id_;i++) to (int i=0; i<=biggest_node_id; i++)
	// cause its more coherent
	biggest_node_id++; // OPTIMISE: memory waste, maybe do a node_id to index mapping

	KCoreDataArrayList* neighbourhood      = MALLOC(sizeof(KCoreDataArrayList) * biggest_node_id);
	KCoreDataArrayList* next_neighbourhood = MALLOC(sizeof(KCoreDataArrayList) * biggest_node_id);
	for (size_t node = 0; node < biggest_node_id; node++) {
		neighbourhood[node]	 = KCoreDataArrayList_new();
		next_neighbourhood[node] = KCoreDataArrayList_new();
	}

	// Add all the stream data at the beginning (0-core)
	nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		SGA_LinksIterator links = fns.neighbours_of_node(stream->stream_data, node_id);
		SGA_FOR_EACH_LINK(link_id, links) {
			SGA_Link link		= fns.link_by_id(stream->stream_data, link_id);
			SGA_NodeId neighbour	= SGA_Link_get_other_node(&link, node_id);
			SGA_TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
			SGA_FOR_EACH_TIME(interval, times) {
				KCores_add(&neighbourhood[node_id], interval, neighbour);
			}
		}
	}

	// Then, remove iteratively the nodes with degree < k until the k-cores are stable

	SGA_IntervalsSet* nodes_presence = MALLOC(sizeof(SGA_IntervalsSet) * biggest_node_id);

	while (true) {

		for (size_t i = 0; i < biggest_node_id; i++) {
			nodes_presence[i] = SGA_IntervalsSet_alloc(neighbourhood[i].length);
		}

		for (size_t i = 0; i < biggest_node_id; i++) {
			for (size_t j = 0; j < neighbourhood[i].length; j++) {
				SGA_IntervalsSet_add_at(&nodes_presence[i], neighbourhood[i].array[j].time, j);
			}
		}

		for (size_t node = 0; node < biggest_node_id; node++) {
			for (size_t neighbour = 0; neighbour < neighbourhood[node].length; neighbour++) {
				if (neighbourhood[node].array[neighbour].neighbours.length >= degree) {
					for (size_t x = 0; x < neighbourhood[node].array[neighbour].neighbours.length; x++) {
						// See if a node that's a neighbour of the current node has been removed
						SGA_IntervalsSet inter = SGA_IntervalsSet_intersection_with_single(
						    nodes_presence[neighbourhood[node].array[neighbour].neighbours.array[x]],
						    neighbourhood[node].array[neighbour].time);

						for (size_t l = 0; l < inter.nb_intervals; l++) {
							KCores_add(&next_neighbourhood[node],
								   inter.intervals[l],
								   neighbourhood[node].array[neighbour].neighbours.array[x]);
						}

						SGA_IntervalsSet_destroy(inter);
					}
				}
			}
		}

		for (size_t i = 0; i < biggest_node_id; i++) {
			SGA_IntervalsSet_destroy(nodes_presence[i]);
		}

		// Stop if the k-cores are stable
		bool stable		  = true;
		size_t stopped_merging_at = 0;
		for (size_t node = 0; node < biggest_node_id; node++) {
			KCoreDataArrayList_merge(&next_neighbourhood[node]);
			if (!KCoreDataArrayList_equals(&neighbourhood[node], &next_neighbourhood[node])) {
				stable		   = false;
				stopped_merging_at = node;
				break;
			}
		}

		if (stable) {
			// Since they are equal, we can just use the past iteration's neighbourhood
			for (size_t node_id = 0; node_id < biggest_node_id; node_id++) {
				KCoreDataArrayList_destroy(next_neighbourhood[node_id]);
			}
			free(next_neighbourhood);
			break;
		}

		// Prepare for the next iteration

		// Finish the merging
		for (size_t i = stopped_merging_at + 1; i < biggest_node_id; i++) {
			KCoreDataArrayList_merge(&next_neighbourhood[i]);
		}

		// Swap memory buffers
		for (size_t i = 0; i < biggest_node_id; i++) {
			KCoreDataArrayList_clear(&neighbourhood[i]);
		}
		KCoreDataArrayList* tmp = neighbourhood;
		neighbourhood		= next_neighbourhood;
		next_neighbourhood	= tmp;
	}

	free(nodes_presence);

	// Then, extract the k-core

	// Edge case : empty k-core
	size_t total_size = 0;
	for (size_t i = 0; i < biggest_node_id; i++) {
		total_size += neighbourhood[i].length;
	}
	if (total_size == 0) {
		for (size_t i = 0; i < biggest_node_id; i++) {
			KCoreDataArrayList_destroy(neighbourhood[i]);
		}
		free(neighbourhood);

		return (SGA_KCore){.nodes = NodePresenceArrayList_new()};
	}

	SGA_NodeId cur_node    = SIZE_MAX;
	SGA_KCore kcore	       = {.nodes = NodePresenceArrayList_new()};
	size_t cur_idx_to_push = 0;

	for (size_t i = 0; i < biggest_node_id; i++) {
		for (size_t j = 0; j < neighbourhood[i].length; j++) {
			if (i != cur_node) {
				NodePresence new_node = {
				    .node_id  = i,
				    .presence = SGA_IntervalArrayList_new(),
				};
				SGA_IntervalArrayList_push(&new_node.presence, neighbourhood[i].array[j].time);
				NodePresenceArrayList_push(&kcore.nodes, new_node);
				cur_node = i;
				cur_idx_to_push++;
			}
			else {
				SGA_IntervalArrayList_push(&kcore.nodes.array[cur_idx_to_push - 1].presence,
							   neighbourhood[i].array[j].time);
			}
		}
	}

	// KCoreDataArrayList_destroy(k_cores);

	for (size_t i = 0; i < biggest_node_id; i++) {
		KCoreDataArrayList_destroy(neighbourhood[i]);
	}
	free(neighbourhood);

	KCore_simplify(&kcore);

	return kcore;
}

String SGA_KCore_to_string(const SGA_KCore* k_core) {
	String str = String_from_duplicate("KCore : { \n");
	for (size_t i = 0; i < k_core->nodes.length; i++) {
		String intervals_str = SGA_IntervalArrayList_to_string(&k_core->nodes.array[i].presence);
		String_append_formatted(&str, "\t%zu : ", k_core->nodes.array[i].node_id);
		String_concat_consume(&str, intervals_str);
		if (i != k_core->nodes.length - 1) {
			String_push_str(&str, "\n");
		}
	}
	String_push_str(&str, "\n}");
	return str;
}

bool SGA_KCore_equals(const SGA_KCore* left, const SGA_KCore* right) {
	if (left->nodes.length != right->nodes.length) {
		return false;
	}

	for (size_t i = 0; i < left->nodes.length; i++) {
		if (left->nodes.array[i].node_id != right->nodes.array[i].node_id) {
			return false;
		}

		if (!SGA_IntervalArrayList_equals(&left->nodes.array[i].presence, &right->nodes.array[i].presence)) {
			return false;
		}
	}

	return true;
}
