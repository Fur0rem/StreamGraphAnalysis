#include "isomorphism.h"
#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_wrappers.h"

#include <stdbool.h>
#include <stdio.h>

bool areIsomorphic(size_t n, IntervalVector** graph1, IntervalVector** graph2);
bool isIsomorphicUtil(size_t n, IntervalVector** graph1, IntervalVector** graph2, int* mapping, bool* visited,
					  int vertex);

// FIXME: leakkkkkkks
bool areIsomorphic(size_t n, IntervalVector** graph1, IntervalVector** graph2) {
	int* mapping  = malloc(n * sizeof(int));
	bool* visited = malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++) {
		visited[i] = false;
	}

	return isIsomorphicUtil(n, graph1, graph2, mapping, visited, 0);
}

bool isIsomorphicUtil(size_t n, IntervalVector** graph1, IntervalVector** graph2, int* mapping, bool* visited,
					  int vertex) {
	if (vertex == n) {
		// Check if the mapping preserves adjacency
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				// String str1 = IntervalVector_to_string(&graph1[i][j]);
				// String str2 = IntervalVector_to_string(&graph2[mapping[i]][mapping[j]]);
				// printf("graph1[%d][%d] = %s, graph2[%d][%d] = %s  ", i, j, str1.data, mapping[i], mapping[j],
				// 	   str2.data);
				if (IntervalVector_equals(&graph1[i][j], &graph2[mapping[i]][mapping[j]])) {
					// printf("Same\n");
					// return false;
					continue;
				}
				else {
					// compare whether they are the same but offset by a constant
					if (graph1[i][j].size != graph2[mapping[i]][mapping[j]].size) {
						// printf("Different\n");
						return false;
					}
					const size_t offset = graph2[mapping[i]][mapping[j]].array[0].start - graph1[i][j].array[0].start;
					for (size_t k = 0; k < graph1[i][j].size; k++) {
						size_t offset_k = graph2[mapping[i]][mapping[j]].array[k].start - graph1[i][j].array[k].start;
						if (offset_k != offset) {
							// printf("Different by offset : %zu != %zu\n", offset_k, offset);
							return false;
						}
					}
					// printf("Same\n");
				}
				// printf("Same\n");
			}
		}
		return true;
	}

	for (int i = 0; i < n; i++) {
		if (!visited[i]) {
			visited[i]		= true;
			mapping[vertex] = i;

			if (isIsomorphicUtil(n, graph1, graph2, mapping, visited, vertex + 1)) {
				return true;
			}

			visited[i] = false;
		}
	}

	return false;
}

void print_adjacency_matrix(size_t n, IntervalVector** graph) {
	printf("[");
	for (int i = 0; i < n; i++) {
		printf(" ");
		for (int j = 0; j < n; j++) {
			String str = IntervalVector_to_string(&graph[i][j]);
			printf("%-40s", str.data);
			String_destroy(str);
		}
		if (i < n - 1) {
			printf("\n");
		}
	}
	printf("]\n");
}

