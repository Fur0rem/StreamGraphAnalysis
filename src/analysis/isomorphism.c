#include "isomorphism.h"
#include "../metrics.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_wrappers.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool areIsomorphic(size_t n, IntervalArrayList** graph1, IntervalArrayList** graph2, const Stream* s1, const Stream* s2,
		   NodeIdArrayList node_to_id_map1, NodeIdArrayList node_to_id_map2, size_t universal_offset);
bool isIsomorphicUtil(size_t n, IntervalArrayList** graph1, IntervalArrayList** graph2, int* mapping, bool* visited, int vertex,
		      size_t universal_offset);

// FIXME: leakkkkkkks
// TODO: switch to Laszlo Babai's algorithm
// or VF2++ (https://blog.scientific-python.org/networkx/vf2pp/graph-iso-vf2pp/)
// or Nauty
// (https://www3.cs.stonybrook.edu/~algorith/implement/nauty/implement.shtml)
// https://mivia.unisa.it/wp-content/uploads/2013/05/foggia01.pdf
// https://algorist.com/problems/Graph_Isomorphism.html

bool check_node_times(const Stream* s1, const Stream* s2, IntervalArrayList** graph1, IntervalArrayList** graph2, size_t n, int* mapping,
		      NodeIdArrayList node_to_id_map1, NodeIdArrayList node_to_id_map2, size_t universal_offset) {
	StreamFunctions fns1 = STREAM_FUNCS(fns1, s1);
	StreamFunctions fns2 = STREAM_FUNCS(fns2, s2);

	size_t global_offset = SIZE_MAX;

	// For each link in the graph, check if the time is offset by a constant
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			if (i == j) {
				continue;
			}

			IntervalArrayList* iv_og     = &graph1[i][j];
			IntervalArrayList* iv_mapped = &graph2[mapping[i]][mapping[j]];
			SGA_Offset offset_res	     = IntervalArrayList_offset_of(iv_og, iv_mapped);

			if (SGA_Offset_is_not_matching(offset_res)) {
				printf("Link between node %zu and %zu have different times\n", i, j);
				return false;
			}
			if (!SGA_Offset_is_empty(offset_res)) {
				size_t offset = SGA_Offset_unwrap(offset_res);
				if (global_offset == SIZE_MAX) {
					global_offset = offset;
				}
				else if (global_offset != offset) {
					printf("Link between node %zu and %zu have a different offset %zu\n", i, j, offset);
					return false;
				}
				printf("Link between node %zu and %zu have offset %zu\n", i, j, offset);
			}

			size_t real_node_id_1 = node_to_id_map1.array[i];
			size_t real_node_id_2 = node_to_id_map2.array[mapping[i]];

			TimesIterator times_iter1 = fns1.times_node_present(s1->stream_data, real_node_id_1);
			TimesIterator times_iter2 = fns2.times_node_present(s2->stream_data, real_node_id_2);

			IntervalArrayList iv1 = SGA_collect_times(times_iter1);
			IntervalArrayList iv2 = SGA_collect_times(times_iter2);

			offset_res = IntervalArrayList_offset_of(&iv1, &iv2);
			IntervalArrayList_destroy(iv1);
			IntervalArrayList_destroy(iv2);
			if (SGA_Offset_is_not_matching(offset_res)) {
				printf("Times for node %zu and mapped %zu have different times\n", real_node_id_1, real_node_id_2);
				return false;
			}
			if (!SGA_Offset_is_empty(offset_res)) {
				size_t offset = SGA_Offset_unwrap(offset_res);
				if (global_offset == SIZE_MAX) {
					global_offset = offset;
				}
				else if (global_offset != offset) {
					printf("Times for node %zu and mapped %zu have a different offset %zu\n",
					       real_node_id_1,
					       real_node_id_2,
					       offset);
					return false;
				}
				printf("Times for node %zu and mapped %zu have offset %zu\n", real_node_id_1, real_node_id_2, offset);
			}
		}
	}

	// Same offset for all nodes
	// for (size_t i = 0; i < n; i++) {
	// 	NodeId n1				  = n;
	// 	NodeId n2				  = mapping[i];
	// 	TimesIterator times_iter1 = fns1.times_node_present(s1->stream_data,
	// n1); 	TimesIterator times_iter2 =
	// fns2.times_node_present(s2->stream_data, n2);

	// 	printf("Original times for node %zu : ", n1);
	// 	FOR_EACH_TIME(interval, times_iter1) {
	// 		printf("[%lu, %lu] ", interval.start, interval.end);
	// 	}
	// 	printf("\n");
	// 	printf("Mapped times for node %zu : ", n2);
	// 	FOR_EACH_TIME(interval, times_iter2) {
	// 		printf("[%lu, %lu] ", interval.start, interval.end);
	// 	}

	// 	times_iter1		   = fns1.times_node_present(s1->stream_data,
	// n1); 	times_iter2		   =
	// fns2.times_node_present(s2->stream_data, n2); 	IntervalArrayList iv1 =
	// SGA_collect_times(times_iter1); 	IntervalArrayList iv2 =
	// SGA_collect_times(times_iter2);

	// 	size_t offset = IntervalArrayList_offset_of(&iv1, &iv2);
	// 	if (offset == SIZE_MAX) {
	// 		return false;
	// 	}
	// 	else if (global_offset == SIZE_MAX) {
	// 		global_offset = offset;
	// 	}
	// 	else if (global_offset != offset) {
	// 		return false;
	// 	}
	// }

	return true;
}
bool areIsomorphic(size_t n, IntervalArrayList** graph1, IntervalArrayList** graph2, const Stream* s1, const Stream* s2,
		   NodeIdArrayList node_to_id_map1, NodeIdArrayList node_to_id_map2, size_t universal_offset) {
	int* mapping  = malloc(n * sizeof(int));
	bool* visited = malloc(n * sizeof(bool));
	for (int i = 0; i < n; i++) {
		visited[i] = false;
	}

	bool result = isIsomorphicUtil(n, graph1, graph2, mapping, visited, 0, universal_offset);
	if (!result) {
		free(mapping);
		free(visited);
		return false;
	}
	// check node times
	result = check_node_times(s1, s2, graph1, graph2, n, mapping, node_to_id_map1, node_to_id_map2, universal_offset);
	free(mapping);
	free(visited);
	return result;
}

