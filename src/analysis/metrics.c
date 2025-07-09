/**
 * @file src/analysis/metrics.c
 * @brief Functions to compute metrics on a Stream.
 */

#define SGA_INTERNAL

#include "metrics.h"
#include "../interval.h"
#include "../iterators.h"
#include "../stream.h"
#include "../stream_functions.h"
#include "../stream_graph/links_set.h"
#include "../streams.h"
#include "../units.h"
#include "../utils.h"
#include <assert.h>
#include <stddef.h>

/**
 * @brief Catch a more specialised implementation of a metric function.
 * @param function The name of the function to catch.
 * @param stream The Stream to catch the function for.
 *
 * Some metrics can be computed more efficiently if the Stream is of a specific type.
 * This function catches the more specialised implementation of a metric function if it exists.
 * If it does not exist, the default stream-generic implementation will be used.
 */
#define CATCH_METRICS_IMPLEM(function, stream)                                                                                             \
	switch (stream->type) {                                                                                                            \
		case FULL_STREAM_GRAPH: {                                                                                                  \
			if (FullStreamGraph_metrics_functions.function != NULL) {                                                          \
				return FullStreamGraph_metrics_functions.function(stream);                                                 \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
		case LINK_STREAM: {                                                                                                        \
			if (LinkStream_metrics_functions.function != NULL) {                                                               \
				return LinkStream_metrics_functions.function(stream);                                                      \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
		case CHUNK_STREAM: {                                                                                                       \
			if (ChunkStream_metrics_functions.function != NULL) {                                                              \
				return ChunkStream_metrics_functions.function(stream);                                                     \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
		case CHUNK_STREAM_SMALL: {                                                                                                 \
			if (ChunkStreamSmall_metrics_functions.function != NULL) {                                                         \
				return ChunkStreamSmall_metrics_functions.function(stream);                                                \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
		case TIMEFRAME_STREAM: {                                                                                                   \
			if (TimeFrameStream_metrics_functions.function != NULL) {                                                          \
				return TimeFrameStream_metrics_functions.function(stream);                                                 \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
		case DELTA_STREAM: {                                                                                                       \
			if (DeltaStream_metrics_functions.function != NULL) {                                                              \
				return DeltaStream_metrics_functions.function(stream);                                                     \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
	}

// TODO : rename the functions to be more explicit
// TODO : rewrite them to be cleaner
// TODO: move them somewhere else
size_t SGA_Stream_duration(const SGA_Stream* stream) {
	FETCH_CACHE(stream, duration);
	CATCH_METRICS_IMPLEM(duration, stream);
	StreamFunctions fns   = STREAM_FUNCS(fns, stream);
	SGA_Interval lifespan = fns.lifespan(stream->stream_data);
	UPDATE_CACHE(stream, duration, SGA_Interval_duration(lifespan));
	return SGA_Interval_duration(lifespan);
}

size_t SGA_Stream_distinct_cardinal_of_node_set(const SGA_Stream* stream) {
	FETCH_CACHE(stream, distinct_cardinal_of_node_set);
	CATCH_METRICS_IMPLEM(distinct_cardinal_of_node_set, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	size_t count		= COUNT_ITERATOR(nodes);
	UPDATE_CACHE(stream, distinct_cardinal_of_node_set, count);
	return count;
}

size_t SGA_Stream_distinct_cardinal_of_link_set(const SGA_Stream* stream) {
	FETCH_CACHE(stream, distinct_cardinal_of_link_set);
	CATCH_METRICS_IMPLEM(distinct_cardinal_of_link_set, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_LinksIterator links = fns.links_set(stream->stream_data);
	size_t count		= COUNT_ITERATOR(links);
	UPDATE_CACHE(stream, distinct_cardinal_of_link_set, count);
	return count;
}

size_t SGA_Stream_temporal_cardinal_of_link_set(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(temporal_cardinal_of_link_set, stream);
	FETCH_CACHE(stream, temporal_cardinal_of_link_set);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_LinksIterator links = fns.links_set(stream->stream_data);
	size_t count		= 0;
	SGA_FOR_EACH_LINK(link_id, links) {
		SGA_TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
		count += SGA_total_time_of(times);
	}
	UPDATE_CACHE(stream, temporal_cardinal_of_link_set, count);
	return count;
}

size_t SGA_Stream_temporal_cardinal_of_node_set(const SGA_Stream* stream) {
	FETCH_CACHE(stream, temporal_cardinal_of_node_set);
	CATCH_METRICS_IMPLEM(temporal_cardinal_of_node_set, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	size_t count		= 0;
	SGA_FOR_EACH_NODE(node_id, nodes) {
		SGA_TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
		count += SGA_total_time_of(times);
	}
	UPDATE_CACHE(stream, temporal_cardinal_of_node_set, count);
	return count;
}

double SGA_Stream_coverage(const SGA_Stream* stream) {
	CATCH_METRICS_IMPLEM(coverage, stream);
	size_t w = SGA_Stream_temporal_cardinal_of_node_set(stream);
	size_t t = SGA_Stream_duration(stream);
	size_t v = SGA_Stream_distinct_cardinal_of_node_set(stream);

	return (double)w / (double)(t * v);
}

double SGA_Stream_node_duration(const SGA_Stream* stream) {
	CATCH_METRICS_IMPLEM(node_duration, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t w	    = SGA_Stream_temporal_cardinal_of_node_set(stream);
	size_t v	    = SGA_Stream_distinct_cardinal_of_node_set(stream);
	size_t time_scale   = fns.time_scale(stream->stream_data);
	return (double)w / (double)(v * time_scale);
}

double SGA_Stream_contribution_of_node(const SGA_Stream* stream, SGA_NodeId node_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_node, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
	size_t t_v		= SGA_total_time_of(times);
	size_t t		= SGA_Stream_duration(stream);
	return (double)t_v / (double)t;
}

double SGA_Stream_contribution_of_link(const SGA_Stream* stream, SGA_LinkId link_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_link, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
	size_t t_v		= SGA_total_time_of(times);
	size_t t		= SGA_Stream_duration(stream);
	return (double)t_v / (double)(t);
}

double SGA_Stream_number_of_nodes(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(number_of_nodes, stream);
	size_t w = SGA_Stream_temporal_cardinal_of_node_set(stream);
	size_t t = SGA_Stream_duration(stream);
	return (double)w / (double)t;
}

double SGA_Stream_number_of_links(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(number_of_links, stream);
	size_t e = SGA_Stream_temporal_cardinal_of_link_set(stream);
	size_t t = SGA_Stream_duration(stream);
	return (double)e / (double)t;
}

double SGA_Stream_node_contribution_at_instant(const SGA_Stream* stream, SGA_Time instant) {
	// CATCH_METRICS_IMPLEM(node_contribution_at_time, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_NodesIterator nodes = fns.nodes_present_at_t(stream->stream_data, instant);
	size_t v_t		= COUNT_ITERATOR(nodes);
	size_t time_scale	= fns.time_scale(stream->stream_data);
	size_t v		= SGA_Stream_distinct_cardinal_of_node_set(stream);
	return (double)v_t / (double)(v * time_scale);
}

size_t size_set_unordered_pairs_itself(size_t n) {
	return n * (n - 1) / 2;
}

double SGA_Stream_link_contribution_at_instant(const SGA_Stream* stream, SGA_Time instant) {
	// CATCH_METRICS_IMPLEM(link_contribution_at_time, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	SGA_LinksIterator links = fns.links_present_at_t(stream->stream_data, instant);
	size_t e_t		= COUNT_ITERATOR(links);
	size_t time_scale	= fns.time_scale(stream->stream_data);
	size_t v		= SGA_Stream_distinct_cardinal_of_node_set(stream);
	size_t vxv		= size_set_unordered_pairs_itself(v);
	return (double)e_t / (double)(vxv * time_scale);
}

double SGA_Stream_link_duration(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(link_duration, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t e	    = SGA_Stream_temporal_cardinal_of_link_set(stream);
	size_t v	    = SGA_Stream_distinct_cardinal_of_node_set(stream);
	size_t time_scale   = fns.time_scale(stream->stream_data);
	size_t vxv	    = size_set_unordered_pairs_itself(v);
	return (double)e / (double)(vxv * time_scale);
}

double SGA_Stream_uniformity(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(uniformity, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	size_t sum_num		= 0;
	size_t sum_den		= 0;
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		SGA_NodesIterator other_pair = fns.nodes_set(stream->stream_data);
		SGA_FOR_EACH_NODE(other_node_id, other_pair) {
			if (node_id >= other_node_id) {
				continue;
			}
			SGA_TimesIterator times_node	   = fns.times_node_present(stream->stream_data, node_id);
			SGA_TimesIterator times_other_node = fns.times_node_present(stream->stream_data, other_node_id);
			SGA_TimesIterator times_union	   = SGA_TimesIterator_union(times_node, times_other_node);

			size_t t_u			     = SGA_total_time_of(times_union);
			times_node			     = fns.times_node_present(stream->stream_data, node_id);
			times_other_node		     = fns.times_node_present(stream->stream_data, other_node_id);
			SGA_TimesIterator times_intersection = SGA_TimesIterator_intersection(times_node, times_other_node);
			size_t t_i			     = SGA_total_time_of(times_intersection);

			sum_num += t_i;
			sum_den += t_u;
		}
	}
	return (double)sum_num / (double)sum_den;
}

double SGA_Stream_uniformity_pair_nodes(const SGA_Stream* stream, SGA_NodeId node1, SGA_NodeId node2) {
	// CATCH_METRICS_IMPLEM(uniformity_pair_nodes, stream);
	StreamFunctions fns	      = STREAM_FUNCS(fns, stream);
	SGA_TimesIterator times_node1 = fns.times_node_present(stream->stream_data, node1);
	SGA_TimesIterator times_node2 = fns.times_node_present(stream->stream_data, node2);
	SGA_TimesIterator times_union = SGA_TimesIterator_union(times_node1, times_node2);

	size_t t_u			     = SGA_total_time_of(times_union);
	times_node1			     = fns.times_node_present(stream->stream_data, node1);
	times_node2			     = fns.times_node_present(stream->stream_data, node2);
	SGA_TimesIterator times_intersection = SGA_TimesIterator_intersection(times_node1, times_node2);
	size_t t_i			     = SGA_total_time_of(times_intersection);

	return (double)t_i / (double)t_u;
}

double SGA_Stream_compactness(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(compactness, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	// Compute T' (the set of times where at least one node is present)
	// Compute V' (the set of nodes present at least once)
	size_t nb_nodes_exist		 = 0;
	SGA_TimeId first_node_apparition = SIZE_MAX;
	SGA_TimeId last_node_apparition	 = 0;

	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		SGA_TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
		size_t nb_times		= 0;
		SGA_FOR_EACH_TIME(interval, times) {
			nb_times++;
			if (interval.start < first_node_apparition) {
				first_node_apparition = interval.start;
			}
			if (interval.end > last_node_apparition) {
				last_node_apparition = interval.end;
			}
		}
		if (nb_times > 0) {
			nb_nodes_exist++;
		}
	}

	// |W| / (|T'| * |V'|)
	return (double)SGA_Stream_temporal_cardinal_of_node_set(stream) /
	       (double)((last_node_apparition - first_node_apparition) * nb_nodes_exist);
}

double SGA_Stream_density(const SGA_Stream* stream) {
	CATCH_METRICS_IMPLEM(density, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	size_t sum_den		= 0;
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		SGA_NodesIterator other_nodes = fns.nodes_set(stream->stream_data);
		SGA_FOR_EACH_NODE(other_node_id, other_nodes) {
			if (node_id >= other_node_id) {
				continue;
			}
			SGA_TimesIterator times_node	     = fns.times_node_present(stream->stream_data, node_id);
			SGA_TimesIterator times_other_node   = fns.times_node_present(stream->stream_data, other_node_id);
			SGA_TimesIterator times_intersection = SGA_TimesIterator_intersection(times_node, times_other_node);
			sum_den += SGA_total_time_of(times_intersection);
		}
	}

	size_t sum_num		= 0;
	SGA_LinksIterator links = fns.links_set(stream->stream_data);
	SGA_FOR_EACH_LINK(link_id, links) {
		SGA_TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += SGA_total_time_of(times_link);
	}

	return (double)sum_num / (double)sum_den;
}

double SGA_Stream_density_of_link(const SGA_Stream* stream, SGA_LinkId link_id) {
	StreamFunctions fns	     = STREAM_FUNCS(fns, stream);
	SGA_TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
	size_t sum_num		     = SGA_total_time_of(times_link);
	SGA_Link l		     = fns.link_by_id(stream->stream_data, link_id);
	SGA_TimesIterator t_u	     = fns.times_node_present(stream->stream_data, l.nodes[0]);
	SGA_TimesIterator t_v	     = fns.times_node_present(stream->stream_data, l.nodes[1]);
	SGA_TimesIterator t_i	     = SGA_TimesIterator_intersection(t_u, t_v);
	size_t sum_den		     = SGA_total_time_of(t_i);

	return (double)sum_num / (double)sum_den;
}

double SGA_Stream_density_of_node(const SGA_Stream* stream, SGA_NodeId node_id) {
	// CATCH_METRICS_IMPLEM(density_of_node, stream);
	StreamFunctions fns	     = STREAM_FUNCS(fns, stream);
	SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	size_t sum_num		     = 0;
	size_t sum_den		     = 0;
	SGA_FOR_EACH_LINK(link_id, neighbours) {
		SGA_TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += SGA_total_time_of(times_link);
	}

	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(other_node_id, nodes) {
		if (node_id == other_node_id) {
			continue;
		}
		SGA_TimesIterator times_node	     = fns.times_node_present(stream->stream_data, node_id);
		SGA_TimesIterator times_other_node   = fns.times_node_present(stream->stream_data, other_node_id);
		SGA_TimesIterator times_intersection = SGA_TimesIterator_intersection(times_node, times_other_node);
		sum_den += SGA_total_time_of(times_intersection);
	}
	// printf("sum_num = %zu, sum_den = %zu\n", sum_num, sum_den);
	return (double)sum_num / (double)sum_den;
}

double SGA_Stream_density_at_instant(const SGA_Stream* stream, SGA_TimeId time_id) {
	// CATCH_METRICS_IMPLEM(density_at_instant, stream);
	StreamFunctions fns	     = STREAM_FUNCS(fns, stream);
	SGA_LinksIterator links_at_t = fns.links_present_at_t(stream->stream_data, time_id);
	// printf("got links present at t\n");
	size_t et = 0;
	SGA_FOR_EACH_LINK(link_id, links_at_t) {
		// printf("link_id = %zu\n", link_id);
		et++;
	}
	SGA_NodesIterator nodes_at_t = fns.nodes_present_at_t(stream->stream_data, time_id);
	// printf("got nodes present at t\n");
	size_t vt = 0;
	SGA_FOR_EACH_NODE(node_id, nodes_at_t) {
		// printf("node_id = %zu\n", node_id);
		vt++;
	}
	// size_t et = COUNT_ITERATOR(links_at_t);
	// size_t et = COUNT_ITERATOR(links_at_t);
	// size_t vt = COUNT_ITERATOR(nodes_at_t);
	// printf("et = %zu, vt = %zu\n", et, vt);
	// printf("time_id = %zu, et = %zu, vt = %zu\n", time_id, et, vt);
	return (double)et / (double)(size_set_unordered_pairs_itself(vt));
}

double SGA_Stream_degree_of_node(const SGA_Stream* stream, SGA_NodeId node_id) {
	// CATCH_METRICS_IMPLEM(degree_of_node, stream);
	StreamFunctions fns	     = STREAM_FUNCS(fns, stream);
	SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	size_t sum_num		     = 0;
	SGA_FOR_EACH_LINK(link_id, neighbours) {
		SGA_TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += SGA_total_time_of(times_link);
	}
	size_t sum_den = SGA_Interval_duration(fns.lifespan(stream->stream_data));
	return (double)sum_num / (double)sum_den;
}

double SGA_Stream_average_node_degree(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(average_node_degree, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	double sum		= 0;
	double w		= (double)SGA_Stream_temporal_cardinal_of_node_set(stream);
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		double degree = SGA_Stream_degree_of_node(stream, node_id);
		size_t t_v    = SGA_total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += degree * ((double)t_v / w);
	}
	return sum;
}

double SGA_Stream_degree(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = SGA_Stream_temporal_cardinal_of_link_set(stream);
	size_t t	       = SGA_Stream_duration(stream);
	size_t v	       = SGA_Stream_distinct_cardinal_of_node_set(stream);
	return (double)(2 * number_of_links) / (double)(t * v);
}

double SGA_Stream_average_expected_degree(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = SGA_Stream_temporal_cardinal_of_link_set(stream);
	size_t number_of_nodes = SGA_Stream_temporal_cardinal_of_node_set(stream);
	return (double)(2 * number_of_links) / (double)number_of_nodes;
}

double SGA_Stream_clustering_coeff_of_node(const SGA_Stream* stream, SGA_NodeId node_id) {
	SGA_NodeId v	    = node_id;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	SGA_LinksIterator neighbours_of_v = fns.neighbours_of_node(stream->stream_data, v);
	size_t sum_den			  = 0;
	size_t sum_num			  = 0;

	SGA_FOR_EACH_LINK(uv, neighbours_of_v) {
		SGA_LinksIterator neighbours_of_v_copy = fns.neighbours_of_node(stream->stream_data, v);
		SGA_FOR_EACH_LINK(vw, neighbours_of_v_copy) {
			SGA_Link vw_link = fns.link_by_id(stream->stream_data, vw);
			SGA_NodeId w	 = SGA_Link_get_other_node(&vw_link, v);
			SGA_Link uv_link = fns.link_by_id(stream->stream_data, uv);
			SGA_NodeId u	 = SGA_Link_get_other_node(&uv_link, v);

			if (u >= w) {
				continue;
			}

			SGA_LinkId uw = fns.link_between_nodes(stream->stream_data, u, w);
			if (uw == SIZE_MAX) {
				continue;
			}

			SGA_IntervalArrayList times_uv	     = SGA_collect_times(fns.times_link_present(stream->stream_data, uv));
			SGA_IntervalArrayList times_vw	     = SGA_collect_times(fns.times_link_present(stream->stream_data, vw));
			SGA_IntervalArrayList times_uv_vw    = SGA_IntervalArrayList_intersection(&times_uv, &times_vw);
			SGA_IntervalArrayList times_uw	     = SGA_collect_times(fns.times_link_present(stream->stream_data, uw));
			SGA_IntervalArrayList times_uw_uv_vw = SGA_IntervalArrayList_intersection(&times_uw, &times_uv_vw);

			size_t t_uv_vw = 0;
			for (size_t i = 0; i < times_uv_vw.length; i++) {
				t_uv_vw += SGA_Interval_duration(times_uv_vw.array[i]);
			}

			size_t t_uw_uv_vw = 0;
			for (size_t i = 0; i < times_uw_uv_vw.length; i++) {
				t_uw_uv_vw += SGA_Interval_duration(times_uw_uv_vw.array[i]);
			}

			sum_den += t_uv_vw;
			sum_num += t_uw_uv_vw;

			SGA_IntervalArrayList_destroy(times_uv);
			SGA_IntervalArrayList_destroy(times_vw);
			SGA_IntervalArrayList_destroy(times_uv_vw);
			SGA_IntervalArrayList_destroy(times_uw);
			SGA_IntervalArrayList_destroy(times_uw_uv_vw);
		}
	}
	// Clustering coefficient is 0 if there are no triangles
	if (sum_den == 0) {
		return 0.0;
	}

	return (double)sum_num / (double)sum_den;
}

double SGA_Stream_node_clustering_coeff(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(node_clustering_coeff, stream);
	StreamFunctions fns	= STREAM_FUNCS(fns, stream);
	double sum		= 0;
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	double w		= (double)SGA_Stream_temporal_cardinal_of_node_set(stream);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		double clustering_coeff = SGA_Stream_clustering_coeff_of_node(stream, node_id);
		size_t t_v		= SGA_total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += clustering_coeff * ((double)t_v / w);
	}
	return sum;
}

double SGA_Stream_transitivity_ratio(const SGA_Stream* stream) {
	// CATCH_METRICS_IMPLEM(transitivity_ratio, stream);

	// Compute the number of triangles
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_num	    = 0;
	size_t sum_den	    = 0;

	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(u, nodes) {
		SGA_LinksIterator n_u = fns.neighbours_of_node(stream->stream_data, u);
		SGA_FOR_EACH_LINK(uv, n_u) {
			SGA_Link link_uv      = fns.link_by_id(stream->stream_data, uv);
			SGA_NodeId v	      = SGA_Link_get_other_node(&link_uv, u);
			SGA_LinksIterator n_v = fns.neighbours_of_node(stream->stream_data, v);
			SGA_FOR_EACH_LINK(vw, n_v) {
				SGA_Link link_vw = fns.link_by_id(stream->stream_data, vw);
				SGA_NodeId w	 = SGA_Link_get_other_node(&link_vw, v);

				// If (u,v,w) is present, so is (w,v,u), and therefore half can be skipped.
				if (w >= u) {
					continue;
				}

				SGA_LinkId uw = fns.link_between_nodes(stream->stream_data, u, w);

				SGA_IntervalArrayList times_uv	    = SGA_collect_times(fns.times_link_present(stream->stream_data, uv));
				SGA_IntervalArrayList times_vw	    = SGA_collect_times(fns.times_link_present(stream->stream_data, vw));
				SGA_IntervalArrayList times_uv_N_vw = SGA_IntervalArrayList_intersection(&times_uv, &times_vw);

				size_t time_of_triplet = 0;
				for (size_t i = 0; i < times_uv_N_vw.length; i++) {
					time_of_triplet += SGA_Interval_duration(times_uv_N_vw.array[i]);
				}
				sum_den += time_of_triplet;

				if (uw != SIZE_MAX) { // Triangle
					SGA_IntervalArrayList times_uw = SGA_collect_times(fns.times_link_present(stream->stream_data, uw));
					SGA_IntervalArrayList times_uv_N_uw = SGA_IntervalArrayList_intersection(&times_uv, &times_uw);
					SGA_IntervalArrayList times_uv_N_uw_N_vw =
					    SGA_IntervalArrayList_intersection(&times_uv_N_uw, &times_vw);

					size_t time_of_triangle = 0;
					for (size_t i = 0; i < times_uv_N_uw_N_vw.length; i++) {
						time_of_triangle += SGA_Interval_duration(times_uv_N_uw_N_vw.array[i]);
					}
					sum_num += time_of_triangle;

					SGA_IntervalArrayList_destroy(times_uw);
					SGA_IntervalArrayList_destroy(times_uv_N_uw);
					SGA_IntervalArrayList_destroy(times_uv_N_uw_N_vw);
				}

				SGA_IntervalArrayList_destroy(times_uv);
				SGA_IntervalArrayList_destroy(times_vw);
				SGA_IntervalArrayList_destroy(times_uv_N_vw);
			}
		}
	}

	// Transitivity ratio is 0 if there are no triangles
	if (sum_den == 0) {
		return 0.0;
	}

	return (double)sum_num / (double)sum_den;
}

// TODO: move that to a more appropriate place
String SGA_Stream_to_string(const SGA_Stream* stream) {
	StreamFunctions fns   = STREAM_FUNCS(fns, stream);
	String str	      = String_from_duplicate("Stream {\n\tLifespan: ");
	SGA_Interval lifespan = fns.lifespan(stream->stream_data);
	String interval_str   = SGA_Interval_to_string(&lifespan);
	String_concat_consume(&str, interval_str);
	// TODO : rename push_str to append
	String_push_str(&str, "\n\tNodes:\n");
	SGA_NodesIterator nodes = fns.nodes_set(stream->stream_data);
	SGA_FOR_EACH_NODE(node_id, nodes) {
		String_append_formatted(&str, "\t\t%zu, Times : { ", node_id);
		SGA_TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
		SGA_FOR_EACH_TIME(interval, times) {
			String_append_formatted(&str, "[%zu, %zu], ", interval.start, interval.end);
		}
		String_push_str(&str, "}\n");

		// Neighbours
		String_push_str(&str, "\t\tNeighbours: ");
		SGA_LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
		SGA_FOR_EACH_LINK(link_id, neighbours) {
			SGA_Link link	      = fns.link_by_id(stream->stream_data, link_id);
			SGA_NodeId other_node = SGA_Link_get_other_node(&link, node_id);
			String_append_formatted(&str, "%zu, ", other_node);
		}
		String_push_str(&str, "\n");
	}
	String_push_str(&str, "\n");

	String_push_str(&str, "\tLinks: \n");
	SGA_LinksIterator links = fns.links_set(stream->stream_data);
	SGA_FOR_EACH_LINK(link_id, links) {
		SGA_Link link = fns.link_by_id(stream->stream_data, link_id);
		String_append_formatted(&str, "\t\t%zu-%zu, Times : { ", link.nodes[0], link.nodes[1]);
		SGA_TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
		SGA_FOR_EACH_TIME(interval, times) {
			String_append_formatted(&str, "[%zu, %zu], ", interval.start, interval.end);
		}
		String_push_str(&str, "}\n");
	}
	String_push_str(&str, "\n");

	String_push_str(&str, "}\n");
	return str;
}
