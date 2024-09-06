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
	NodeId node_id;
	Interval time;
	NodeIdVector neighbours;
} KCoreData;

String KCoreData_to_string(const KCoreData* data) {
	String str = String_new();
	String_append_formatted(&str, "Node %zu, Time [%zu, %zu], Neighbours : { ", data->node_id, data->time.start,
							data->time.end);
	for (size_t i = 0; i < data->neighbours.size; i++) {
		String_append_formatted(&str, "%zu, ", data->neighbours.array[i]);
	}
	String_push_str(&str, "}\n");
	return str;
}

bool KCoreData_equals(const KCoreData* a, const KCoreData* b) {
	return a->node_id == b->node_id && Interval_equals(&a->time, &b->time) &&
		   NodeIdVector_equals(&a->neighbours, &b->neighbours);
}

int KCoreData_compare(const void* a, const void* b) {
	KCoreData da = *(KCoreData*)a;
	KCoreData db = *(KCoreData*)b;

	if (da.node_id < db.node_id) {
		return -1;
	}
	if (da.node_id > db.node_id) {
		return 1;
	}

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

void KCores_add(KCoreDataVector* k_cores, NodeId og_node, Interval time, NodeId neigh) {
	// Find if the node is already in the vector
	for (size_t i = 0; i < k_cores->size; i++) {
		if (k_cores->array[i].node_id == og_node && Interval_equals(&k_cores->array[i].time, &time)) {
			printf("CASE 0\n");
			printf("adding = [%zu, %zu] with neigh = %zu\n", time.start, time.end, neigh);
			NodeIdVector_push(&k_cores->array[i].neighbours, neigh);
			return;
		}
	}

	// Find if there exists an interval that contains this one
	for (size_t i = 0; i < k_cores->size; i++) {
		if (k_cores->array[i].node_id == og_node && Interval_contains_interval(k_cores->array[i].time, time)) {
			printf("CASE 1\n");
			// split the interval into 3
			Interval one   = Interval_from(k_cores->array[i].time.start, time.start);
			Interval two   = Interval_from(time.start, time.end);
			Interval three = Interval_from(time.end, k_cores->array[i].time.end);

			NodeIdVector old_neigh_backup = k_cores->array[i].neighbours;
			NodeIdVector old_neigh		  = NodeIdVector_new();
			for (size_t i = 0; i < old_neigh_backup.size; i++) {
				NodeIdVector_push(&old_neigh, old_neigh_backup.array[i]);
			}

			// Remove the old interval
			KCoreDataVector_remove_and_swap(k_cores, i);

			// Add the new intervals
			if (!Interval_is_empty(one)) {
				KCoreData new_data = {og_node, one, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					KCores_add(k_cores, og_node, one, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				// KCores_add(k_cores, og_node, one, neigh);
			}
			if (!Interval_is_empty(two)) {
				KCoreData new_data = {og_node, two, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					KCores_add(k_cores, og_node, two, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				KCores_add(k_cores, og_node, two, neigh);
			}
			if (!Interval_is_empty(three)) {
				KCoreData new_data = {og_node, three, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					KCores_add(k_cores, og_node, three, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				// KCores_add(k_cores, og_node, three, neigh);
			}

			return;
		}
	}

	// Find if there exists an interval that is contained by this one
	for (size_t i = 0; i < k_cores->size; i++) {
		if (k_cores->array[i].node_id == og_node && Interval_contains_interval(time, k_cores->array[i].time)) {
			printf("CASE 2\n");
			printf("adding = [%zu, %zu] with neigh = %zu\n", time.start, time.end, neigh);
			// split the interval into 3
			Interval one   = Interval_from(time.start, k_cores->array[i].time.start);
			Interval two   = Interval_from(k_cores->array[i].time.start, k_cores->array[i].time.end);
			Interval three = Interval_from(k_cores->array[i].time.end, time.end);

			NodeIdVector old_neigh_backup = k_cores->array[i].neighbours;
			NodeIdVector old_neigh		  = NodeIdVector_new();
			for (size_t i = 0; i < old_neigh_backup.size; i++) {
				NodeIdVector_push(&old_neigh, old_neigh_backup.array[i]);
			}

			printf("to old [%zu, %zu] with neigh = ", k_cores->array[i].time.start, k_cores->array[i].time.end);
			for (size_t i = 0; i < old_neigh.size; i++) {
				printf("%zu, ", old_neigh.array[i]);
			}
			printf("\n");

			printf("one = [%zu, %zu], two = [%zu, %zu], three = [%zu, %zu]\n", one.start, one.end, two.start, two.end,
				   three.start, three.end);

			// Remove the old interval
			KCoreDataVector_remove_and_swap(k_cores, i);

			// Add the new intervals
			if (!Interval_is_empty(one)) {
				KCoreData new_data = {og_node, one, NodeIdVector_new()};
				// for (size_t i = 0; i < old_neigh.size; i++) {
				// 	NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
				// }

				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				KCores_add(k_cores, og_node, one, neigh);
			}
			if (!Interval_is_empty(two)) {
				KCoreData new_data = {og_node, two, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					KCores_add(k_cores, og_node, two, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				KCores_add(k_cores, og_node, two, neigh);
			}
			if (!Interval_is_empty(three)) {
				KCoreData new_data = {og_node, three, NodeIdVector_new()};
				// for (size_t i = 0; i < old_neigh.size; i++) {
				// 	NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
				// }
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				KCores_add(k_cores, og_node, three, neigh);
			}

			return;
		}
	}

	// Find if one interval overlaps the other
	for (size_t i = 0; i < k_cores->size; i++) {
		if (k_cores->array[i].node_id == og_node && Interval_overlaps_interval(k_cores->array[i].time, time)) {
			printf("CASE 3\n");
			printf("adding = [%zu, %zu] with neigh = %zu\n", time.start, time.end, neigh);
			printf("Overlapping with [%zu, %zu]\n", k_cores->array[i].time.start, k_cores->array[i].time.end);

			// split the interval into 3
			Interval inter		  = Interval_intersection(k_cores->array[i].time, time);
			Interval old_excluded = Interval_minus(k_cores->array[i].time, inter);
			Interval new_excluded = Interval_minus(time, inter);

			// if (old_excluded.start > new_excluded.start) {
			// 	printf("CASE 3.1\n");
			// 	Interval tmp = old_excluded;
			// 	old_excluded = new_excluded;
			// 	new_excluded = tmp;
			// }

			NodeIdVector old_neigh_backup = k_cores->array[i].neighbours;
			NodeIdVector old_neigh		  = NodeIdVector_new();
			for (size_t i = 0; i < old_neigh_backup.size; i++) {
				NodeIdVector_push(&old_neigh, old_neigh_backup.array[i]);
			}

			printf("to old [%zu, %zu] with neigh = ", k_cores->array[i].time.start, k_cores->array[i].time.end);
			for (size_t i = 0; i < old_neigh.size; i++) {
				printf("%zu, ", old_neigh.array[i]);
			}
			printf("\n");

			printf("one = [%zu, %zu], two = [%zu, %zu], three = [%zu, %zu]\n", old_excluded.start, old_excluded.end,
				   inter.start, inter.end, new_excluded.start, new_excluded.end);

			// Remove the old interval
			KCoreDataVector_remove_and_swap(k_cores, i);

			// Add the new intervals
			if (!Interval_is_empty(inter)) {
				KCoreData new_data = {og_node, inter, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					printf("Adding inter [%zu, %zu] with neigh = %zu\n", inter.start, inter.end, old_neigh.array[i]);
					KCores_add(k_cores, og_node, inter, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				printf("Adding inter [%zu, %zu] with neigh = %zu\n", inter.start, inter.end, neigh);
				KCores_add(k_cores, og_node, inter, neigh);
			}

			if (!Interval_is_empty(new_excluded)) {
				KCoreData new_data = {og_node, new_excluded, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					// printf("Adding new_excluded [%zu, %zu] with neigh = %zu\n", new_excluded.start, new_excluded.end,
					// 	   old_neigh.array[i]);
					// KCores_add(k_cores, og_node, new_excluded, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				printf("Adding new_excluded [%zu, %zu] with neigh = %zu\n", new_excluded.start, new_excluded.end,
					   neigh);
				KCores_add(k_cores, og_node, new_excluded, neigh);
			}

			if (!Interval_is_empty(old_excluded)) {
				KCoreData new_data = {og_node, old_excluded, NodeIdVector_new()};
				for (size_t i = 0; i < old_neigh.size; i++) {
					// NodeIdVector_push(&new_data.neighbours, old_neigh.array[i]);
					printf("Adding old_excluded [%zu, %zu] with neigh = %zu\n", old_excluded.start, old_excluded.end,
						   old_neigh.array[i]);
					KCores_add(k_cores, og_node, old_excluded, old_neigh.array[i]);
				}
				// NodeIdVector_push(&new_data.neighbours, neigh);
				// KCoreDataVector_push(k_cores, new_data);
				// printf("Adding old_excluded [%zu, %zu] with neigh = %zu\n", old_excluded.start, old_excluded.end,
				//    neigh);
				// KCores_add(k_cores, og_node, old_excluded, neigh);
			}

			return;
		}
	}

	// Otherwise, add the node
	printf("CASE 4\n");
	printf("adding = [%zu, %zu] with neigh = %zu\n", time.start, time.end, neigh);
	KCoreData new_data = {og_node, time, NodeIdVector_new()};
	NodeIdVector_push(&new_data.neighbours, neigh);
	KCoreDataVector_push(k_cores, new_data);
}
typedef struct {
	NodeId og_node;
	Interval time;
	NodeId neigh;
} KCoreToRemove;

String KCoreToRemove_to_string(const KCoreToRemove* data) {
	String str = String_new();
	String_append_formatted(&str, "Node %zu, Time [%zu, %zu], Neighbour %zu", data->og_node, data->time.start,
							data->time.end, data->neigh);
	return str;
}

DeclareVector(KCoreToRemove);
DefineVector(KCoreToRemove);
DefineVectorDeriveRemove(KCoreToRemove, NO_FREE(KCoreToRemove));
DefineVectorDeriveToString(KCoreToRemove);

/*void KCores_find_nodes_with_inferior_degree(KCoreDataVector* k_cores, size_t degree, const Stream* stream) {
	int ie = 0;
	while (true) {
		printf("Nodes with inferior degree\n");
		printf("Before remove : %s\n", KCoreDataVector_to_string(k_cores).data);

		KCoreDataVector new_k_cores = KCoreDataVector_new();
		for (size_t i = 0; i < k_cores->size; i++) {
			if (k_cores->array[i].neighbours.size >= degree) {
				for (size_t j = 0; j < k_cores->array[i].neighbours.size; j++) {
					// check if the neighbour still exists by the time

					KCoreData new_data = {k_cores->array[i].node_id, k_cores->array[i].time, NodeIdVector_new()};
					NodeIdVector_push(&new_data.neighbours, k_cores->array[i].neighbours.array[j]);
					KCores_add(&new_k_cores, new_data.node_id, new_data.time, new_data.neighbours.array[0]);
				}
			}
		}

		k_cores = &new_k_cores;

		StreamFunctions fns = STREAM_FUNCS(fns, stream);
		NodesIterator nodes = fns.nodes_set(stream->stream_data);
		size_t nb_nodes		= count_nodes(nodes);

		size_t* nb_intervals = MALLOC(sizeof(size_t) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_intervals[i] = 0;
		}

		for (size_t i = 0; i < k_cores->size; i++) {
			nb_intervals[k_cores->array[i].node_id]++;
		}

		printf("nb_intervals : \n");
		for (size_t i = 0; i < nb_nodes; i++) {
			printf("%zu : %zu,\n", i, nb_intervals[i]);
		}

		IntervalsSet* intervals = MALLOC(sizeof(IntervalsSet) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			intervals[i] = IntervalsSet_alloc(nb_intervals[i]);
		}

		int* nb_added = MALLOC(sizeof(int) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_added[i] = 0;
		}

		for (size_t i = 0; i < k_cores->size; i++) {
			IntervalsSet_add_at(&intervals[k_cores->array[i].node_id], k_cores->array[i].time,
								nb_added[k_cores->array[i].node_id]);
			nb_added[k_cores->array[i].node_id]++;
		}

		printf("After remove : %s\n", KCoreDataVector_to_string(&new_k_cores).data);

		KCoreDataVector newer_k_cores = KCoreDataVector_new();
		for (size_t i = 0; i < k_cores->size; i++) {
			if (k_cores->array[i].neighbours.size >= degree) {
				for (size_t j = 0; j < k_cores->array[i].neighbours.size; j++) {

					printf("i = %zu, j = %zu\n", i, j);
					printf("interval = %s\n", Interval_to_string(&k_cores->array[i].time).data);
					// check if the neighbour still exists by the time
					IntervalsSet e = IntervalsSet_alloc(1);
					IntervalsSet_add_at(&e, k_cores->array[i].time, 0);

					printf("e[%zu] = %s\n", e.nb_intervals, Interval_to_string(&e.intervals[0]).data);

					IntervalsSet inter = IntervalsSet_intersection(intervals[k_cores->array[i].neighbours.array[j]],
e); if (inter.nb_intervals == 0) { continue;
					}

					// KCoreData new_data = {k_cores->array[i].node_id, k_cores->array[i].time, NodeIdVector_new()};
					// NodeIdVector_push(&new_data.neighbours, k_cores->array[i].neighbours.array[j]);
					// KCores_add(&newer_k_cores, new_data.node_id, new_data.time, new_data.neighbours.array[0]);
					printf("old intervals : %s & %s\n",
						   Interval_to_string(&intervals[k_cores->array[i].neighbours.array[j]].intervals[0]).data,
						   Interval_to_string(&k_cores->array[i].time).data);
					for (size_t k = 0; k < inter.nb_intervals; k++) {
						printf("new interval : %s\n", Interval_to_string(&inter.intervals[k]).data);
						KCoreData new_data = {k_cores->array[i].node_id, inter.intervals[k], NodeIdVector_new()};
						NodeIdVector_push(&new_data.neighbours, k_cores->array[i].neighbours.array[j]);
						KCores_add(&newer_k_cores, new_data.node_id, new_data.time, new_data.neighbours.array[0]);
					}
				}
			}
		}
		printf("After EEE remove %d : %s\n", ie, KCoreDataVector_to_string(&newer_k_cores).data);

		if (KCoreDataVector_equals(k_cores, &newer_k_cores)) {
			break;
		}

		k_cores = &newer_k_cores;
		ie++;
	}
}

void Stream_k_cores(const Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	KCoreDataVector k_cores = KCoreDataVector_new();
	NodesIterator nodes		= fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		// Find the neighbours of the node
		NodeIdVector neighbours = NodeIdVector_new();
		LinksIterator links		= fns.neighbours_of_node(stream->stream_data, node_id);

		FOR_EACH_LINK(link_id, links) {
			Link link			= fns.link_by_id(stream->stream_data, link_id);
			NodeId other_node	= Link_get_other_node(&link, node_id);
			TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times) {
				String str = KCoreDataVector_to_string(&k_cores);
				printf("Before adding %zu->%zu [%zu, %zu[ : \n%s\n", node_id, other_node, interval.start,
interval.end, str.data); String_destroy(str); KCores_add(&k_cores, node_id, interval, other_node); str =
KCoreDataVector_to_string(&k_cores); printf("After adding %zu->%zu, [%zu, %zu[ : \n%s\n", node_id, other_node,
interval.start, interval.end, str.data); String_destroy(str);
			}
		}
	}

	KCoreDataVector_sort(&k_cores);
	String str = KCoreDataVector_to_string(&k_cores);
	printf("%s\n", str.data);
	String_destroy(str);

	printf("Nodes with inferior degree\n");

	KCores_find_nodes_with_inferior_degree(&k_cores, 2, stream);
	// str = KCoreDataVector_to_string(&k_cores);
	// printf("%s\n", str.data);
	// String_destroy(str);

	str = KCoreDataVector_to_string(&k_cores);
	printf("after remove : %s\n", str.data);
	String_destroy(str);
	DegreeInIntervalVector degree_evol_0 = Stream_evolution_of_node_degree(stream, 0);
	String str0							 = DegreeInIntervalVector_to_string(&degree_evol_0);
	printf("%s\n", str0.data);
	DegreeInIntervalVector degree_evol_1 = Stream_evolution_of_node_degree(stream, 1);
	String str1							 = DegreeInIntervalVector_to_string(&degree_evol_1);
	printf("%s\n", str1.data);
	DegreeInIntervalVector degree_evol_2 = Stream_evolution_of_node_degree(stream, 2);
	String str2							 = DegreeInIntervalVector_to_string(&degree_evol_2);
	printf("%s\n", str2.data);
	DegreeInIntervalVector degree_evol_3 = Stream_evolution_of_node_degree(stream, 3);
	String str3							 = DegreeInIntervalVector_to_string(&degree_evol_3);
	printf("%s\n", str3.data);
}*/

DefineVector(NodePresence);

void KCoreDataVector_merge(KCoreDataVector* vec) {
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
		vec->array[i].neighbours = new_neigh;
	}

	// see for each element in the vector that if the next one is the same except for the interval, merge them
	for (size_t i = 0; i < vec->size - 1; i++) {
		// printf("Element %zu : %s", i, KCoreData_to_string(&vec->array[i]).data);
		// printf("Element %zu : %s", i + 1, KCoreData_to_string(&vec->array[i + 1]).data);
		// bool merge = false;
		if (vec->array[i].node_id == vec->array[i + 1].node_id &&
			NodeIdVector_equals(&vec->array[i].neighbours, &vec->array[i + 1].neighbours)) {
			Interval i1 = vec->array[i].time;
			Interval i2 = vec->array[i + 1].time;
			if (i1.end == i2.start) {
				Interval new_interval = Interval_from(i1.start, i2.end);
				vec->array[i].time	  = new_interval;
				KCoreDataVector_remove(vec, i + 1);
				// merge = true;
				i--;
			}
		}
		// printf("merged? %d\n", merge);
	}

	KCoreDataVector_sort(vec);
}

/// Doesn't work with 0 but who cares about 0-core cause it's the same as the initial graph
KCore Stream_k_cores(const Stream* stream, size_t degree) {

	printf(TEXT_BOLD "Computing the %zu-cores of the stream\n" TEXT_RESET, degree);

	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	KCoreDataVector k_cores = KCoreDataVector_new();

	// Add all the stream data at the beginning (0-core)
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		// Find the neighbours of the node
		NodeIdVector neighbours = NodeIdVector_new();
		LinksIterator links		= fns.neighbours_of_node(stream->stream_data, node_id);

		FOR_EACH_LINK(link_id, links) {
			Link link			= fns.link_by_id(stream->stream_data, link_id);
			NodeId other_node	= Link_get_other_node(&link, node_id);
			TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
			FOR_EACH_TIME(interval, times) {
				// if (other_node == 3 && interval.start == 3 && interval.end == 10) {
				// 	printf("Before adding %zu->%zu [%zu, %zu[ : \n%s\n", node_id, other_node, interval.start,
				// 		   interval.end, KCoreDataVector_to_string(&k_cores).data);
				// }
				printf("Before adding %zu->%zu [%zu, %zu[ : \n%s\n", node_id, other_node, interval.start, interval.end,
					   KCoreDataVector_to_string(&k_cores).data);
				KCores_add(&k_cores, node_id, interval, other_node);
				// if (other_node == 3 && interval.start == 3 && interval.end == 10) {
				// 	printf("After adding %zu->%zu, [%zu, %zu[ : \n%s\n", node_id, other_node, interval.start,
				// 		   interval.end, KCoreDataVector_to_string(&k_cores).data);
				// }
				printf("After adding %zu->%zu, [%zu, %zu[ : \n%s\n", node_id, other_node, interval.start, interval.end,
					   KCoreDataVector_to_string(&k_cores).data);
			}
		}
	}

	size_t nb_nodes = count_nodes(fns.nodes_set(stream->stream_data));

	// Then, remove iteratively the nodes with degree < k until the k-cores are stable
	while (true) {

		KCoreDataVector_merge(&k_cores);
		printf(TEXT_BOLD "BEGIN OF ITERATION\n");
		printf("k_cores = %s\n" TEXT_RESET, KCoreDataVector_to_string(&k_cores).data);

		// Compute the presence intervals of each node
		size_t* nb_intervals = MALLOC(sizeof(size_t) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_intervals[i] = 0;
		}

		for (size_t i = 0; i < k_cores.size; i++) {
			nb_intervals[k_cores.array[i].node_id]++;
		}

		IntervalsSet* intervals = MALLOC(sizeof(IntervalsSet) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			intervals[i] = IntervalsSet_alloc(nb_intervals[i]);
		}

		int* nb_added = MALLOC(sizeof(int) * nb_nodes);
		for (size_t i = 0; i < nb_nodes; i++) {
			nb_added[i] = 0;
		}

		for (size_t i = 0; i < k_cores.size; i++) {
			IntervalsSet_add_at(&intervals[k_cores.array[i].node_id], k_cores.array[i].time,
								nb_added[k_cores.array[i].node_id]);
			nb_added[k_cores.array[i].node_id]++;
		}

		// for (size_t i = 0; i < k_cores.size; i++) {
		// 	IntervalsSet_sort(&intervals[k_cores.array[i].node_id]);
		// 	IntervalsSet_merge(&intervals[k_cores.array[i].node_id]);
		// }

		// print the presence intervals
		for (size_t i = 0; i < nb_nodes; i++) {
			printf("PRESENCE OF Node %zu : \n", i);
			for (size_t j = 0; j < intervals[i].nb_intervals; j++) {
				printf("\t[%zu, %zu[\n", intervals[i].intervals[j].start, intervals[i].intervals[j].end);
			}
		}

		KCoreDataVector newer_k_cores = KCoreDataVector_new();
		for (size_t i = 0; i < k_cores.size; i++) {
			if (k_cores.array[i].neighbours.size >= degree) {
				// printf("Node %zu has degree %zu in [%zu, %zu[\n", k_cores.array[i].node_id,
				//    k_cores.array[i].neighbours.size, k_cores.array[i].time.start, k_cores.array[i].time.end);
				for (size_t j = 0; j < k_cores.array[i].neighbours.size; j++) {

					// printf("i = %zu, j = %zu\n", i, j);

					// See if a node that's a neighbour of the current node has been removed
					IntervalsSet current = IntervalsSet_alloc(1);
					IntervalsSet_add_at(&current, k_cores.array[i].time, 0);
					// IntervalsSet_sort(&intervals[k_cores.array[i].neighbours.array[j]]);
					IntervalsSet inter =
						IntervalsSet_intersection(intervals[k_cores.array[i].neighbours.array[j]], current);

					// printf("current = %s\n", Interval_to_string(&current.intervals[0]).data);
					// printf("other = ");
					// for (size_t k = 0; k < intervals[k_cores.array[i].neighbours.array[j]].nb_intervals; k++)
					// { 	printf("[%zu, %zu[ ",
					// intervals[k_cores.array[i].neighbours.array[j]].intervals[k].start,
					// 		   intervals[k_cores.array[i].neighbours.array[j]].intervals[k].end);
					// }
					// printf("\n");
					// printf("inter = ");
					// for (size_t k = 0; k < inter.nb_intervals; k++) {
					// 	printf("[%zu, %zu[ ", inter.intervals[k].start, inter.intervals[k].end);
					// }
					// printf("\n");
					// printf("inter.nb_intervals = %zu\n", inter.nb_intervals);

					for (size_t k = 0; k < inter.nb_intervals; k++) {
						KCoreData new_data = {k_cores.array[i].node_id, inter.intervals[k], NodeIdVector_new()};
						NodeIdVector_push(&new_data.neighbours, k_cores.array[i].neighbours.array[j]);

						// if (new_data.node_id == 1 && new_data.time.start == 3 && new_data.time.end == 10) {
						if (new_data.node_id == 1) {
							// printf("Before adding %s : %s\n", KCoreData_to_string(&new_data).data,
							//    KCoreDataVector_to_string(&newer_k_cores).data);
							KCores_add(&newer_k_cores, new_data.node_id, new_data.time, new_data.neighbours.array[0]);
							// printf("After adding %s : %s\n", KCoreData_to_string(&new_data).data,
							//    KCoreDataVector_to_string(&newer_k_cores).data);
						}
						else {
							KCores_add(&newer_k_cores, new_data.node_id, new_data.time, new_data.neighbours.array[0]);
						}
						// }
						// else {
						// KCores_add(&newer_k_cores, new_data.node_id, new_data.time,
						// new_data.neighbours.array[0]);
						// }
					}

					// NodeId target_node = k_cores.array[i].neighbours.array[j];
					// IntervalsSet inter =
					// 	IntervalsSet_intersection(intervals[target_node], intervals[k_cores.array[i].node_id]);
					// for (size_t k = 0; k < inter.nb_intervals; k++) {
					// 	KCoreData new_data = {k_cores.array[i].node_id, inter.intervals[k], NodeIdVector_new()};
					// 	NodeIdVector_push(&new_data.neighbours, target_node);
					// 	KCoreDataVector_push(&newer_k_cores, new_data);
					// }
				}
			}
		}

		KCoreDataVector_sort(&newer_k_cores);
		KCoreDataVector_merge(&newer_k_cores);

		// Stop if the k-cores are stable
		if (KCoreDataVector_equals(&k_cores, &newer_k_cores)) {
			break;
		}

		k_cores = newer_k_cores;
		// KCoreDataVector_sort(&k_cores);
		// KCoreDataVector_merge(&k_cores);
		printf(TEXT_BOLD "END OF ITERATION\n");
		printf("k_cores = %s\n" TEXT_RESET, KCoreDataVector_to_string(&k_cores).data);
	}

	// print
	String str1 = KCoreDataVector_to_string(&k_cores);
	printf("%s\n", str1.data);

	// Then, extract the k-core

	// Edge case : empty k-core
	if (k_cores.size == 0) {
		KCore kcore = {
			.nodes = NodePresenceVector_new(),
		};
		return kcore;
	}

	NodeId cur_node = SIZE_MAX;
	KCore kcore		= {
			.nodes = NodePresenceVector_new(),
	};
	size_t cur_idx_to_push = 0;

	for (size_t i = 0; i < k_cores.size; i++) {
		if (k_cores.array[i].node_id != cur_node) {
			NodePresence n = {
				.node_id  = k_cores.array[i].node_id,
				.presence = IntervalVector_new(),
			};
			IntervalVector_push(&n.presence, k_cores.array[i].time);
			NodePresenceVector_push(&kcore.nodes, n);
			cur_node = k_cores.array[i].node_id;
			cur_idx_to_push++;
		}
		else {
			IntervalVector_push(&kcore.nodes.array[cur_idx_to_push - 1].presence, k_cores.array[i].time);
		}
	}

	// String str = KCore_to_string(&kcore);
	// printf("%s\n", str.data);
	// String_destroy(str);

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