bool isIsomorphicUtil(size_t n, IntervalArrayList** graph1, IntervalArrayList** graph2, int* mapping, bool* visited, int vertex,
		      size_t universal_offset) {
	if (vertex == n) {
		// Check if the mapping preserves adjacency
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				// String str1 = IntervalArrayList_to_string(&graph1[i][j]);
				// String str2 =
				// IntervalArrayList_to_string(&graph2[mapping[i]][mapping[j]]);
				// printf("\ngraph1[%d][%d] = %s, graph2[%d][%d] = %s  ", i, j,
				// str1.data, mapping[i], mapping[j], 	   str2.data);
				SGA_Offset offset = IntervalArrayList_offset_of(&graph1[i][j], &graph2[mapping[i]][mapping[j]]);
				// if (offset == SIZE_MAX) {
				// 	// printf("Different\n");
				// 	return false;
				// }
				if (SGA_Offset_is_not_matching(offset)) {
					// printf("Different\n");
					return false;
				}
				if (SGA_Offset_is_empty(offset)) {
					// printf("Same because empty\n");
					continue;
				}
				size_t offset_ok = SGA_Offset_unwrap(offset);
				if (universal_offset != offset_ok) {
					// printf("Different by offset : %zu != %zu\n", universal_offset,
					// offset_ok);
					return false;
				}
				// printf("offset = %zu\n", offset);
				// if (IntervalArrayList_equals(&graph1[i][j],
				// &graph2[mapping[i]][mapping[j]])) {
				// 	// printf("Same\n");
				// 	// return false;
				// 	printf("Same because equals\n");
				// 	continue;
				// }
				// else {
				// 	// compare whether they are the same but offset by a constant
				// 	if (graph1[i][j].length !=
				// graph2[mapping[i]][mapping[j]].length) {
				// 		// printf("Different\n");
				// 		return false;
				// 	}
				// 	const size_t offset =
				// graph2[mapping[i]][mapping[j]].array[0].start -
				// graph1[i][j].array[0].start; 	for (size_t k = 0; k <
				// graph1[i][j].length; k++) { size_t offset_k =
				// graph2[mapping[i]][mapping[j]].array[k].start -
				// graph1[i][j].array[k].start; if (offset_k != offset) {
				// 			// printf("Different by offset : %zu != %zu\n",
				// offset_k, offset); 			return false;
				// 		}
				// 		size_t offset_k_end =
				// graph2[mapping[i]][mapping[j]].array[k].end -
				// graph1[i][j].array[k].end; 		if (offset_k_end != offset) {
				// 			// printf("Different by offset end : %zu !=
				// %zu\n", offset_k_end, offset); 			return false;
				// 		}
				// 	}
				// 	// printf("Same\n");
				// }
				// // if (IntervalArrayList_offset_of(&graph1[i][j],
				// &graph2[mapping[i]][mapping[j]]) == SIZE_MAX)
				// {
				// // return false;
				// // }

				// printf("Same because good offset : %zu\n", offset);
			}
		}
		return true;
	}

	for (int i = 0; i < n; i++) {
		if (!visited[i]) {
			visited[i]	= true;
			mapping[vertex] = i;

			if (isIsomorphicUtil(n, graph1, graph2, mapping, visited, vertex + 1, universal_offset)) {
				return true;
			}

			visited[i] = false;
		}
	}

	return false;
}

