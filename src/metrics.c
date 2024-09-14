#include "metrics.h"
#include "generic_data_structures/vector.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream_functions.h"
#include "stream_graph/links_set.h"
#include "stream_graph/nodes_set.h"
#include "stream_wrappers.h"
#include "units.h"
#include "utils.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define CATCH_METRICS_IMPLEM(function, stream)                                                                                             \
	switch (stream->type) {                                                                                                                \
		case FULL_STREAM_GRAPH: {                                                                                                          \
			if (FullStreamGraph_metrics_functions.function != NULL) {                                                                      \
				return FullStreamGraph_metrics_functions.function(stream);                                                                 \
			}                                                                                                                              \
			break;                                                                                                                         \
		}                                                                                                                                  \
		case LINK_STREAM: {                                                                                                                \
			if (LinkStream_metrics_functions.function != NULL) {                                                                           \
				return LinkStream_metrics_functions.function(stream);                                                                      \
			}                                                                                                                              \
			break;                                                                                                                         \
		}                                                                                                                                  \
		case CHUNK_STREAM: {                                                                                                               \
			if (ChunkStream_metrics_functions.function != NULL) {                                                                          \
				return ChunkStream_metrics_functions.function(stream);                                                                     \
			}                                                                                                                              \
			break;                                                                                                                         \
		}                                                                                                                                  \
		case CHUNK_STREAM_SMALL: {                                                                                                         \
			if (ChunkStreamSmall_metrics_functions.function != NULL) {                                                                     \
				return ChunkStreamSmall_metrics_functions.function(stream);                                                                \
			}                                                                                                                              \
			break;                                                                                                                         \
		}                                                                                                                                  \
		case SNAPSHOT_STREAM: {                                                                                                            \
			if (SnapshotStream_metrics_functions.function != NULL) {                                                                       \
				return SnapshotStream_metrics_functions.function(stream);                                                                  \
			}                                                                                                                              \
			break;                                                                                                                         \
		}                                                                                                                                  \
	}

// TODO : rename the functions to be more explicit
// TODO : rewrite them to be cleaner
// TODO: move them somewhere else
size_t cardinalOfT(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfT);
	CATCH_METRICS_IMPLEM(cardinalOfT, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	Interval lifespan	= fns.lifespan(stream->stream_data);
	UPDATE_CACHE(stream, cardinalOfT, Interval_duration(lifespan));
	return Interval_duration(lifespan);
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
	// printf("et = %zu, vt = %zu\n", et, vt);
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
	size_t sum_den = Interval_duration(fns.lifespan(stream->stream_data));
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

			LinkId uw = fns.link_between_nodes(stream->stream_data, u, w);
			if (uw == SIZE_MAX) {
				continue;
			}

			IntervalVector times_uv		  = SGA_collect_times(fns.times_link_present(stream->stream_data, uv));
			IntervalVector times_vw		  = SGA_collect_times(fns.times_link_present(stream->stream_data, vw));
			IntervalVector times_uv_vw	  = IntervalVector_intersection(&times_uv, &times_vw);
			IntervalVector times_uw		  = SGA_collect_times(fns.times_link_present(stream->stream_data, uw));
			IntervalVector times_uw_uv_vw = IntervalVector_intersection(&times_uw, &times_uv_vw);

			size_t t_uv_vw = 0;
			for (size_t i = 0; i < times_uv_vw.size; i++) {
				t_uv_vw += Interval_duration(times_uv_vw.array[i]);
			}

			size_t t_uw_uv_vw = 0;
			for (size_t i = 0; i < times_uw_uv_vw.size; i++) {
				t_uw_uv_vw += Interval_duration(times_uw_uv_vw.array[i]);
			}

			sum_den += t_uv_vw;
			sum_num += t_uw_uv_vw;

			IntervalVector_destroy(times_uv);
			IntervalVector_destroy(times_vw);
			IntervalVector_destroy(times_uv_vw);
			IntervalVector_destroy(times_uw);
			IntervalVector_destroy(times_uw_uv_vw);
		}
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_node_clustering_coeff(Stream* stream) {
	// CATCH_METRICS_IMPLEM(node_clustering_coeff, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	double sum			= 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	double w			= (double)cardinalOfW(stream);
	FOR_EACH_NODE(node_id, nodes) {
		double clustering_coeff = Stream_clustering_coeff_of_node(stream, node_id);
		size_t t_v				= total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += clustering_coeff * ((double)t_v / w);
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

				LinkId uw = fns.link_between_nodes(stream->stream_data, u, w);

				// TimesIterator times_uv		= fns.times_link_present(stream->stream_data, uv);
				// TimesIterator times_vw		= fns.times_link_present(stream->stream_data, vw);
				// TimesIterator times_uv_N_vw = TimesIterator_intersection(times_uv, times_vw);

				// size_t time_of_triplet = total_time_of(times_uv_N_vw);
				// sum_den += time_of_triplet;

				// if (uw != SIZE_MAX) { // Triangle
				// 	TimesIterator times_uv			 = fns.times_link_present(stream->stream_data, uv);
				// 	TimesIterator times_uw			 = fns.times_link_present(stream->stream_data, uw);
				// 	TimesIterator times_vw			 = fns.times_link_present(stream->stream_data, vw);
				// 	TimesIterator times_uv_N_uw		 = TimesIterator_intersection(times_uv, times_uw);
				// 	TimesIterator times_uv_N_uw_N_vw = TimesIterator_intersection(times_uv_N_uw, times_vw);

				// 	size_t time_of_triangle = total_time_of(times_uv_N_uw_N_vw);
				// 	sum_num += time_of_triangle;
				// }

				IntervalVector times_uv		 = SGA_collect_times(fns.times_link_present(stream->stream_data, uv));
				IntervalVector times_vw		 = SGA_collect_times(fns.times_link_present(stream->stream_data, vw));
				IntervalVector times_uv_N_vw = IntervalVector_intersection(&times_uv, &times_vw);

				size_t time_of_triplet = 0;
				for (size_t i = 0; i < times_uv_N_vw.size; i++) {
					time_of_triplet += Interval_duration(times_uv_N_vw.array[i]);
				}
				sum_den += time_of_triplet;

				if (uw != SIZE_MAX) { // Triangle
					IntervalVector times_uw			  = SGA_collect_times(fns.times_link_present(stream->stream_data, uw));
					IntervalVector times_uv_N_uw	  = IntervalVector_intersection(&times_uv, &times_uw);
					IntervalVector times_uv_N_uw_N_vw = IntervalVector_intersection(&times_uv_N_uw, &times_vw);

					size_t time_of_triangle = 0;
					for (size_t i = 0; i < times_uv_N_uw_N_vw.size; i++) {
						time_of_triangle += Interval_duration(times_uv_N_uw_N_vw.array[i]);
					}

					IntervalVector_destroy(times_uw);
					IntervalVector_destroy(times_uv_N_uw);
					IntervalVector_destroy(times_uv_N_uw_N_vw);
				}

				IntervalVector_destroy(times_uv);
				IntervalVector_destroy(times_vw);
				IntervalVector_destroy(times_uv_N_vw);
			}
		}
	}

	return (double)sum_num / (double)sum_den;
}

// TODO: move out of here
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