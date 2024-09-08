#include "kcores.h"
#include "../stream/chunk_stream.h"
#include "../stream/chunk_stream_small.h"
#include "../stream/full_stream_graph.h"
#include "../stream/link_stream.h"
#include "../stream/snapshot_stream.h"
#include "../stream_functions.h"

int DegreeInInterval_compare(const void* a, const void* b) {
	DegreeInInterval da = *(DegreeInInterval*)a;
	DegreeInInterval db = *(DegreeInInterval*)b;

	if (da.time.start < db.time.start) {
		return -1;
	}
	if (da.time.start > db.time.start) {
		return 1;
	}
	return 0;
}

bool DegreeInInterval_equals(const DegreeInInterval* a, const DegreeInInterval* b) {
	return a->degree == b->degree && Interval_equals(&a->time, &b->time);
}

String DegreeInInterval_to_string(const DegreeInInterval* dii) {
	String str			= String_new();
	String interval_str = Interval_to_string(&dii->time);
	String_append_formatted(&str, "%s : %zu", interval_str.data, dii->degree);
	String_destroy(interval_str);
	return str;
}

DefineVector(DegreeInInterval);
DefineVectorDeriveRemove(DegreeInInterval, NO_FREE(DegreeInInterval));
DefineVectorDeriveOrdered(DegreeInInterval);
DefineVectorDeriveEquals(DegreeInInterval);
DefineVectorDeriveToString(DegreeInInterval);

void DegreeInIntervalVector_add(DegreeInIntervalVector* vec, Interval interval, size_t degree) {
	// Find if there is already a degree for this interval
	for (size_t i = 0; i < vec->size; i++) {
		if (Interval_equals(&vec->array[i].time, &interval)) {
			vec->array[i].degree += degree;
			return;
		}
	}

	// Find if there is an interval that contains this one
	for (size_t i = 0; i < vec->size; i++) {
		if (Interval_contains_interval(vec->array[i].time, interval)) {
			// split the interval into 3
			Interval one   = Interval_from(vec->array[i].time.start, interval.start);
			Interval two   = Interval_from(interval.start, interval.end);
			Interval three = Interval_from(interval.end, vec->array[i].time.end);

			size_t old_degree = vec->array[i].degree;

			// Remove the old interval
			DegreeInIntervalVector_remove_and_swap(vec, i);

			// Add the new intervals
			if (!Interval_is_empty(one)) {
				DegreeInIntervalVector_add(vec, one, old_degree);
			}
			if (!Interval_is_empty(two)) {
				DegreeInIntervalVector_add(vec, two, old_degree + 1);
			}
			if (!Interval_is_empty(three)) {
				DegreeInIntervalVector_add(vec, three, old_degree);
			}

			return;
		}
	}

	// Find if this one contains an interval
	for (size_t i = 0; i < vec->size; i++) {
		if (Interval_contains_interval(interval, vec->array[i].time)) {
			// split the interval into 3
			Interval one   = Interval_from(interval.start, vec->array[i].time.start);
			Interval two   = Interval_from(vec->array[i].time.start, vec->array[i].time.end);
			Interval three = Interval_from(vec->array[i].time.end, interval.end);

			size_t old_degree = vec->array[i].degree;

			// Remove the old interval
			DegreeInIntervalVector_remove_and_swap(vec, i);

			// Add the new intervals
			if (!Interval_is_empty(one)) {
				DegreeInIntervalVector_add(vec, one, old_degree);
			}
			if (!Interval_is_empty(two)) {
				DegreeInIntervalVector_add(vec, two, old_degree + 1);
			}
			if (!Interval_is_empty(three)) {
				DegreeInIntervalVector_add(vec, three, old_degree);
			}

			return;
		}
	}

	// Find if one interval overlaps the other
	for (size_t i = 0; i < vec->size; i++) {
		if (Interval_overlaps_interval(vec->array[i].time, interval)) {
			// split the interval into 3
			Interval inter		  = Interval_intersection(vec->array[i].time, interval);
			Interval old_excluded = Interval_minus(vec->array[i].time, inter);
			Interval new_excluded = Interval_minus(interval, inter);

			size_t old_degree = vec->array[i].degree;

			// Remove the old interval
			DegreeInIntervalVector_remove_and_swap(vec, i);

			// Add the new intervals
			if (!Interval_is_empty(inter)) {
				DegreeInIntervalVector_add(vec, inter, old_degree + 1);
			}
			if (!Interval_is_empty(old_excluded)) {
				DegreeInIntervalVector_add(vec, old_excluded, old_degree);
			}
			if (!Interval_is_empty(new_excluded)) {
				DegreeInIntervalVector_add(vec, new_excluded, old_degree);
			}

			return;
		}
	}

	// Otherwise, just add the interval
	DegreeInInterval new_degree = {degree, interval};
	DegreeInIntervalVector_push(vec, new_degree);
}