// TODO: check isomorphism for times of nodes

void print_adjacency_matrix(size_t n, IntervalArrayList** graph) {
	printf("[");
	for (int i = 0; i < n; i++) {
		printf(" ");
		for (int j = 0; j < n; j++) {
			String str = IntervalArrayList_to_string(&graph[i][j]);
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

	IntervalArrayList** streamgraph1 = MALLOC(cardinalOfV1 * sizeof(IntervalArrayList*));
	IntervalArrayList** streamgraph2 = MALLOC(cardinalOfV1 * sizeof(IntervalArrayList*));
	for (int i = 0; i < cardinalOfV1; i++) {
		streamgraph1[i] = MALLOC(cardinalOfV1 * sizeof(IntervalArrayList));
		streamgraph2[i] = MALLOC(cardinalOfV1 * sizeof(IntervalArrayList));
		for (int j = 0; j < cardinalOfV1; j++) {
			streamgraph1[i][j] = IntervalArrayList_with_capacity(1);
			streamgraph2[i][j] = IntervalArrayList_with_capacity(1);
		}
	}

	StreamFunctions fns1 = STREAM_FUNCS(fns1, s1);
	StreamFunctions fns2 = STREAM_FUNCS(fns2, s2);

	NodesIterator nodes1	= fns1.nodes_set(s1->stream_data);
	size_t nb_nodes_1	= 0;
	size_t max_node_index_1 = 0;
	FOR_EACH_NODE(node_id, nodes1) {
		nb_nodes_1++;
		if (node_id > max_node_index_1) {
			max_node_index_1 = node_id;
		}
	}

	NodesIterator nodes2	= fns2.nodes_set(s2->stream_data);
	size_t nb_nodes_2	= 0;
	size_t max_node_index_2 = 0;
	FOR_EACH_NODE(node_id, nodes2) {
		nb_nodes_2++;
		if (node_id > max_node_index_2) {
			max_node_index_2 = node_id;
		}
	}

	NodeIdArrayList nodes_mapping_1 = NodeIdArrayList_with_capacity(max_node_index_1 + 1);
	size_t* nodes_map_1		= malloc((max_node_index_1 + 1) * sizeof(size_t));
	for (size_t i = 0; i < max_node_index_1 + 1; i++) {
		nodes_map_1[i] = SIZE_MAX;
	}
	nodes1 = fns1.nodes_set(s1->stream_data);
	FOR_EACH_NODE(node_id, nodes1) {
		NodeIdArrayList_push(&nodes_mapping_1, node_id);
		size_t size_after    = nodes_mapping_1.length;
		nodes_map_1[node_id] = size_after - 1;
	}

	NodeIdArrayList nodes_mapping_2 = NodeIdArrayList_with_capacity(max_node_index_2 + 1);
	size_t* nodes_map_2		= malloc((max_node_index_2 + 1) * sizeof(size_t));
	for (size_t i = 0; i < max_node_index_2 + 1; i++) {
		nodes_map_2[i] = SIZE_MAX;
	}
	nodes2 = fns2.nodes_set(s2->stream_data);
	FOR_EACH_NODE(node_id, nodes2) {
		NodeIdArrayList_push(&nodes_mapping_2, node_id);
		size_t size_after    = nodes_mapping_2.length;
		nodes_map_2[node_id] = size_after - 1;
	}

	// Fill the adjacency matrix
	LinksIterator links1 = fns1.links_set(s1->stream_data);
	FOR_EACH_LINK(link_id, links1) {
		Link l = fns1.link_by_id(s1->stream_data, link_id);
		// printf("l.id = %zu, l.nodes = {%zu, %zu}\n", link_id, l.nodes[0],
		// l.nodes[1]);
		size_t u			 = nodes_map_1[l.nodes[0]];
		size_t v			 = nodes_map_1[l.nodes[1]];
		TimesIterator times_link_present = fns1.times_link_present(s1->stream_data, link_id);
		IntervalArrayList vec		 = SGA_collect_times(times_link_present);
		// IntervalArrayList_destroy(streamgraph1[v][u]);
		// IntervalArrayList_destroy(streamgraph1[u][v]);
		streamgraph1[u][v] = vec;
		streamgraph1[v][u] = vec;
	}

	LinksIterator links2 = fns2.links_set(s2->stream_data);
	FOR_EACH_LINK(link_id, links2) {
		Link l = fns2.link_by_id(s2->stream_data, link_id);
		// printf("l.id = %zu, l.nodes = {%zu, %zu}\n", link_id, l.nodes[0],
		// l.nodes[1]);
		size_t u			 = nodes_map_2[l.nodes[0]];
		size_t v			 = nodes_map_2[l.nodes[1]];
		TimesIterator times_link_present = fns2.times_link_present(s2->stream_data, link_id);
		IntervalArrayList vec		 = SGA_collect_times(times_link_present);
		// IntervalArrayList_destroy(streamgraph1[v][u]);
		// IntervalArrayList_destroy(streamgraph1[u][v]);
		streamgraph2[u][v] = vec;
		streamgraph2[v][u] = vec;
	}

	Interval lifespan_g1 = fns1.lifespan(s1->stream_data);
	Interval lifespan_g2 = fns2.lifespan(s2->stream_data);

	SGA_Offset offset_result = Interval_offset_of(&lifespan_g1, &lifespan_g2);
	if (SGA_Offset_is_not_matching(offset_result)) {
		// printf("Different lifespans\n");
		// TODO: merge cleanup
		for (int i = 0; i < cardinalOfV1; i++) {
			for (int j = i + 1; j < cardinalOfV1; j++) {
				// IntervalArrayList_destroy(streamgraph1[i][j]);
			}
			free(streamgraph1[i]);
			free(streamgraph2[i]);
		}
		free(streamgraph1);
		free(streamgraph2);
		free(nodes_map_1);
		free(nodes_map_2);
		NodeIdArrayList_destroy(nodes_mapping_1);
		NodeIdArrayList_destroy(nodes_mapping_2);

		return false;
	}

	size_t universal_offset = SGA_Offset_unwrap(offset_result);
	printf("Universal offset = %zu\n", universal_offset);

	// TODO: better name than universal_offset
	/*if (areIsomorphic(nb_nodes_1, streamgraph1, streamgraph2, s1, s2,
	nodes_mapping_1, nodes_mapping_2, universal_offset)) { return true;
	}
	else {
		return false;
	}*/
	bool are_isomorphic =
	    areIsomorphic(nb_nodes_1, streamgraph1, streamgraph2, s1, s2, nodes_mapping_1, nodes_mapping_2, universal_offset);
	for (int i = 0; i < cardinalOfV1; i++) {
		for (int j = i + 1; j < cardinalOfV1; j++) {
			// IntervalArrayList_destroy(streamgraph1[i][j]);
		}
		free(streamgraph1[i]);
		free(streamgraph2[i]);
	}
	free(streamgraph1);
	free(streamgraph2);
	NodeIdArrayList_destroy(nodes_mapping_1);
	NodeIdArrayList_destroy(nodes_mapping_2);
	free(nodes_map_1);
	free(nodes_map_2);

	return are_isomorphic;
}

#define COMPARE_METRIC(fn, s1, s2)                                                                                                         \
	if (fn(s1) != fn(s2)) {                                                                                                            \
		printf("different values for %s\n", #fn);                                                                                  \
		return false;                                                                                                              \
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