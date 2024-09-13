#include "kcores.h"
#include "../stream_functions.h"
#include "../stream_wrappers.h"
#include <stdint.h>

typedef struct {
	Interval time;
	NodeIdVector neighbours;
} KCoreData;

String KCoreData_to_string(const KCoreData* self) {
	String str = String_with_capacity(80 + self->neighbours.size * 10);
	String_append_formatted(&str, "Time [%zu, %zu], Neighbours : { ", self->time.start, self->time.end);
	for (size_t i = 0; i < self->neighbours.size; i++) {
		String_append_formatted(&str, "%zu, ", self->neighbours.array[i]);
	}
	String_push_str(&str, "}\n");
	return str;
}

bool KCoreData_equals(const KCoreData* left, const KCoreData* right) {
	return Interval_equals(&left->time, &right->time) &&
		   NodeIdVector_equals(&left->neighbours, &right->neighbours);
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
	NodeIdVector_destroy(data.neighbours);
}

DeclareVector(KCoreData);
DefineVector(KCoreData);
DefineVectorDeriveRemove(KCoreData, KCoreData_destroy);
DefineVectorDeriveOrdered(KCoreData);
DefineVectorDeriveEquals(KCoreData);
DefineVectorDeriveToString(KCoreData);
// TODO: that's neighbourhood -> KCoreDataVector

DefineVector(NodePresence);

void NodePresence_destroy(NodePresence node) {
	IntervalVector_destroy(node.presence);
}

DefineVectorDeriveRemove(NodePresence, NodePresence_destroy);

void KCore_destroy(KCore self) {
	NodePresenceVector_destroy(self.nodes);
}

void KCores_add(KCoreDataVector* k_cores, Interval time, NodeId neigh) {
	// Find if the node is already in the vector and with the same interval
	for (size_t i = 0; i < k_cores->size; i++) {
		if (Interval_equals(&k_cores->array[i].time, &time)) {
			NodeIdVector_push(&k_cores->array[i].neighbours, neigh);
			return;
		}
	}

	// Find if there exists an interval that contains this one
	for (size_t i = 0; i < k_cores->size; i++) {
		if (Interval_contains_interval(k_cores->array[i].time, time)) {
			Interval one   = Interval_from(k_cores->array[i].time.start, time.start);
			Interval two   = Interval_from(time.start, time.end);
			Interval three = Interval_from(time.end, k_cores->array[i].time.end);

			KCoreData x			   = KCoreDataVector_pop_nth_swap(k_cores, i);
			NodeIdVector old_neigh = x.neighbours;

			// Add the new intervals
			if (!Interval_is_empty(one)) {
				for (size_t i = 0; i < old_neigh.size; i++) {
					KCores_add(k_cores, one, old_neigh.array[i]);
				}
			}
			if (!Interval_is_empty(two)) {
				for (size_t i = 0; i < old_neigh.size; i++) {
					KCores_add(k_cores, two, old_neigh.array[i]);
				}
				KCores_add(k_cores, two, neigh);
			}
			if (!Interval_is_empty(three)) {
				for (size_t i = 0; i < old_neigh.size; i++) {
					KCores_add(k_cores, three, old_neigh.array[i]);
				}
			}

			// Remove the old interval
			KCoreData_destroy(x);
			return;
		}
	}

	// Find if there exists an interval that is contained by this one
	for (size_t i = 0; i < k_cores->size; i++) {
		if (Interval_contains_interval(time, k_cores->array[i].time)) {
			// split the interval into 3
			Interval one   = Interval_from(time.start, k_cores->array[i].time.start);
			Interval two   = Interval_from(k_cores->array[i].time.start, k_cores->array[i].time.end);
			Interval three = Interval_from(k_cores->array[i].time.end, time.end);

			KCoreData x			   = KCoreDataVector_pop_nth_swap(k_cores, i);
			NodeIdVector old_neigh = x.neighbours;

			// Add the new intervals
			if (!Interval_is_empty(one)) {
				KCores_add(k_cores, one, neigh);
			}
			if (!Interval_is_empty(two)) {
				for (size_t i = 0; i < old_neigh.size; i++) {
					KCores_add(k_cores, two, old_neigh.array[i]);
				}
				KCores_add(k_cores, two, neigh);
			}
			if (!Interval_is_empty(three)) {
				KCores_add(k_cores, three, neigh);
			}

			// Remove the old interval
			KCoreData_destroy(x);

			return;
		}
	}

	// Find if one interval overlaps the other
	for (size_t i = 0; i < k_cores->size; i++) {
		if (Interval_overlaps_interval(k_cores->array[i].time, time)) {
			// split the interval into 3
			Interval inter		  = Interval_intersection(k_cores->array[i].time, time);
			Interval old_excluded = Interval_minus(k_cores->array[i].time, inter);
			Interval new_excluded = Interval_minus(time, inter);

			KCoreData x			   = KCoreDataVector_pop_nth_swap(k_cores, i);
			NodeIdVector old_neigh = x.neighbours;

			// Add the new intervals
			if (!Interval_is_empty(inter)) {
				for (size_t i = 0; i < old_neigh.size; i++) {
					KCores_add(k_cores, inter, old_neigh.array[i]);
				}
				KCores_add(k_cores, inter, neigh);
			}

			if (!Interval_is_empty(new_excluded)) {
				KCores_add(k_cores, new_excluded, neigh);
			}

			if (!Interval_is_empty(old_excluded)) {
				for (size_t i = 0; i < old_neigh.size; i++) {
					KCores_add(k_cores, old_excluded, old_neigh.array[i]);
				}
			}

			// Remove the old interval
			KCoreData_destroy(x);

			// KCoreDataVector_remove_and_swap(k_cores, i);

			return;
		}
	}

	// Otherwise, add the node
	KCoreData new_data = {time, NodeIdVector_new()};
	NodeIdVector_push(&new_data.neighbours, neigh);
	KCoreDataVector_push(k_cores, new_data);
}

