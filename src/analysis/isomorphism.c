#define SGA_INTERNAL

#include "isomorphism.h"
#include "../analysis/metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../streams.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Possible switches to more efficient algorithms
// Laszlo Babai's algorithm, VF2++, Nauty
// https://blog.scientific-python.org/networkx/vf2pp/graph-iso-vf2pp/
// https://www3.cs.stonybrook.edu/~algorith/implement/nauty/implement.shtml
// https://mivia.unisa.it/wp-content/uploads/2013/05/foggia01.pdf
// https://algorist.com/problems/Graph_Isomorphism.html

bool check_interval_lists_match(SGA_IntervalArrayList* l1, SGA_IntervalArrayList* l2, size_t universal_offset_amount) {
	SGA_Offset offset = SGA_IntervalArrayList_offset_of(l1, l2);

	// The two lists don't match
	if (SGA_Offset_is_not_matching(offset)) {
		return false;
	}

	if (!SGA_Offset_is_empty(offset)) {
		size_t offset_amount = SGA_Offset_unwrap(offset);
		// Not offset by the same amount as the streamgraphs
		if (offset_amount != universal_offset_amount) {
			return false;
		}
	}

	return true;
}

bool check_all_possible_mappings(const SGA_Stream* s1, const SGA_Stream* s2, size_t n, SGA_IntervalArrayList** graph1,
				 SGA_IntervalArrayList** graph2, int* mapping, bool* visited, int vertex, size_t universal_offset_amount) {
	StreamFunctions fns1 = STREAM_FUNCS(fns1, s1);
	StreamFunctions fns2 = STREAM_FUNCS(fns2, s2);

	if (vertex == n) {
		// Check if the mapping preserves adjacency
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {

				// Check if the intervals of the links match
				if (!check_interval_lists_match(&graph1[i][j], &graph2[mapping[i]][mapping[j]], universal_offset_amount)) {
					return false;
				}

				// Check if the times of the nodes match
				SGA_TimesIterator times_iter_original	  = fns1.times_node_present(s1->stream_data, i);
				SGA_TimesIterator times_iter_mapped	  = fns2.times_node_present(s2->stream_data, mapping[i]);
				SGA_IntervalArrayList times_original_node = SGA_collect_times(times_iter_original);
				SGA_IntervalArrayList times_mapped_node	  = SGA_collect_times(times_iter_mapped);

				if (!check_interval_lists_match(&times_original_node, &times_mapped_node, universal_offset_amount)) {
					SGA_IntervalArrayList_destroy(times_original_node);
					SGA_IntervalArrayList_destroy(times_mapped_node);
					return false;
				}

				SGA_IntervalArrayList_destroy(times_original_node);
				SGA_IntervalArrayList_destroy(times_mapped_node);
			}
		}
		return true;
	}

	for (int i = 0; i < n; i++) {
		if (!visited[i]) {
			visited[i]	= true;
			mapping[vertex] = i;

			if (check_all_possible_mappings(s1, s2, n, graph1, graph2, mapping, visited, vertex + 1, universal_offset_amount)) {
				return true;
			}

			visited[i] = false;
		}
	}

	return false;
}

void print_adjacency_matrix(size_t n, SGA_IntervalArrayList** graph) {
	printf("[");
	for (size_t i = 0; i < n; i++) {
		printf(" ");
		for (size_t j = 0; j < n; j++) {
			String str = SGA_IntervalArrayList_to_string(&graph[i][j]);
			printf("%-40s", str.data);
			String_destroy(str);
		}
		if (i < n - 1) {
			printf("\n");
		}
	}
	printf("]\n");
}