// TODO : move that into an analysis file maybe ?
// TODO: rename to instantaneus_degree
DegreeInIntervalVector Stream_evolution_of_node_degree(const Stream* stream, NodeId node_id) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	DegreeInIntervalVector degree_evol = DegreeInIntervalVector_new();
	TimesIterator times_node		   = fns.times_node_present(stream->stream_data, node_id);
	FOR_EACH_TIME(interval, times_node) {
		DegreeInIntervalVector_add(&degree_evol, interval, 0);
	}

	LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	FOR_EACH_LINK(link_id, neighbours) {
		TimesIterator times_neigh_present = fns.times_link_present(stream->stream_data, link_id);
		FOR_EACH_TIME(interval, times_neigh_present) {
			DegreeInIntervalVector_add(&degree_evol, interval, 1);
		}
	}

	DegreeInIntervalVector_sort(&degree_evol);
	return degree_evol;
}

typedef struct {
	// NodeId node_id;
	Interval time;
	NodeIdVector neighbours;
} KCoreData;

String KCoreData_to_string(const KCoreData* data) {
	String str = String_new();
	String_append_formatted(&str, "Time [%zu, %zu], Neighbours : { ", data->time.start, data->time.end);
	for (size_t i = 0; i < data->neighbours.size; i++) {
		String_append_formatted(&str, "%zu, ", data->neighbours.array[i]);
	}
	String_push_str(&str, "}\n");
	return str;
}

bool KCoreData_equals(const KCoreData* a, const KCoreData* b) {
	return Interval_equals(&a->time, &b->time) && NodeIdVector_equals(&a->neighbours, &b->neighbours);
}