void KCoreDataVector_merge(KCoreDataVector* vec) {

	if (vec->size == 0) {
		return;
	}

	KCoreDataVector_sort(vec);

	// for each node
	for (size_t i = 0; i < vec->size; i++) {
		// see if there are duplicate neighbours (for example {0, 3, 3 })
		NodeIdVector neighbours = vec->array[i].neighbours;
		NodeIdVector new_neigh	= NodeIdVector_new();
		for (size_t j = 0; j < neighbours.size; j++) {
			if (!NodeIdVector_contains(new_neigh, neighbours.array[j])) {
				NodeIdVector_push(&new_neigh, neighbours.array[j]);
			}
		}
		NodeIdVector_destroy(neighbours);
		vec->array[i].neighbours = new_neigh;
	}

	// see for each element in the vector that if the next one is the same except for the interval, merge them
	for (size_t i = 0; i < vec->size - 1; i++) {
		if (NodeIdVector_equals(&vec->array[i].neighbours, &vec->array[i + 1].neighbours)) {
			Interval i1 = vec->array[i].time;
			Interval i2 = vec->array[i + 1].time;
			if (i1.end == i2.start) {
				Interval new_interval = Interval_from(i1.start, i2.end);
				vec->array[i].time	  = new_interval;
				KCoreDataVector_remove_and_swap(vec, i + 1);
				i--;
			}
		}
	}

	KCoreDataVector_sort(vec);
}