bool are_isomorphic(const SGA_Stream* s1, const SGA_Stream* s2) {
	// Check if the two streamgraphs have the same number of nodes and links
	if ((SGA_Stream_distinct_cardinal_of_node_set(s1) != SGA_Stream_distinct_cardinal_of_node_set(s2)) ||
	    (SGA_Stream_temporal_cardinal_of_link_set(s1) != SGA_Stream_temporal_cardinal_of_link_set(s2))) {
		return false;
	}

	size_t nb_nodes = SGA_Stream_distinct_cardinal_of_node_set(s1);

	StreamFunctions fns1 = STREAM_FUNCS(fns1, s1);
	StreamFunctions fns2 = STREAM_FUNCS(fns2, s2);

	SGA_Interval lifespan_g1 = fns1.lifespan(s1->stream_data);
	SGA_Interval lifespan_g2 = fns2.lifespan(s2->stream_data);

	// Offset between the two streams
	SGA_Offset universal_offset = SGA_Interval_offset_of(&lifespan_g1, &lifespan_g2);

	// Different lifespans, can't be isomorphic time-wise
	if (SGA_Offset_is_not_matching(universal_offset)) {
		return false;
	}

	size_t universal_offset_amount = SGA_Offset_unwrap(universal_offset);

	SGA_IntervalArrayList** streamgraph1 = MALLOC(nb_nodes * sizeof(SGA_IntervalArrayList*));
	SGA_IntervalArrayList** streamgraph2 = MALLOC(nb_nodes * sizeof(SGA_IntervalArrayList*));
	for (int i = 0; i < nb_nodes; i++) {
		streamgraph1[i] = MALLOC(nb_nodes * sizeof(SGA_IntervalArrayList));
		streamgraph2[i] = MALLOC(nb_nodes * sizeof(SGA_IntervalArrayList));
		for (int j = 0; j < nb_nodes; j++) {
			streamgraph1[i][j] = SGA_IntervalArrayList_with_capacity(1);
			streamgraph2[i][j] = SGA_IntervalArrayList_with_capacity(1);
		}
	}

	SGA_NodesIterator nodes1 = fns1.nodes_set(s1->stream_data);
	size_t nb_nodes_1	 = 0;
	size_t max_node_index_1	 = 0;
	SGA_FOR_EACH_NODE(node_id, nodes1) {
		nb_nodes_1++;
		if (node_id > max_node_index_1) {
			max_node_index_1 = node_id;
		}
	}

	SGA_NodesIterator nodes2 = fns2.nodes_set(s2->stream_data);
	size_t nb_nodes_2	 = 0;
	size_t max_node_index_2	 = 0;
	SGA_FOR_EACH_NODE(node_id, nodes2) {
		nb_nodes_2++;
		if (node_id > max_node_index_2) {
			max_node_index_2 = node_id;
		}
	}

	SGA_NodeIdArrayList nodes_mapping_1 = SGA_NodeIdArrayList_with_capacity(max_node_index_1 + 1);
	size_t* nodes_map_1		    = malloc((max_node_index_1 + 1) * sizeof(size_t));
	for (size_t i = 0; i < max_node_index_1 + 1; i++) {
		nodes_map_1[i] = SIZE_MAX;
	}
	nodes1 = fns1.nodes_set(s1->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes1) {
		SGA_NodeIdArrayList_push(&nodes_mapping_1, node_id);
		size_t size_after    = nodes_mapping_1.length;
		nodes_map_1[node_id] = size_after - 1;
	}

	SGA_NodeIdArrayList nodes_mapping_2 = SGA_NodeIdArrayList_with_capacity(max_node_index_2 + 1);
	size_t* nodes_map_2		    = malloc((max_node_index_2 + 1) * sizeof(size_t));
	for (size_t i = 0; i < max_node_index_2 + 1; i++) {
		nodes_map_2[i] = SIZE_MAX;
	}
	nodes2 = fns2.nodes_set(s2->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes2) {
		SGA_NodeIdArrayList_push(&nodes_mapping_2, node_id);
		size_t size_after    = nodes_mapping_2.length;
		nodes_map_2[node_id] = size_after - 1;
	}

	// Fill the adjacency matrix
	SGA_LinksIterator links1 = fns1.links_set(s1->stream_data);
	SGA_FOR_EACH_LINK(link_id, links1) {
		SGA_Link l			     = fns1.link_by_id(s1->stream_data, link_id);
		size_t u			     = nodes_map_1[l.nodes[0]];
		size_t v			     = nodes_map_1[l.nodes[1]];
		SGA_TimesIterator times_link_present = fns1.times_link_present(s1->stream_data, link_id);
		SGA_FOR_EACH_TIME(t, times_link_present) {
			SGA_IntervalArrayList_push(&streamgraph1[u][v], t);
			SGA_IntervalArrayList_push(&streamgraph1[v][u], t);
		}
	}

	SGA_LinksIterator links2 = fns2.links_set(s2->stream_data);
	SGA_FOR_EACH_LINK(link_id, links2) {
		SGA_Link l			     = fns2.link_by_id(s2->stream_data, link_id);
		size_t u			     = nodes_map_2[l.nodes[0]];
		size_t v			     = nodes_map_2[l.nodes[1]];
		SGA_TimesIterator times_link_present = fns2.times_link_present(s2->stream_data, link_id);
		SGA_FOR_EACH_TIME(t, times_link_present) {
			SGA_IntervalArrayList_push(&streamgraph2[u][v], t);
			SGA_IntervalArrayList_push(&streamgraph2[v][u], t);
		}
	}

	int* mapping  = MALLOC(nb_nodes * sizeof(int));
	bool* visited = MALLOC(nb_nodes * sizeof(bool));
	for (int i = 0; i < nb_nodes; i++) {
		visited[i] = false;
	}

	bool result =
	    check_all_possible_mappings(s1, s2, nb_nodes, streamgraph1, streamgraph2, mapping, visited, 0, universal_offset_amount);

	// Cleanup
	free(mapping);
	free(visited);
	for (int i = 0; i < nb_nodes; i++) {
		for (int j = 0; j < nb_nodes; j++) {
			SGA_IntervalArrayList_destroy(streamgraph1[i][j]);
			SGA_IntervalArrayList_destroy(streamgraph2[i][j]);
		}
		free(streamgraph1[i]);
		free(streamgraph2[i]);
	}
	free(streamgraph1);
	free(streamgraph2);
	SGA_NodeIdArrayList_destroy(nodes_mapping_1);
	SGA_NodeIdArrayList_destroy(nodes_mapping_2);
	free(nodes_map_1);
	free(nodes_map_2);

	return result;
}

#define COMPARE_METRIC(fn, s1, s2)                                                                                                         \
	if (fn(s1) != fn(s2)) {                                                                                                            \
		return false;                                                                                                              \
	}

bool are_probably_isomorphic(SGA_Stream* s1, SGA_Stream* s2) {
	COMPARE_METRIC(SGA_Stream_distinct_cardinal_of_node_set, s1, s2);
	COMPARE_METRIC(SGA_Stream_temporal_cardinal_of_link_set, s1, s2);
	COMPARE_METRIC(SGA_Stream_duration, s1, s2);
	COMPARE_METRIC(SGA_Stream_temporal_cardinal_of_node_set, s1, s2);

	COMPARE_METRIC(SGA_Stream_density, s1, s2);
	COMPARE_METRIC(SGA_Stream_average_node_degree, s1, s2);
	COMPARE_METRIC(SGA_Stream_uniformity, s1, s2);

	return true;
}