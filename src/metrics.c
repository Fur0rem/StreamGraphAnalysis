#include "metrics.h"
#include "induced_graph.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream/chunk_stream.h"
#include "stream/chunk_stream_small.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream/snapshot_stream.h"
#include "stream_functions.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "units.h"
#include "utils.h"
#include "vector.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define CATCH_METRICS_IMPLEM(function, stream)                                                                         \
	switch (stream->type) {                                                                                            \
		case FULL_STREAM_GRAPH: {                                                                                      \
			if (FullStreamGraph_metrics_functions.function != NULL) {                                                  \
				return FullStreamGraph_metrics_functions.function(stream);                                             \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case LINK_STREAM: {                                                                                            \
			if (LinkStream_metrics_functions.function != NULL) {                                                       \
				return LinkStream_metrics_functions.function(stream);                                                  \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case CHUNK_STREAM: {                                                                                           \
			if (ChunkStream_metrics_functions.function != NULL) {                                                      \
				return ChunkStream_metrics_functions.function(stream);                                                 \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case CHUNK_STREAM_SMALL: {                                                                                     \
			if (ChunkStreamSmall_metrics_functions.function != NULL) {                                                 \
				return ChunkStreamSmall_metrics_functions.function(stream);                                            \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case SNAPSHOT_STREAM: {                                                                                        \
			if (SnapshotStream_metrics_functions.function != NULL) {                                                   \
				return SnapshotStream_metrics_functions.function(stream);                                              \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
	}

// TODO : rename the functions to be more explicit
// TODO : rewrite them to be cleaner
size_t cardinalOfT(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfT);
	CATCH_METRICS_IMPLEM(cardinalOfT, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	Interval lifespan	= fns.lifespan(stream->stream_data);
	UPDATE_CACHE(stream, cardinalOfT, Interval_size(lifespan));
	return Interval_size(lifespan);
}

size_t cardinalOfV(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfV);
	CATCH_METRICS_IMPLEM(cardinalOfV, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	size_t count		= COUNT_ITERATOR(nodes);
	UPDATE_CACHE(stream, cardinalOfV, count);
	return count;
}

size_t cardinalOfE(Stream* stream) {
	// CATCH_METRICS_IMPLEM(cardinalOfE, stream);
	FETCH_CACHE(stream, cardinalOfE);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	LinksIterator links = fns.links_set(stream->stream_data);
	size_t count		= 0;
	FOR_EACH_LINK(link_id, links) {
		TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
		count += total_time_of(times);
	}
	UPDATE_CACHE(stream, cardinalOfE, count);
	return count;
}

size_t cardinalOfW(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfW);
	CATCH_METRICS_IMPLEM(cardinalOfW, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	size_t count		= 0;
	FOR_EACH_NODE(node_id, nodes) {
		TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
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
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t w			= cardinalOfW(stream);
	size_t v			= cardinalOfV(stream);
	size_t scaling		= fns.scaling(stream->stream_data);
	return (double)w / (double)(v * scaling);
}

double Stream_contribution_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_node, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
	size_t t_v			= total_time_of(times);
	size_t t			= cardinalOfT(stream);
	return (double)t_v / (double)t;
}

double Stream_contribution_of_link(Stream* stream, LinkId link_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_link, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
	size_t t_v			= total_time_of(times);
	size_t t			= cardinalOfT(stream);
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
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	NodesIterator nodes = fns.nodes_present_at_t(stream->stream_data, time_id);
	size_t v_t			= COUNT_ITERATOR(nodes);
	size_t scaling		= fns.scaling(stream->stream_data);
	size_t v			= cardinalOfV(stream);
	return (double)v_t / (double)(v * scaling);
}

size_t size_set_unordered_pairs_itself(size_t n) {
	return n * (n - 1) / 2;
}

double Stream_link_contribution_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(link_contribution_at_time, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	LinksIterator links = fns.links_present_at_t(stream->stream_data, time_id);
	size_t e_t			= COUNT_ITERATOR(links);
	size_t scaling		= fns.scaling(stream->stream_data);
	size_t v			= cardinalOfV(stream);
	size_t vxv			= size_set_unordered_pairs_itself(v);
	return (double)e_t / (double)(vxv * scaling);
}

double Stream_link_duration(Stream* stream) {
	// CATCH_METRICS_IMPLEM(link_duration, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t e			= cardinalOfE(stream);
	size_t v			= cardinalOfV(stream);
	size_t scaling		= fns.scaling(stream->stream_data);
	size_t vxv			= size_set_unordered_pairs_itself(v);
	return (double)e / (double)(vxv * scaling);
}

double Stream_uniformity(Stream* stream) {
	// CATCH_METRICS_IMPLEM(uniformity, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_num		= 0;
	size_t sum_den		= 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		NodesIterator other_pair = fns.nodes_set(stream->stream_data);
		FOR_EACH_NODE(other_node_id, other_pair) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node	   = fns.times_node_present(stream->stream_data, node_id);
			TimesIterator times_other_node = fns.times_node_present(stream->stream_data, other_node_id);
			TimesIterator times_union	   = TimesIterator_union(times_node, times_other_node);

			size_t t_u						 = total_time_of(times_union);
			times_node						 = fns.times_node_present(stream->stream_data, node_id);
			times_other_node				 = fns.times_node_present(stream->stream_data, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			size_t t_i						 = total_time_of(times_intersection);

			sum_num += t_i;
			sum_den += t_u;
		}
	}
	return (double)sum_num / (double)sum_den;
}

double Stream_uniformity_pair_nodes(Stream* stream, NodeId node1, NodeId node2) {
	// CATCH_METRICS_IMPLEM(uniformity_pair_nodes, stream);
	StreamFunctions fns		  = STREAM_FUNCS(fns, stream);
	TimesIterator times_node1 = fns.times_node_present(stream->stream_data, node1);
	TimesIterator times_node2 = fns.times_node_present(stream->stream_data, node2);
	TimesIterator times_union = TimesIterator_union(times_node1, times_node2);

	size_t t_u						 = total_time_of(times_union);
	times_node1						 = fns.times_node_present(stream->stream_data, node1);
	times_node2						 = fns.times_node_present(stream->stream_data, node2);
	TimesIterator times_intersection = TimesIterator_intersection(times_node1, times_node2);
	size_t t_i						 = total_time_of(times_intersection);

	return (double)t_i / (double)t_u;
}

double Stream_density(Stream* stream) {
	CATCH_METRICS_IMPLEM(density, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_den		= 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		NodesIterator other_nodes = fns.nodes_set(stream->stream_data);
		FOR_EACH_NODE(other_node_id, other_nodes) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node		 = fns.times_node_present(stream->stream_data, node_id);
			TimesIterator times_other_node	 = fns.times_node_present(stream->stream_data, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			sum_den += total_time_of(times_intersection);
		}
	}

	size_t sum_num		= 0;
	LinksIterator links = fns.links_set(stream->stream_data);
	FOR_EACH_LINK(link_id, links) {
		TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += total_time_of(times_link);
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_link(Stream* stream, LinkId link_id) {
	StreamFunctions fns		 = STREAM_FUNCS(fns, stream);
	TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
	size_t sum_num			 = total_time_of(times_link);
	Link l					 = fns.link_by_id(stream->stream_data, link_id);
	TimesIterator t_u		 = fns.times_node_present(stream->stream_data, l.nodes[0]);
	TimesIterator t_v		 = fns.times_node_present(stream->stream_data, l.nodes[1]);
	TimesIterator t_i		 = TimesIterator_intersection(t_u, t_v);
	size_t sum_den			 = total_time_of(t_i);

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(density_of_node, stream);
	StreamFunctions fns		 = STREAM_FUNCS(fns, stream);
	LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	size_t sum_num			 = 0;
	size_t sum_den			 = 0;
	FOR_EACH_LINK(link_id, neighbours) {
		TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += total_time_of(times_link);
	}

	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(other_node_id, nodes) {
		if (node_id == other_node_id) {
			continue;
		}
		TimesIterator times_node		 = fns.times_node_present(stream->stream_data, node_id);
		TimesIterator times_other_node	 = fns.times_node_present(stream->stream_data, other_node_id);
		TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
		sum_den += total_time_of(times_intersection);
	}
	// printf("sum_num = %zu, sum_den = %zu\n", sum_num, sum_den);
	return (double)sum_num / (double)sum_den;
}

double Stream_density_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(density_at_instant, stream);
	StreamFunctions fns		 = STREAM_FUNCS(fns, stream);
	NodesIterator nodes_at_t = fns.nodes_present_at_t(stream->stream_data, time_id);
	LinksIterator links_at_t = fns.links_present_at_t(stream->stream_data, time_id);
	// size_t et = COUNT_ITERATOR(links_at_t);
	size_t et = COUNT_ITERATOR(links_at_t);
	size_t vt = COUNT_ITERATOR(nodes_at_t);
	printf("et = %zu, vt = %zu\n", et, vt);
	return (double)et / (double)(size_set_unordered_pairs_itself(vt));
}

double Stream_degree_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(degree_of_node, stream);
	StreamFunctions fns		 = STREAM_FUNCS(fns, stream);
	LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	size_t sum_num			 = 0;
	FOR_EACH_LINK(link_id, neighbours) {
		TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += total_time_of(times_link);
	}
	size_t sum_den = Interval_size(fns.lifespan(stream->stream_data));
	return (double)sum_num / (double)sum_den;
}

double Stream_average_node_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(average_node_degree, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	double sum			= 0;
	double w			= (double)cardinalOfW(stream);
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		double degree = Stream_degree_of_node(stream, node_id);
		size_t t_v	  = total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += degree * ((double)t_v / w);
	}
	return sum;
}

double Stream_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = cardinalOfE(stream);
	size_t t			   = cardinalOfT(stream);
	size_t v			   = cardinalOfV(stream);
	return (double)(2 * number_of_links) / (double)(t * v);
}

double Stream_average_expected_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = cardinalOfE(stream);
	size_t number_of_nodes = cardinalOfW(stream);
	return (double)(2 * number_of_links) / (double)number_of_nodes;
}

double Stream_clustering_coeff_of_node(Stream* stream, NodeId node_id) {
	NodeId v			= node_id;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	LinksIterator neighbours_of_v = fns.neighbours_of_node(stream->stream_data, v);
	size_t sum_den				  = 0;
	size_t sum_num				  = 0;

	FOR_EACH_LINK(uv, neighbours_of_v) {
		LinksIterator neighbours_of_v_copy = fns.neighbours_of_node(stream->stream_data, v);
		FOR_EACH_LINK(vw, neighbours_of_v_copy) {
			Link vw_link = fns.link_by_id(stream->stream_data, vw);
			NodeId w	 = Link_get_other_node(&vw_link, v);
			Link uv_link = fns.link_by_id(stream->stream_data, uv);
			NodeId u	 = Link_get_other_node(&uv_link, v);

			if (u >= w) {
				continue;
			}

			TimesIterator times_uv = fns.times_link_present(stream->stream_data, uv);
			TimesIterator times_vw = fns.times_link_present(stream->stream_data, vw);

			TimesIterator times_uv_vw = TimesIterator_intersection(times_uv, times_vw);
			size_t t_uv_vw			  = total_time_of(times_uv_vw);
			sum_den += t_uv_vw;

			LinkId uw = fns.links_between_nodes(stream->stream_data, u, w);
			if (uw == SIZE_MAX) {
				continue;
			}

			TimesIterator times_uw = fns.times_link_present(stream->stream_data, uw);

			// OPTIMISE : Reuse the first intersections
			// reconstruct times_uv_vw
			times_uv					 = fns.times_link_present(stream->stream_data, uv);
			times_vw					 = fns.times_link_present(stream->stream_data, vw);
			times_uv_vw					 = TimesIterator_intersection(times_uv, times_vw);
			TimesIterator times_uw_uv_vw = TimesIterator_intersection(times_uw, times_uv_vw);
			size_t t_uw_uv_vw			 = total_time_of(times_uw_uv_vw);
			sum_num += t_uw_uv_vw;
		}
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_node_clustering_coeff(Stream* stream) {
	// CATCH_METRICS_IMPLEM(node_clustering_coeff, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	double sum			= 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		double clustering_coeff = Stream_clustering_coeff_of_node(stream, node_id);
		size_t t_v				= total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += clustering_coeff * ((double)t_v / (double)cardinalOfW(stream));
	}
	return sum;
}

double Stream_transitivity_ratio(Stream* stream) {
	// CATCH_METRICS_IMPLEM(transitivity_ratio, stream);

	// Compute the number of triangles
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_num		= 0;
	size_t sum_den		= 0;

	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(u, nodes) {
		LinksIterator n_u = fns.neighbours_of_node(stream->stream_data, u);
		FOR_EACH_LINK(uv, n_u) {
			Link link_uv	  = fns.link_by_id(stream->stream_data, uv);
			NodeId v		  = Link_get_other_node(&link_uv, u);
			LinksIterator n_v = fns.neighbours_of_node(stream->stream_data, v);
			FOR_EACH_LINK(vw, n_v) {
				Link link_vw = fns.link_by_id(stream->stream_data, vw);
				NodeId w	 = Link_get_other_node(&link_vw, v);

				// If (u,v,w) is present, so is (w,v,u), and therefore half can be skipped.
				if (w >= u) {
					continue;
				}

				LinkId uw = fns.links_between_nodes(stream->stream_data, u, w);

				TimesIterator times_uv		= fns.times_link_present(stream->stream_data, uv);
				TimesIterator times_vw		= fns.times_link_present(stream->stream_data, vw);
				TimesIterator times_uv_N_vw = TimesIterator_intersection(times_uv, times_vw);

				size_t time_of_triplet = total_time_of(times_uv_N_vw);
				sum_den += time_of_triplet;

				if (uw != SIZE_MAX) { // Triangle
					TimesIterator times_uv			 = fns.times_link_present(stream->stream_data, uv);
					TimesIterator times_uw			 = fns.times_link_present(stream->stream_data, uw);
					TimesIterator times_vw			 = fns.times_link_present(stream->stream_data, vw);
					TimesIterator times_uv_N_uw		 = TimesIterator_intersection(times_uv, times_uw);
					TimesIterator times_uv_N_uw_N_vw = TimesIterator_intersection(times_uv_N_uw, times_vw);

					size_t time_of_triangle = total_time_of(times_uv_N_uw_N_vw);
					sum_num += time_of_triangle;
				}
			}
		}
	}

	return (double)sum_num / (double)sum_den;
}

bool Stream_equals(const Stream* stream1, const Stream* stream2) {
	StreamFunctions fn1 = STREAM_FUNCS(fn1, stream1);
	StreamFunctions fn2 = STREAM_FUNCS(fn2, stream2);

	NodeIdVector nodes_set_1 = SGA_collect_node_ids(fn1.nodes_set(stream1->stream_data));
	NodeIdVector nodes_set_2 = SGA_collect_node_ids(fn2.nodes_set(stream2->stream_data));

	if (!NodeIdVector_equals(&nodes_set_1, &nodes_set_2)) {
		NodeIdVector_destroy(nodes_set_1);
		NodeIdVector_destroy(nodes_set_2);
		printf("Nodes set are different\n");
		return false;
	}

	for (size_t n = 0; n < nodes_set_1.size; n++) {
		NodeId node = nodes_set_1.array[n];

		// Compare their presence
		IntervalVector times_n1 = SGA_collect_times(fn1.times_node_present(stream1->stream_data, node));
		IntervalVector times_n2 = SGA_collect_times(fn2.times_node_present(stream2->stream_data, node));
		if (!IntervalVector_equals(&times_n1, &times_n2)) {
			IntervalVector_destroy(times_n1);
			IntervalVector_destroy(times_n2);
			// TODO: remove repetition of destroy
			NodeIdVector_destroy(nodes_set_1);
			NodeIdVector_destroy(nodes_set_2);
			printf("Times of node %zu are different\n", node);
			return false;
		}
		IntervalVector_destroy(times_n1);
		IntervalVector_destroy(times_n2);

		// Compare their neighbours
		LinkIdVector neighbours_n1 = SGA_collect_link_ids(fn1.neighbours_of_node(stream1->stream_data, node));
		LinkIdVector neighbours_n2 = SGA_collect_link_ids(fn2.neighbours_of_node(stream2->stream_data, node));

		if (neighbours_n1.size != neighbours_n2.size) {
			LinkIdVector_destroy(neighbours_n1);
			LinkIdVector_destroy(neighbours_n2);
			NodeIdVector_destroy(nodes_set_1);
			NodeIdVector_destroy(nodes_set_2);
			printf("Neighbours of node %zu are different\n", node);
			return false;
		}

		// Double inclusion (since a link is defined by 2 nodes, their ID doesn't matter)
		LinkIdVector links_vectors[2] = {neighbours_n1, neighbours_n2};
		for (size_t i = 0; i < 2; i++) {

			for (size_t l = 0; l < neighbours_n1.size; l++) {
				LinkId lid	  = neighbours_n1.array[l];
				Link l1		  = fn1.link_by_id(stream1->stream_data, lid);
				NodeId neigh1 = Link_get_other_node(&l1, node);

				// Find the matching link in the other stream (by the other node)
				bool found = false;
				for (size_t l2 = 0; l2 < neighbours_n2.size; l2++) {
					LinkId lid2	  = neighbours_n2.array[l2];
					Link l2		  = fn2.link_by_id(stream2->stream_data, lid2);
					NodeId neigh2 = Link_get_other_node(&l2, node);

					if (neigh1 == neigh2) { // We found the same neighbour
						// Compare their presence
						IntervalVector times_l1 = SGA_collect_times(fn1.times_link_present(stream1->stream_data, lid));
						IntervalVector times_l2 = SGA_collect_times(fn2.times_link_present(stream2->stream_data, lid2));
						if (!IntervalVector_equals(&times_l1, &times_l2)) {
							IntervalVector_destroy(times_l1);
							IntervalVector_destroy(times_l2);
							NodeIdVector_destroy(nodes_set_1);
							NodeIdVector_destroy(nodes_set_2);
							LinkIdVector_destroy(neighbours_n1);
							LinkIdVector_destroy(neighbours_n2);
							printf("Times of link %zu are different\n", lid);
							return false;
						}
						IntervalVector_destroy(times_l1);
						IntervalVector_destroy(times_l2);
						found = true;
						break;
					}
				}

				if (!found) {
					LinkIdVector_destroy(neighbours_n1);
					LinkIdVector_destroy(neighbours_n2);
					NodeIdVector_destroy(nodes_set_1);
					NodeIdVector_destroy(nodes_set_2);
					printf("Neighbours of node %zu are different\n", node);
					return false;
				}
			}

			// Swap the vectors
			LinkIdVector tmp = links_vectors[0];
			links_vectors[0] = links_vectors[1];
			links_vectors[1] = tmp;
		}

		LinkIdVector_destroy(neighbours_n2);
		LinkIdVector_destroy(neighbours_n1);
	}

	NodeIdVector_destroy(nodes_set_1);
	NodeIdVector_destroy(nodes_set_2);
	return true;
}

String Stream_to_string(const Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	String str			= String_from_duplicate("Stream {\n\tLifespan: ");
	Interval lifespan	= fns.lifespan(stream->stream_data);
	String interval_str = Interval_to_string(&lifespan);
	String_concat_consume(&str, &interval_str);
	// TODO : rename push_str to append
	String_push_str(&str, "\n\tNodes:\n");
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		String_append_formatted(&str, "\t\t%zu, Times : { ", node_id);
		TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
		FOR_EACH_TIME(interval, times) {
			String_append_formatted(&str, "[%zu, %zu], ", interval.start, interval.end);
		}
		String_push_str(&str, "}\n");

		// Neighbours
		String_push_str(&str, "\t\tNeighbours: ");
		LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
		FOR_EACH_LINK(link_id, neighbours) {
			Link link		  = fns.link_by_id(stream->stream_data, link_id);
			NodeId other_node = Link_get_other_node(&link, node_id);
			String_append_formatted(&str, "%zu, ", other_node);
		}
		String_push_str(&str, "\n");
	}
	String_push_str(&str, "\n");

	String_push_str(&str, "\tLinks: \n");
	LinksIterator links = fns.links_set(stream->stream_data);
	FOR_EACH_LINK(link_id, links) {
		Link link = fns.link_by_id(stream->stream_data, link_id);
		String_append_formatted(&str, "\t\t%zu-%zu, Times : { ", link.nodes[0], link.nodes[1]);
		TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
		FOR_EACH_TIME(interval, times) {
			String_append_formatted(&str, "[%zu, %zu], ", interval.start, interval.end);
		}
		String_push_str(&str, "}\n");
	}
	String_push_str(&str, "\n");

	String_push_str(&str, "}\n");
	return str;
}

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
	size_t iteration = 0;
	while (true) {

		// KCoreDataVector_merge(&k_cores);
		// printf(TEXT_BOLD "BEGIN OF ITERATION\n");
		// for (size_t i = 0; i < nb_nodes; i++) {
		// 	printf("k_cores_datas[%zu] = %s\n", i, KCoreDataVector_to_string(&k_cores_datas[i]).data);
		// }
		// printf(TEXT_RESET);
		// printf("k_cores = %s\n" TEXT_RESET, KCoreDataVector_to_string(&k_cores).data);

		// Compute the presence intervals of each node
		size_t* nb_intervals = MALLOC(sizeof(size_t) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_intervals[i] = 0;
		}

		// for (size_t i = 0; i < k_cores.size; i++) {
		// 	nb_intervals[k_cores.array[i].node_id]++;
		// }
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_intervals[i] = k_cores_datas[i].size;
		}

		IntervalsSet* intervals = MALLOC(sizeof(IntervalsSet) * nb_nodes);
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

		free(nb_intervals);
		// free(nb_added);
		for (size_t i = 0; i < nb_nodes; i++) {
			IntervalsSet_destroy(intervals[i]);
		}
		free(intervals);

		// KCoreDataVector_sort(&newer_k_cores);
		// KCoreDataVector_merge(&newer_k_cores);

		for (size_t i = 0; i < nb_nodes; i++) {
			KCoreDataVector_merge(&newer_k_cores_datas[i]);
		}

		// // Stop if the k-cores are stable
		// if (KCoreDataVector_equals(&k_cores, &newer_k_cores)) {
		// 	KCoreDataVector_destroy(newer_k_cores);
		// 	break;
		// }

		// Stop if the k-cores are stable
		bool stable = true;
		for (size_t i = 0; i < nb_nodes; i++) {
			if (!KCoreDataVector_equals(&k_cores_datas[i], &newer_k_cores_datas[i])) {
				stable = false;
				// break;
			}
		}

		if (stable) {
			for (size_t i = 0; i < nb_nodes; i++) {
				KCoreDataVector_destroy(newer_k_cores_datas[i]);
			}
			free(newer_k_cores_datas);
			break;
		}

		for (size_t i = 0; i < nb_nodes; i++) {
			KCoreDataVector_destroy(k_cores_datas[i]);
		}
		free(k_cores_datas);

		k_cores_datas = newer_k_cores_datas;

		// KCoreDataVector_destroy(k_cores);
		// k_cores = newer_k_cores;

		printf(TEXT_BOLD "Iteration %zu\n" TEXT_RESET, iteration);
		// for (size_t i = 0; i < nb_nodes; i++) {
		// 	printf("Node %zu : %s\n", i, KCoreDataVector_to_string(&k_cores_datas[i]).data);
		// }
		iteration++;
	}

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