int KCoreData_compare(const void* a, const void* b) {
	KCoreData da = *(KCoreData*)a;
	KCoreData db = *(KCoreData*)b;

	if (da.time.start < db.time.start) {
		return -1;
	}
	if (da.time.start > db.time.start) {
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

DefineVector(NodePresence);

void NodePresence_destroy(NodePresence node) {
	IntervalVector_destroy(node.presence);
}

DefineVectorDeriveRemove(NodePresence, NodePresence_destroy);

void KCore_destroy(KCore k_core) {
	NodePresenceVector_destroy(k_core.nodes);
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

	// printf(TEXT_BOLD "Computing the %zu-cores of the stream\n" TEXT_RESET, degree);

	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	// KCoreDataVector k_cores = KCoreDataVector_new();
	size_t nb_nodes				   = count_nodes(fns.nodes_set(stream->stream_data));
	KCoreDataVector* k_cores_datas = MALLOC(sizeof(KCoreDataVector) * nb_nodes);
	for (size_t i = 0; i < nb_nodes; i++) {
		k_cores_datas[i] = KCoreDataVector_new();
	}

	// Add all the stream data at the beginning (0-core)
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		LinksIterator links = fns.neighbours_of_node(stream->stream_data, node_id);
		FOR_EACH_LINK(link_id, links) {
			Link link			= fns.link_by_id(stream->stream_data, link_id);
			NodeId neighbour	= Link_get_other_node(&link, node_id);
			TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times) {
				KCores_add(&k_cores_datas[node_id], interval, neighbour);
			}
		}
	}

	// Then, remove iteratively the nodes with degree < k until the k-cores are stable
	// size_t iteration = 0;
	size_t* nb_intervals	= MALLOC(sizeof(size_t) * nb_nodes);
	IntervalsSet* intervals = MALLOC(sizeof(IntervalsSet) * nb_nodes);

	while (true) {

		// KCoreDataVector_merge(&k_cores);
		// printf(TEXT_BOLD "BEGIN OF ITERATION\n");
		// for (size_t i = 0; i < nb_nodes; i++) {
		// 	printf("k_cores_datas[%zu] = %s\n", i, KCoreDataVector_to_string(&k_cores_datas[i]).data);
		// }
		// printf(TEXT_RESET);
		// printf("k_cores = %s\n" TEXT_RESET, KCoreDataVector_to_string(&k_cores).data);

		// for (size_t i = 0; i < k_cores.size; i++) {
		// 	nb_intervals[k_cores.array[i].node_id]++;
		// }
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_intervals[i] = k_cores_datas[i].size;
		}

		for (size_t i = 0; i < nb_nodes; i++) {
			intervals[i] = IntervalsSet_alloc(nb_intervals[i]);
		}

		// int* nb_added = MALLOC(sizeof(int) * nb_nodes);
		// for (size_t i = 0; i < nb_nodes; i++) {
		// 	nb_added[i] = 0;
		// }

		// for (size_t i = 0; i < k_cores.size; i++) {
		// IntervalsSet_add_at(&intervals[k_cores.array[i].node_id], k_cores.array[i].time,
		// 					nb_added[k_cores.array[i].node_id]);
		// nb_added[k_cores.array[i].node_id]++;
		// }
		for (size_t i = 0; i < nb_nodes; i++) {
			for (size_t j = 0; j < k_cores_datas[i].size; j++) {
				IntervalsSet_add_at(&intervals[i], k_cores_datas[i].array[j].time, j);
				// nb_added[i]++;
			}
		}

		KCoreDataVector* newer_k_cores_datas = MALLOC(sizeof(KCoreDataVector) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			newer_k_cores_datas[i] = KCoreDataVector_new();
		}

		// for (size_t i = 0; i < k_cores.size; i++) {
		// 	if (k_cores.array[i].neighbours.size >= degree) {
		// 		for (size_t j = 0; j < k_cores.array[i].neighbours.size; j++) {
		// 			// See if a node that's a neighbour of the current node has been removed
		// 			IntervalsSet current = IntervalsSet_alloc(1);
		// 			IntervalsSet_add_at(&current, k_cores.array[i].time, 0);
		// 			IntervalsSet inter =
		// 				IntervalsSet_intersection(intervals[k_cores.array[i].neighbours.array[j]], current);

		// 			for (size_t k = 0; k < inter.nb_intervals; k++) {
		// 				KCores_add(&newer_k_cores, k_cores.array[i].node_id, inter.intervals[k],
		// 						   k_cores.array[i].neighbours.array[j]);
		// 			}

		// 			IntervalsSet_destroy(inter);
		// 			IntervalsSet_destroy(current);
		// 		}
		// 	}
		// }

		for (size_t i = 0; i < nb_nodes; i++) {
			for (size_t j = 0; j < k_cores_datas[i].size; j++) {
				if (k_cores_datas[i].array[j].neighbours.size >= degree) {
					for (size_t k = 0; k < k_cores_datas[i].array[j].neighbours.size; k++) {
						// See if a node that's a neighbour of the current node has been removed
						// IntervalsSet current = IntervalsSet_alloc(1);
						// IntervalsSet_add_at(&current, k_cores_datas[i].array[j].time, 0);
						// IntervalsSet inter = IntervalsSet_intersection(
						// 	intervals[k_cores_datas[i].array[j].neighbours.array[k]], current);

						IntervalsSet inter = IntervalsSet_intersection_with_single(
							intervals[k_cores_datas[i].array[j].neighbours.array[k]], k_cores_datas[i].array[j].time);

						for (size_t l = 0; l < inter.nb_intervals; l++) {
							KCores_add(&newer_k_cores_datas[i], inter.intervals[l],
									   k_cores_datas[i].array[j].neighbours.array[k]);
						}

						IntervalsSet_destroy(inter);
					}
				}
			}
		}

		// free(nb_added);
		for (size_t i = 0; i < nb_nodes; i++) {
			IntervalsSet_destroy(intervals[i]);
		}

		// KCoreDataVector_sort(&newer_k_cores);
		// KCoreDataVector_merge(&newer_k_cores);

		// // Stop if the k-cores are stable
		// if (KCoreDataVector_equals(&k_cores, &newer_k_cores)) {
		// 	KCoreDataVector_destroy(newer_k_cores);
		// 	break;
		// }

		// for (size_t i = 0; i < nb_nodes; i++) {
		// 	KCoreDataVector_merge(&newer_k_cores_datas[i]);
		// }

		// Stop if the k-cores are stable
		bool stable				  = true;
		size_t stopped_merging_at = 0;
		for (size_t i = 0; i < nb_nodes; i++) {
			KCoreDataVector_merge(&newer_k_cores_datas[i]);
			if (!KCoreDataVector_equals(&k_cores_datas[i], &newer_k_cores_datas[i])) {
				stable			   = false;
				stopped_merging_at = i;
				break;
			}
		}

		if (stable) {
			for (size_t i = 0; i < nb_nodes; i++) {
				KCoreDataVector_destroy(newer_k_cores_datas[i]);
			}
			free(newer_k_cores_datas);
			break;
		}
		else {
			for (size_t i = stopped_merging_at + 1; i < nb_nodes; i++) {
				KCoreDataVector_merge(&newer_k_cores_datas[i]);
			}
		}

		for (size_t i = 0; i < nb_nodes; i++) {
			KCoreDataVector_destroy(k_cores_datas[i]);
		}
		free(k_cores_datas);

		k_cores_datas = newer_k_cores_datas;

		// KCoreDataVector_destroy(k_cores);
		// k_cores = newer_k_cores;

		// printf(TEXT_BOLD "Iteration %zu\n" TEXT_RESET, iteration);
		// for (size_t i = 0; i < nb_nodes; i++) {
		// 	printf("Node %zu : %s\n", i, KCoreDataVector_to_string(&k_cores_datas[i]).data);
		// }
		// iteration++;
	}
	free(nb_intervals);
	free(intervals);

	// Then, extract the k-core

	// printf("RESULT\n");
	// for (size_t i = 0; i < nb_nodes; i++) {
	// 	printf("Node %zu : %s\n", i, KCoreDataVector_to_string(&k_cores_datas[i]).data);
	// }

	// Edge case : empty k-core
	// if (k_cores_datas.size == 0) {
	// 	// KCoreDataVector_destroy(k_cores);
	// 	KCore kcore = {
	// 		.nodes = NodePresenceVector_new(),
	// 	};
	// 	return kcore;
	// }

	// Edge case : empty k-core
	size_t total_size = 0;
	for (size_t i = 0; i < nb_nodes; i++) {
		total_size += k_cores_datas[i].size;
	}
	if (total_size == 0) {

		// free the mem
		for (size_t i = 0; i < nb_nodes; i++) {
			KCoreDataVector_destroy(k_cores_datas[i]);
		}
		free(k_cores_datas);

		return (KCore){.nodes = NodePresenceVector_new()};
	}

	NodeId cur_node = SIZE_MAX;
	KCore kcore		= {
			.nodes = NodePresenceVector_new(),
	};
	size_t cur_idx_to_push = 0;

	// for (size_t i = 0; i < k_cores.size; i++) {
	// 	if (k_cores.array[i].node_id != cur_node) {
	// 		NodePresence n = {
	// 			.node_id  = k_cores.array[i].node_id,
	// 			.presence = IntervalVector_new(),
	// 		};
	// 		IntervalVector_push(&n.presence, k_cores.array[i].time);
	// 		NodePresenceVector_push(&kcore.nodes, n);
	// 		cur_node = k_cores.array[i].node_id;
	// 		cur_idx_to_push++;
	// 	}
	// 	else {
	// 		IntervalVector_push(&kcore.nodes.array[cur_idx_to_push - 1].presence, k_cores.array[i].time);
	// 	}
	// }

	for (size_t i = 0; i < nb_nodes; i++) {
		for (size_t j = 0; j < k_cores_datas[i].size; j++) {
			if (i != cur_node) {
				NodePresence n = {
					.node_id  = i,
					.presence = IntervalVector_new(),
				};
				IntervalVector_push(&n.presence, k_cores_datas[i].array[j].time);
				NodePresenceVector_push(&kcore.nodes, n);
				cur_node = i;
				cur_idx_to_push++;
			}
			else {
				IntervalVector_push(&kcore.nodes.array[cur_idx_to_push - 1].presence, k_cores_datas[i].array[j].time);
			}
		}
	}

	// KCoreDataVector_destroy(k_cores);

	for (size_t i = 0; i < nb_nodes; i++) {
		KCoreDataVector_destroy(k_cores_datas[i]);
	}
	free(k_cores_datas);

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

bool KCore_equals(const KCore* a, const KCore* b) {
	if (a->nodes.size != b->nodes.size) {
		return false;
	}

	for (size_t i = 0; i < a->nodes.size; i++) {
		if (a->nodes.array[i].node_id != b->nodes.array[i].node_id) {
			return false;
		}

		if (!IntervalVector_equals(&a->nodes.array[i].presence, &b->nodes.array[i].presence)) {
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