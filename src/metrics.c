#include "metrics.h"
#include "induced_graph.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream_functions.h"
#include "stream_graph.h"
#include "units.h"
#include "utils.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

// TODO : Confirm this
// I hope gcc or clang does this optimisation :
// ```
// int a = 0;
// int b = 0;
// if (condition) {
//     a = 5;
// }
// if (condition) {
//     b = 5;
// }
// ```
// into
// ```
// int a = 0;
// int b = 0;
// if (condition) {
//     a = 5;
//     b = 5;
// }
// ```
// Otherwise the performance is going to be baaaaaaaaaaaaaaaaaaaaad
#define CATCH_METRICS_IMPLEM(function, stream)                                                                         \
	switch (stream->type) {                                                                                            \
		case FULL_STREAM_GRAPH: {                                                                                      \
			if (FullStreamGraph_metrics_functions.function != NULL) {                                                  \
				return FullStreamGraph_metrics_functions.function(stream->stream);                                     \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case LINK_STREAM: {                                                                                            \
			if (LinkStream_metrics_functions.function != NULL) {                                                       \
				return LinkStream_metrics_functions.function(stream->stream);                                          \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case CHUNK_STREAM: {                                                                                           \
			if (ChunkStream_metrics_functions.function != NULL) {                                                      \
				return ChunkStream_metrics_functions.function(stream->stream);                                         \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case CHUNK_STREAM_SMALL: {                                                                                     \
			if (ChunkStreamSmall_metrics_functions.function != NULL) {                                                 \
				return ChunkStreamSmall_metrics_functions.function(stream->stream);                                    \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
	}                                                                                                                  \
	printf("COULD NOT CATCH " #function "\n");

// TODO : rename the functions to be more explicit
// TODO : rewrite them to be cleaner
size_t cardinalOfT(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfT);
	CATCH_METRICS_IMPLEM(cardinalOfT, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	Interval lifespan = stream_functions.lifespan(stream->stream);
	UPDATE_CACHE(stream, cardinalOfT, Interval_size(lifespan));
	return Interval_size(lifespan);
}

size_t cardinalOfV(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfV);
	CATCH_METRICS_IMPLEM(cardinalOfV, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	size_t count = COUNT_ITERATOR(nodes);
	UPDATE_CACHE(stream, cardinalOfV, count);
	return count;
}

size_t cardinalOfE(Stream* stream) {
	// CATCH_METRICS_IMPLEM(cardinalOfE, stream);
	FETCH_CACHE(stream, cardinalOfE);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator links = stream_functions.links_set(stream->stream);
	size_t count = 0;
	FOR_EACH_LINK(link_id, links) {
		TimesIterator times = stream_functions.times_link_present(stream->stream, link_id);
		count += total_time_of(times);
	}
	UPDATE_CACHE(stream, cardinalOfE, count);
	return count;
}

size_t cardinalOfW(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfW);
	CATCH_METRICS_IMPLEM(cardinalOfW, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	size_t count = 0;
	FOR_EACH_NODE(node_id, nodes) {
		TimesIterator times = stream_functions.times_node_present(nodes.stream_graph.stream, node_id);
		count += total_time_of(times);
	}
	UPDATE_CACHE(stream, cardinalOfW, count);
	return count;
}

double Stream_coverage(Stream* stream) {
	CATCH_METRICS_IMPLEM(coverage, stream);
	size_t w = cardinalOfW(stream);
	size_t t = cardinalOfT(stream);
	size_t v = cardinalOfV(stream);

	return (double)w / (double)(t * v);
}

double Stream_node_duration(Stream* stream) {
	CATCH_METRICS_IMPLEM(node_duration, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t w = cardinalOfW(stream);
	size_t v = cardinalOfV(stream);
	size_t scaling = stream_functions.scaling(stream->stream);
	return (double)w / (double)(v * scaling);
}

double Stream_contribution_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_node, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times = stream_functions.times_node_present(stream->stream, node_id);
	size_t t_v = total_time_of(times);
	size_t t = cardinalOfT(stream);
	return (double)t_v / (double)t;
}

double Stream_contribution_of_link(Stream* stream, LinkId link_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_link, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times = stream_functions.times_link_present(stream->stream, link_id);
	size_t t_v = total_time_of(times);
	size_t t = cardinalOfT(stream);
	return (double)t_v / (double)(t);
}

double Stream_number_of_nodes(Stream* stream) {
	// CATCH_METRICS_IMPLEM(number_of_nodes, stream);
	size_t w = cardinalOfW(stream);
	size_t t = cardinalOfT(stream);
	return (double)w / (double)t;
}

double Stream_number_of_links(Stream* stream) {
	// CATCH_METRICS_IMPLEM(number_of_links, stream);
	size_t e = cardinalOfE(stream);
	size_t t = cardinalOfT(stream);
	return (double)e / (double)t;
}

double Stream_node_contribution_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(node_contribution_at_time, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_present_at_t(stream->stream, time_id);
	size_t v_t = COUNT_ITERATOR(nodes);
	size_t scaling = stream_functions.scaling(stream->stream);
	size_t v = cardinalOfV(stream);
	return (double)v_t / (double)(v * scaling);
}

size_t size_set_unordered_pairs_itself(size_t n) {
	return n * (n - 1) / 2;
}

double Stream_link_contribution_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(link_contribution_at_time, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator links = stream_functions.links_present_at_t(stream->stream, time_id);
	size_t e_t = COUNT_ITERATOR(links);
	size_t scaling = stream_functions.scaling(stream->stream);
	size_t v = cardinalOfV(stream);
	size_t vxv = size_set_unordered_pairs_itself(v);
	return (double)e_t / (double)(vxv * scaling);
}

double Stream_link_duration(Stream* stream) {
	// CATCH_METRICS_IMPLEM(link_duration, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t e = cardinalOfE(stream);
	size_t v = cardinalOfV(stream);
	size_t scaling = stream_functions.scaling(stream->stream);
	size_t vxv = size_set_unordered_pairs_itself(v);
	return (double)e / (double)(vxv * scaling);
}

double Stream_uniformity(Stream* stream) {
	// CATCH_METRICS_IMPLEM(uniformity, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t sum_num = 0;
	size_t sum_den = 0;
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(node_id, nodes) {
		NodesIterator other_pair = stream_functions.nodes_set(stream->stream);
		FOR_EACH_NODE(other_node_id, other_pair) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node = stream_functions.times_node_present(stream->stream, node_id);
			TimesIterator times_other_node = stream_functions.times_node_present(stream->stream, other_node_id);
			TimesIterator times_union = TimesIterator_union(times_node, times_other_node);

			size_t t_u = total_time_of(times_union);
			times_node = stream_functions.times_node_present(stream->stream, node_id);
			times_other_node = stream_functions.times_node_present(stream->stream, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			size_t t_i = total_time_of(times_intersection);

			sum_num += t_i;
			sum_den += t_u;
		}
	}
	return (double)sum_num / (double)sum_den;
}

double Stream_uniformity_pair_nodes(Stream* stream, NodeId node1, NodeId node2) {
	// CATCH_METRICS_IMPLEM(uniformity_pair_nodes, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times_node1 = stream_functions.times_node_present(stream->stream, node1);
	TimesIterator times_node2 = stream_functions.times_node_present(stream->stream, node2);
	TimesIterator times_union = TimesIterator_union(times_node1, times_node2);

	size_t t_u = total_time_of(times_union);
	times_node1 = stream_functions.times_node_present(stream->stream, node1);
	times_node2 = stream_functions.times_node_present(stream->stream, node2);
	TimesIterator times_intersection = TimesIterator_intersection(times_node1, times_node2);
	size_t t_i = total_time_of(times_intersection);

	return (double)t_i / (double)t_u;
}

double Stream_density(Stream* stream) {
	CATCH_METRICS_IMPLEM(density, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t sum_den = 0;
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(node_id, nodes) {
		NodesIterator other_nodes = stream_functions.nodes_set(stream->stream);
		FOR_EACH_NODE(other_node_id, other_nodes) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node = stream_functions.times_node_present(stream->stream, node_id);
			TimesIterator times_other_node = stream_functions.times_node_present(stream->stream, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			sum_den += total_time_of(times_intersection);
		}
	}

	size_t sum_num = 0;
	LinksIterator links = stream_functions.links_set(stream->stream);
	FOR_EACH_LINK(link_id, links) {
		TimesIterator times_link = stream_functions.times_link_present(stream->stream, link_id);
		sum_num += total_time_of(times_link);
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_link(Stream* stream, LinkId link_id) {
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times_link = stream_functions.times_link_present(stream->stream, link_id);
	size_t sum_num = total_time_of(times_link);
	Link l = stream_functions.nth_link(stream->stream, link_id);
	TimesIterator t_u = stream_functions.times_node_present(stream->stream, l.nodes[0]);
	TimesIterator t_v = stream_functions.times_node_present(stream->stream, l.nodes[1]);
	TimesIterator t_i = TimesIterator_intersection(t_u, t_v);
	size_t sum_den = total_time_of(t_i);

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(density_of_node, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator neighbours = stream_functions.neighbours_of_node(stream->stream, node_id);
	size_t sum_num = 0;
	size_t sum_den = 0;
	FOR_EACH_LINK(link_id, neighbours) {
		TimesIterator times_link = stream_functions.times_link_present(stream->stream, link_id);
		sum_num += total_time_of(times_link);
	}

	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(other_node_id, nodes) {
		if (node_id == other_node_id) {
			continue;
		}
		TimesIterator times_node = stream_functions.times_node_present(stream->stream, node_id);
		TimesIterator times_other_node = stream_functions.times_node_present(stream->stream, other_node_id);
		TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
		sum_den += total_time_of(times_intersection);
	}
	printf("sum_num = %zu, sum_den = %zu\n", sum_num, sum_den);
	return (double)sum_num / (double)sum_den;
}

double Stream_density_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(density_at_instant, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes_at_t = stream_functions.nodes_present_at_t(stream->stream, time_id);
	LinksIterator links_at_t = stream_functions.links_present_at_t(stream->stream, time_id);
	// size_t et = COUNT_ITERATOR(links_at_t);
	size_t et = COUNT_ITERATOR(links_at_t);
	size_t vt = COUNT_ITERATOR(nodes_at_t);
	printf("et = %zu, vt = %zu\n", et, vt);
	return (double)et / (double)(size_set_unordered_pairs_itself(vt));
}

double Stream_degree_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(degree_of_node, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator neighbours = stream_functions.neighbours_of_node(stream->stream, node_id);
	size_t sum_num = 0;
	FOR_EACH_LINK(link_id, neighbours) {
		TimesIterator times_link = stream_functions.times_link_present(stream->stream, link_id);
		sum_num += total_time_of(times_link);
	}
	size_t sum_den = Interval_size(stream_functions.lifespan(stream->stream));
	return (double)sum_num / (double)sum_den;
}

double Stream_average_node_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(average_node_degree, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	double sum = 0;
	double w = (double)cardinalOfW(stream);
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(node_id, nodes) {
		double degree = Stream_degree_of_node(stream, node_id);
		size_t t_v = total_time_of(stream_functions.times_node_present(stream->stream, node_id));
		sum += degree * ((double)t_v / w);
	}
	return sum;
}

double Stream_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = cardinalOfE(stream);
	size_t t = cardinalOfT(stream);
	size_t v = cardinalOfV(stream);
	return (double)(2 * number_of_links) / (double)(t * v);
}

double Stream_average_expected_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = cardinalOfE(stream);
	size_t number_of_nodes = cardinalOfW(stream);
	return (double)(2 * number_of_links) / (double)number_of_nodes;
}

double Stream_clustering_coeff_of_node(Stream* stream, NodeId node_id) {
	NodeId v = node_id;
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);

	LinksIterator neighbours_of_v = stream_functions.neighbours_of_node(stream->stream, v);
	size_t sum_den = 0;
	size_t sum_num = 0;

	FOR_EACH_LINK(uv, neighbours_of_v) {
		LinksIterator neighbours_of_v_copy = stream_functions.neighbours_of_node(stream->stream, v);
		FOR_EACH_LINK(vw, neighbours_of_v_copy) {
			NodeId w, u;
			Link vw_link = stream_functions.nth_link(stream->stream, vw);
			if (vw_link.nodes[0] == v) {
				w = vw_link.nodes[1];
			}
			else {
				w = vw_link.nodes[0];
			}

			Link uv_link = stream_functions.nth_link(stream->stream, uv);
			if (uv_link.nodes[0] == v) {
				u = uv_link.nodes[1];
			}
			else {
				u = uv_link.nodes[0];
			}

			printf("u = %zu | v = %zu | w = %zu\n", u, v, w);

			if (u >= w) {
				continue;
			}
			printf("passed\n");

			TimesIterator times_uv = stream_functions.times_link_present(stream->stream, uv);
			TimesIterator times_vw = stream_functions.times_link_present(stream->stream, vw);

			TimesIterator times_uv_vw = TimesIterator_intersection(times_uv, times_vw);
			size_t t_uv_vw = total_time_of(times_uv_vw);
			sum_den += t_uv_vw;

			// Find link uw
			// OPTIMISE : Reuse the first intersections
			LinksIterator neighbours_of_u = stream_functions.neighbours_of_node(stream->stream, u);
			LinkId uw = SIZE_MAX;
			FOR_EACH_LINK(link_id, neighbours_of_u) {
				Link link = stream_functions.nth_link(stream->stream, link_id);
				if (link.nodes[0] == w || link.nodes[1] == w) {
					uw = link_id;
					break;
				}
			}
			if (uw == SIZE_MAX) {
				continue;
			}
			else {
				neighbours_of_u.destroy(&neighbours_of_u);
			}

			TimesIterator times_uw = stream_functions.times_link_present(stream->stream, uw);

			// reconstruct times_uv_vw
			times_uv = stream_functions.times_link_present(stream->stream, uv);
			times_vw = stream_functions.times_link_present(stream->stream, vw);
			times_uv_vw = TimesIterator_intersection(times_uv, times_vw);
			TimesIterator times_uw_uv_vw = TimesIterator_intersection(times_uw, times_uv_vw);
			size_t t_uw_uv_vw = total_time_of(times_uw_uv_vw);
			sum_num += t_uw_uv_vw;
		}
	}

	printf("sum_den = %zu, sum_num = %zu\n", sum_den, sum_num);

	return (double)sum_num / (double)sum_den;
}

double Stream_node_clustering_coeff(Stream* stream) {
	// CATCH_METRICS_IMPLEM(node_clustering_coeff, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	double sum = 0;
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(node_id, nodes) {
		double clustering_coeff = Stream_clustering_coeff_of_node(stream, node_id);
		size_t t_v = total_time_of(stream_functions.times_node_present(stream->stream, node_id));
		sum += clustering_coeff * ((double)t_v / (double)cardinalOfW(stream));
	}
	return sum;
}

typedef struct {
	int u, v, w, t
} ExpectedTriplets;

double Stream_transitivity_ratio(Stream* stream) {
	// CATCH_METRICS_IMPLEM(transitivity_ratio, stream);

	// Compute the number of triangles
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t sum_num = 0;
	size_t sum_den = 0;
	/*NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(node_id, nodes) {
		LinksIterator neighbours = stream_functions.neighbours_of_node(stream->stream, node_id);
		FOR_EACH_LINK(link_id, neighbours) {
			Link link = stream_functions.nth_link(stream->stream, link_id);
			NodeId neighbour = link.nodes[0] == node_id ? link.nodes[1] : link.nodes[0];
			LinksIterator neighbours_of_neighbour = stream_functions.neighbours_of_node(stream->stream, neighbour);
			FOR_EACH_LINK(neighbour_link_id, neighbours_of_neighbour) {
				Link neighbour_link = stream_functions.nth_link(stream->stream, neighbour_link_id);
				NodeId neighbour_neighbour =
					neighbour_link.nodes[0] == neighbour ? neighbour_link.nodes[1] : neighbour_link.nodes[0];
				if (neighbour_neighbour == node_id) {
					// check if it's an actual triplet of unique nodes
					NodeId nodes[3] = {node_id, neighbour, neighbour_neighbour};
					//if (!(nodes[0] != nodes[1] && nodes[0] != nodes[2] && nodes[1] != nodes[2])) {
					//	continue;
					//}
					// Triangle found
					TimesIterator times_u = stream_functions.times_link_present(stream->stream, link_id);
					TimesIterator times_v = stream_functions.times_link_present(stream->stream, neighbour_link_id);
					TimesIterator times_w = stream_functions.times_node_present(stream->stream, node_id);

					TimesIterator times_uv = TimesIterator_intersection(times_u, times_v);
					times_u = stream_functions.times_link_present(stream->stream, link_id);
					TimesIterator times_uw = TimesIterator_intersection(times_u, times_w);

					TimesIterator times_uv_uw = TimesIterator_intersection(times_uv, times_uw);
					size_t t_uv_uw = total_time_of(times_uv_uw);
					sum_num += t_uv_uw;

					// recompute uv
					times_u = stream_functions.times_link_present(stream->stream, link_id);
					times_v = stream_functions.times_link_present(stream->stream, neighbour_link_id);
					times_uv = TimesIterator_intersection(times_u, times_v);

					// recompute uw
					times_u = stream_functions.times_link_present(stream->stream, link_id);
					times_w = stream_functions.times_node_present(stream->stream, node_id);
					times_uw = TimesIterator_intersection(times_u, times_w);

					times_w = stream_functions.times_node_present(stream->stream, node_id);
					times_v = stream_functions.times_link_present(stream->stream, neighbour_link_id);
					TimesIterator times_vw = TimesIterator_intersection(times_v, times_w);

					// do the intersection of all 3
					times_uv_uw = TimesIterator_intersection(times_uw, times_uv);
					TimesIterator times_uv_uw_vw = TimesIterator_intersection(times_uv_uw, times_vw);
					size_t t_uv_uw_vw = total_time_of(times_uv_uw_vw);
					sum_den += t_uv_uw_vw;

					printf("a: %zu, b: %zu, c: %zu, time V: %zu, time triangle: %zu\n", node_id, neighbour,
						   neighbour_neighbour, t_uv_uw, t_uv_uw_vw);
				}
			}
		}
	}*/

	/*∨ = [2, 4] × {(b, a, c), (c, a, b)}
	∪ ([1, 4] ∪ [6, 8]) × {(a, b, c), (c, b, a)}
	∪ [7, 8] × {(c, b, d), (d, b, c)}
	∪ [7, 9] × {(a, b, d), (d, b, a)}
	∪ [2, 5] × {(a, c, b), (b, c, a)}
	∪ [6, 8] × {(b, c, d), (d, c, b)}
	∪ [7, 9] × {(b, d, c), (c, d, b)}*/
	const ExpectedTriplets expected[18] = {
	// In the line [x, y] × {(a, b, c), (c, b, a)}
	// It turns into one triplet with .t = (y - x), .u = a, .v = b, .w = c
	// And one triplet with .t = (y - x), .u = c, .v = b, .w = a
	// with a=0, b=1, c=2, d=3
		(ExpectedTriplets){.t = (4 - 2),			 .u = 1, .v = 0, .w = 2},
		(ExpectedTriplets){.t = (4 - 2),			 .u = 2, .v = 0, .w = 1},
		(ExpectedTriplets){.t = (4 - 1) + (8 - 6), .u = 0, .v = 1, .w = 2},
		(ExpectedTriplets){.t = (4 - 1) + (8 - 6), .u = 2, .v = 1, .w = 0},
		(ExpectedTriplets){.t = (8 - 7),			 .u = 2, .v = 1, .w = 3},
		(ExpectedTriplets){.t = (8 - 7),			 .u = 3, .v = 1, .w = 2},
		(ExpectedTriplets){.t = (9 - 7),			 .u = 0, .v = 1, .w = 3},
		(ExpectedTriplets){.t = (9 - 7),			 .u = 3, .v = 1, .w = 0},
		(ExpectedTriplets){.t = (5 - 2),			 .u = 0, .v = 2, .w = 1},
		(ExpectedTriplets){.t = (5 - 2),			 .u = 1, .v = 2, .w = 0},
		(ExpectedTriplets){.t = (8 - 6),			 .u = 1, .v = 2, .w = 3},
		(ExpectedTriplets){.t = (8 - 6),			 .u = 3, .v = 2, .w = 1},
		(ExpectedTriplets){.t = (9 - 7),			 .u = 1, .v = 3, .w = 2},
		(ExpectedTriplets){.t = (9 - 7),			 .u = 2, .v = 3, .w = 1},
	};
	NodesIterator nodes = stream_functions.nodes_set(stream->stream);
	FOR_EACH_NODE(u, nodes) {
		NodesIterator nodes2 = stream_functions.nodes_set(stream->stream);
		FOR_EACH_NODE(v, nodes2) {
			NodesIterator nodes3 = stream_functions.nodes_set(stream->stream);
			FOR_EACH_NODE(w, nodes3) {
				const int SPECIAL_U = 0;
				const int SPECIAL_V = 2;
				const int SPECIAL_W = 1;
				if (u == v || u == w || v == w) {
					continue;
				}
				/*if (u == SPECIAL_U && v == SPECIAL_V && w == SPECIAL_W) {
					printf(TEXT_BOLD "SPECIAL CASE\n");
				}*/
				LinkId uv, uw, vw;
				LinksIterator links = stream_functions.links_set(stream->stream);
				bool found_uv, found_uw, found_vw;
				found_uv = found_uw = found_vw = false;
				FOR_EACH_LINK(link_id, links) {
					Link link = stream_functions.nth_link(stream->stream, link_id);
					if ((link.nodes[0] == u && link.nodes[1] == v) || (link.nodes[0] == v && link.nodes[1] == u)) {
						uv = link_id;
						found_uv = true;
					}
					else if ((link.nodes[0] == u && link.nodes[1] == w) || (link.nodes[0] == w && link.nodes[1] == u)) {
						uw = link_id;
						found_uw = true;
					}
					else if ((link.nodes[0] == v && link.nodes[1] == w) || (link.nodes[0] == w && link.nodes[1] == v)) {
						vw = link_id;
						found_vw = true;
					}
				}

				// printf("uv: %zu, uw: %zu, vw: %zu\n", uv, uw, vw);
				if (found_uv && found_vw) {
					TimesIterator times_uv = stream_functions.times_link_present(stream->stream, uv);
					TimesIterator times_vw = stream_functions.times_link_present(stream->stream, vw);
					TimesIterator times_uv_N_vw = TimesIterator_intersection(times_uv, times_vw);

					size_t time_of_triplet = total_time_of(times_uv_N_vw);
					sum_den += time_of_triplet;
				}

				if (found_uv && found_vw && found_uw) {
					TimesIterator times_uv = stream_functions.times_link_present(stream->stream, uv);
					TimesIterator times_uw = stream_functions.times_link_present(stream->stream, uw);
					TimesIterator times_vw = stream_functions.times_link_present(stream->stream, vw);
					TimesIterator times_uv_N_uw = TimesIterator_intersection(times_uv, times_uw);
					TimesIterator times_uv_N_uw_N_vw = TimesIterator_intersection(times_uv_N_uw, times_vw);

					size_t time_of_triangle = total_time_of(times_uv_N_uw_N_vw);
					sum_num += time_of_triangle;
				}

				/*printf("u: %zu, v: %zu, w: %zu, time of triplet: %zu, time of triangle: %zu\n", u, v, w,
					   time_of_triplet, time_of_triangle);

				if (u == SPECIAL_U && v == SPECIAL_V && w == SPECIAL_W) {
					printf("END SPECIAL CASE\n" TEXT_RESET);
				}*/

				// look if the time of the triplet is the same as the expected time
				/*bool found = false;
				for (size_t i = 0; i < sizeof(expected) / sizeof(ExpectedTriplets); i++) {
					if (expected[i].u == u && expected[i].v == v && expected[i].w == w) {
						if (time_of_triplet != expected[i].t) {
							printf(TEXT_RED
								   "ERROR: for triplet %zu, %zu, %zu, expected time is %d, got %zu\n" TEXT_RESET,
								   u, v, w, expected[i].t, time_of_triplet);
						}
						else {
							printf(TEXT_GREEN
								   "SUCCESS: for triplet %zu, %zu, %zu, expected time is %d, got %zu\n" TEXT_RESET,
								   u, v, w, expected[i].t, time_of_triplet);
						}
						found = true;
					}
				}
				if (!found) {
					if (time_of_triplet != 0) {
						printf(TEXT_RED
							   "ERROR: for triplet %zu, %zu, %zu, expected time is not found, got %zu\n" TEXT_RESET,
							   u, v, w, time_of_triplet);
					}
					else {
						printf(TEXT_GREEN
							   "SUCCESS: for triplet %zu, %zu, %zu, expected time is not found, got %zu\n" TEXT_RESET,
							   u, v, w, time_of_triplet);
					}
				}*/
			}
		}
	}

	printf("sum_num = %zu, sum_den = %zu\n", sum_num, sum_den);

	return (double)sum_num / (double)sum_den;
}