bool are_isomorphic(const Stream* s1, const Stream* s2) {
	size_t cardinalOfV1 = cardinalOfV(s1);
	size_t cardinalOfV2 = cardinalOfV(s2);
	if (cardinalOfV1 != cardinalOfV2) {
		return false;
	}

	IntervalVector** streamgraph1 = MALLOC(cardinalOfV1 * sizeof(IntervalVector*));
	IntervalVector** streamgraph2 = MALLOC(cardinalOfV1 * sizeof(IntervalVector*));
	for (int i = 0; i < cardinalOfV1; i++) {
		streamgraph1[i] = MALLOC(cardinalOfV1 * sizeof(IntervalVector));
		streamgraph2[i] = MALLOC(cardinalOfV1 * sizeof(IntervalVector));
		for (int j = 0; j < cardinalOfV1; j++) {
			streamgraph1[i][j] = IntervalVector_with_capacity(1);
			streamgraph2[i][j] = IntervalVector_with_capacity(1);
		}
	}

	StreamFunctions fns1 = STREAM_FUNCS(fns1, s1);
	StreamFunctions fns2 = STREAM_FUNCS(fns2, s2);

	NodesIterator nodes1	= fns1.nodes_set(s1->stream_data);
	size_t nb_nodes_1		= 0;
	size_t max_node_index_1 = 0;
	FOR_EACH_NODE(node_id, nodes1) {
		nb_nodes_1++;
		if (node_id > max_node_index_1) {
			max_node_index_1 = node_id;
		}
	}

	NodesIterator nodes2	= fns2.nodes_set(s2->stream_data);
	size_t nb_nodes_2		= 0;
	size_t max_node_index_2 = 0;
	FOR_EACH_NODE(node_id, nodes2) {
		nb_nodes_2++;
		if (node_id > max_node_index_2) {
			max_node_index_2 = node_id;
		}
	}

	NodeIdVector nodes_mapping_1 = NodeIdVector_with_capacity(max_node_index_1 + 1);
	size_t* nodes_map_1			 = malloc((max_node_index_1 + 1) * sizeof(size_t));
	for (size_t i = 0; i < max_node_index_1 + 1; i++) {
		nodes_map_1[i] = SIZE_MAX;
	}
	nodes1 = fns1.nodes_set(s1->stream_data);
	FOR_EACH_NODE(node_id, nodes1) {
		NodeIdVector_push(&nodes_mapping_1, node_id);
		size_t size_after	 = nodes_mapping_1.size;
		nodes_map_1[node_id] = size_after - 1;
	}
	// printf("nodes_map_1 = ");
	// for (size_t i = 0; i < max_node_index_1 + 1; i++) {
	// 	printf("%zu ", nodes_map_1[i]);
	// }
	// printf("\n");

	NodeIdVector nodes_mapping_2 = NodeIdVector_with_capacity(max_node_index_2 + 1);
	size_t* nodes_map_2			 = malloc((max_node_index_2 + 1) * sizeof(size_t));
	for (size_t i = 0; i < max_node_index_2 + 1; i++) {
		nodes_map_2[i] = SIZE_MAX;
	}
	nodes2 = fns2.nodes_set(s2->stream_data);
	FOR_EACH_NODE(node_id, nodes2) {
		NodeIdVector_push(&nodes_mapping_2, node_id);
		size_t size_after	 = nodes_mapping_2.size;
		nodes_map_2[node_id] = size_after - 1;
	}
	// printf("nodes_map_2 = ");
	// for (size_t i = 0; i < max_node_index_2 + 1; i++) {
	// 	printf("%zu ", nodes_map_2[i]);
	// }
	// printf("\n");

	// printf("nb_nodes_1 = %zu, nb_nodes_2 = %zu\n", nb_nodes_1, nb_nodes_2);
	// printf("max_node_index_1 = %zu, max_node_index_2 = %zu\n", max_node_index_1, max_node_index_2);

	// Fill the adjacency matrix
	LinksIterator links1 = fns1.links_set(s1->stream_data);
	FOR_EACH_LINK(link_id, links1) {
		Link l = fns1.link_by_id(s1->stream_data, link_id);
		// printf("l.id = %zu, l.nodes = {%zu, %zu}\n", link_id, l.nodes[0], l.nodes[1]);
		size_t u						 = nodes_map_1[l.nodes[0]];
		size_t v						 = nodes_map_1[l.nodes[1]];
		TimesIterator times_link_present = fns1.times_link_present(s1->stream_data, link_id);
		IntervalVector vec				 = SGA_collect_times(times_link_present);
		streamgraph1[u][v]				 = vec;
		streamgraph1[v][u]				 = vec;
	}

	LinksIterator links2 = fns2.links_set(s2->stream_data);
	FOR_EACH_LINK(link_id, links2) {
		Link l = fns2.link_by_id(s2->stream_data, link_id);
		// printf("l.id = %zu, l.nodes = {%zu, %zu}\n", link_id, l.nodes[0], l.nodes[1]);
		size_t u						 = nodes_map_2[l.nodes[0]];
		size_t v						 = nodes_map_2[l.nodes[1]];
		TimesIterator times_link_present = fns2.times_link_present(s2->stream_data, link_id);
		IntervalVector vec				 = SGA_collect_times(times_link_present);
		streamgraph2[u][v]				 = vec;
		streamgraph2[v][u]				 = vec;
	}

	// print_adjacency_matrix(nb_nodes_1, streamgraph1);
	// print_adjacency_matrix(nb_nodes_2, streamgraph2);

	if (areIsomorphic(nb_nodes_1, streamgraph1, streamgraph2)) {
		// printf("The graphs are isomorphic.\n");
		return true;
	}
	else {
		// printf("The graphs are not isomorphic.\n");
		return false;
	}
}

#define COMPARE_METRIC(fn, s1, s2)                                                                                     \
	if (fn(s1) != fn(s2)) {                                                                                            \
		printf("different values for %s\n", #fn);                                                                      \
		return false;                                                                                                  \
	}

bool are_probably_isomorphic(const Stream* s1, const Stream* s2) {
	COMPARE_METRIC(cardinalOfV, s1, s2);
	COMPARE_METRIC(cardinalOfE, s1, s2);
	COMPARE_METRIC(cardinalOfT, s1, s2);
	COMPARE_METRIC(cardinalOfW, s1, s2);

	COMPARE_METRIC(Stream_density, s1, s2);
	COMPARE_METRIC(Stream_average_node_degree, s1, s2);
	COMPARE_METRIC(Stream_uniformity, s1, s2);

	return true;
}