/// Doesn't work with 0 but who cares about 0-core cause it's the same as the initial graph
KCore Stream_k_cores(const Stream* stream, size_t degree) {

	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	NodesIterator nodes	   = fns.nodes_set(stream->stream_data);
	size_t nb_nodes		   = 0;
	size_t biggest_node_id = 0;
	FOR_EACH_NODE(node_id, nodes) {
		nb_nodes++;
		if (node_id > biggest_node_id) {
			biggest_node_id = node_id;
		}
	}
	// TODO: switch (biggest_node_id++, int i=0; i<biggest_node_id_;i++) to (int i=0; i<=biggest_node_id; i++)
	// cause its more coherent
	biggest_node_id++; // OPTIMISE: memory waste, maybe do a node_id to index mapping

	KCoreDataVector* neighbourhood		= MALLOC(sizeof(KCoreDataVector) * biggest_node_id);
	KCoreDataVector* next_neighbourhood = MALLOC(sizeof(KCoreDataVector) * biggest_node_id);
	for (size_t node = 0; node < biggest_node_id; node++) {
		neighbourhood[node]		 = KCoreDataVector_new();
		next_neighbourhood[node] = KCoreDataVector_new();
	}

	// Add all the stream data at the beginning (0-core)
	nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		LinksIterator links = fns.neighbours_of_node(stream->stream_data, node_id);
		FOR_EACH_LINK(link_id, links) {
			Link link			= fns.link_by_id(stream->stream_data, link_id);
			NodeId neighbour	= Link_get_other_node(&link, node_id);
			TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times) {
				KCores_add(&neighbourhood[node_id], interval, neighbour);
			}
		}
	}

	// Then, remove iteratively the nodes with degree < k until the k-cores are stable

	IntervalsSet* nodes_presence = MALLOC(sizeof(IntervalsSet) * biggest_node_id);

	while (true) {

		for (size_t i = 0; i < biggest_node_id; i++) {
			nodes_presence[i] = IntervalsSet_alloc(neighbourhood[i].size);
		}

		for (size_t i = 0; i < biggest_node_id; i++) {
			for (size_t j = 0; j < neighbourhood[i].size; j++) {
				IntervalsSet_add_at(&nodes_presence[i], neighbourhood[i].array[j].time, j);
			}
		}

		for (size_t node = 0; node < biggest_node_id; node++) {
			for (size_t neighbour = 0; neighbour < neighbourhood[node].size; neighbour++) {
				if (neighbourhood[node].array[neighbour].neighbours.size >= degree) {
					for (size_t x = 0; x < neighbourhood[node].array[neighbour].neighbours.size; x++) {
						// See if a node that's a neighbour of the current node has been removed
						IntervalsSet inter = IntervalsSet_intersection_with_single(
							nodes_presence[neighbourhood[node].array[neighbour].neighbours.array[x]],
							neighbourhood[node].array[neighbour].time);

						for (size_t l = 0; l < inter.nb_intervals; l++) {
							KCores_add(&next_neighbourhood[node],
									   inter.intervals[l],
									   neighbourhood[node].array[neighbour].neighbours.array[x]);
						}

						IntervalsSet_destroy(inter);
					}
				}
			}
		}

		for (size_t i = 0; i < biggest_node_id; i++) {
			IntervalsSet_destroy(nodes_presence[i]);
		}

		// Stop if the k-cores are stable
		bool stable				  = true;
		size_t stopped_merging_at = 0;
		for (size_t node = 0; node < biggest_node_id; node++) {
			KCoreDataVector_merge(&next_neighbourhood[node]);
			if (!KCoreDataVector_equals(&neighbourhood[node], &next_neighbourhood[node])) {
				stable			   = false;
				stopped_merging_at = node;
				break;
			}
		}

		if (stable) {
			// Since they are equal, we can just use the past iteration's neighbourhood
			for (size_t node_id = 0; node_id < biggest_node_id; node_id++) {
				KCoreDataVector_destroy(next_neighbourhood[node_id]);
			}
			free(next_neighbourhood);
			break;
		}

		// Prepare for the next iteration

		// Finish the merging
		for (size_t i = stopped_merging_at + 1; i < biggest_node_id; i++) {
			KCoreDataVector_merge(&next_neighbourhood[i]);
		}

		// Swap memory buffers
		for (size_t i = 0; i < biggest_node_id; i++) {
			KCoreDataVector_clear(&neighbourhood[i]);
		}
		KCoreDataVector* tmp = neighbourhood;
		neighbourhood		 = next_neighbourhood;
		next_neighbourhood	 = tmp;
	}

	free(nodes_presence);

	// Then, extract the k-core

	// Edge case : empty k-core
	size_t total_size = 0;
	for (size_t i = 0; i < biggest_node_id; i++) {
		total_size += neighbourhood[i].size;
	}
	if (total_size == 0) {
		for (size_t i = 0; i < biggest_node_id; i++) {
			KCoreDataVector_destroy(neighbourhood[i]);
		}
		free(neighbourhood);

		return (KCore){.nodes = NodePresenceVector_new()};
	}

	NodeId cur_node		   = SIZE_MAX;
	KCore kcore			   = {.nodes = NodePresenceVector_new()};
	size_t cur_idx_to_push = 0;

	for (size_t i = 0; i < biggest_node_id; i++) {
		for (size_t j = 0; j < neighbourhood[i].size; j++) {
			if (i != cur_node) {
				NodePresence new_node = {
					.node_id  = i,
					.presence = IntervalVector_new(),
				};
				IntervalVector_push(&new_node.presence, neighbourhood[i].array[j].time);
				NodePresenceVector_push(&kcore.nodes, new_node);
				cur_node = i;
				cur_idx_to_push++;
			}
			else {
				IntervalVector_push(&kcore.nodes.array[cur_idx_to_push - 1].presence,
									neighbourhood[i].array[j].time);
			}
		}
	}

	// KCoreDataVector_destroy(k_cores);

	for (size_t i = 0; i < biggest_node_id; i++) {
		KCoreDataVector_destroy(neighbourhood[i]);
	}
	free(neighbourhood);

	KCore_clean_up(&kcore);

	return kcore;
}

String KCore_to_string(const KCore* k_core) {
	String str = String_from_duplicate("KCore : { \n");
	for (size_t i = 0; i < k_core->nodes.size; i++) {
		String intervals_str = IntervalVector_to_string(&k_core->nodes.array[i].presence);
		String_append_formatted(&str, "\t%zu : ", k_core->nodes.array[i].node_id);
		String_concat_consume(&str, &intervals_str);
		if (i != k_core->nodes.size - 1) {
			String_push_str(&str, "\n");
		}
	}
	String_push_str(&str, "\n}");
	return str;
}

bool KCore_equals(const KCore* left, const KCore* right) {
	if (left->nodes.size != right->nodes.size) {
		return false;
	}

	for (size_t i = 0; i < left->nodes.size; i++) {
		if (left->nodes.array[i].node_id != right->nodes.array[i].node_id) {
			return false;
		}

		if (!IntervalVector_equals(&left->nodes.array[i].presence, &right->nodes.array[i].presence)) {
			return false;
		}
	}

	return true;
}

// TODO : quel nom de merde mais j'ai pas d'idee ptdr
void KCore_clean_up(KCore* k_core) {
	for (size_t i = 0; i < k_core->nodes.size; i++) {
		IntervalsSet intervals = IntervalsSet_from_interval_vector(k_core->nodes.array[i].presence);
		IntervalsSet_sort(&intervals);
		IntervalsSet_merge(&intervals);
		k_core->nodes.array[i].presence = IntervalVector_from_intervals_set(intervals);
	}
}