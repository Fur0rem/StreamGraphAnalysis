#include "metrics.h"
#include "generic_data_structures/arraylist.h"
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
		case SNAPSHOT_STREAM: {                                                                                                    \
			if (SnapshotStream_metrics_functions.function != NULL) {                                                           \
				return SnapshotStream_metrics_functions.function(stream);                                                  \
			}                                                                                                                  \
			break;                                                                                                             \
		}                                                                                                                          \
	}

// TODO : rename the functions to be more explicit
// TODO : rewrite them to be cleaner
// TODO: move them somewhere else
size_t cardinalOfT(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfT);
	CATCH_METRICS_IMPLEM(cardinalOfT, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	Interval lifespan   = fns.lifespan(stream->stream_data);
	UPDATE_CACHE(stream, cardinalOfT, Interval_duration(lifespan));
	return Interval_duration(lifespan);
}

size_t cardinalOfV(Stream* stream) {
	FETCH_CACHE(stream, cardinalOfV);
	CATCH_METRICS_IMPLEM(cardinalOfV, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	size_t count	    = COUNT_ITERATOR(nodes);
	UPDATE_CACHE(stream, cardinalOfV, count);
	return count;
}

size_t cardinalOfE(Stream* stream) {
	// CATCH_METRICS_IMPLEM(cardinalOfE, stream);
	FETCH_CACHE(stream, cardinalOfE);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	LinksIterator links = fns.links_set(stream->stream_data);
	size_t count	    = 0;
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
	size_t count	    = 0;
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
	size_t w	    = cardinalOfW(stream);
	size_t v	    = cardinalOfV(stream);
	size_t scaling	    = fns.scaling(stream->stream_data);
	return (double)w / (double)(v * scaling);
}

double Stream_contribution_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_node, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
	size_t t_v	    = total_time_of(times);
	size_t t	    = cardinalOfT(stream);
	return (double)t_v / (double)t;
}

double Stream_contribution_of_link(Stream* stream, LinkId link_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_link, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	TimesIterator times = fns.times_link_present(stream->stream_data, link_id);
	size_t t_v	    = total_time_of(times);
	size_t t	    = cardinalOfT(stream);
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
	size_t v_t	    = COUNT_ITERATOR(nodes);
	size_t scaling	    = fns.scaling(stream->stream_data);
	size_t v	    = cardinalOfV(stream);
	return (double)v_t / (double)(v * scaling);
}

size_t size_set_unordered_pairs_itself(size_t n) {
	return n * (n - 1) / 2;
}

double Stream_link_contribution_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(link_contribution_at_time, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	LinksIterator links = fns.links_present_at_t(stream->stream_data, time_id);
	size_t e_t	    = COUNT_ITERATOR(links);
	size_t scaling	    = fns.scaling(stream->stream_data);
	size_t v	    = cardinalOfV(stream);
	size_t vxv	    = size_set_unordered_pairs_itself(v);
	return (double)e_t / (double)(vxv * scaling);
}

double Stream_link_duration(Stream* stream) {
	// CATCH_METRICS_IMPLEM(link_duration, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t e	    = cardinalOfE(stream);
	size_t v	    = cardinalOfV(stream);
	size_t scaling	    = fns.scaling(stream->stream_data);
	size_t vxv	    = size_set_unordered_pairs_itself(v);
	return (double)e / (double)(vxv * scaling);
}

double Stream_uniformity(Stream* stream) {
	// CATCH_METRICS_IMPLEM(uniformity, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_num	    = 0;
	size_t sum_den	    = 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		NodesIterator other_pair = fns.nodes_set(stream->stream_data);
		FOR_EACH_NODE(other_node_id, other_pair) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node       = fns.times_node_present(stream->stream_data, node_id);
			TimesIterator times_other_node = fns.times_node_present(stream->stream_data, other_node_id);
			TimesIterator times_union      = TimesIterator_union(times_node, times_other_node);

			size_t t_u			 = total_time_of(times_union);
			times_node			 = fns.times_node_present(stream->stream_data, node_id);
			times_other_node		 = fns.times_node_present(stream->stream_data, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			size_t t_i			 = total_time_of(times_intersection);

			sum_num += t_i;
			sum_den += t_u;
		}
	}
	return (double)sum_num / (double)sum_den;
}

double Stream_uniformity_pair_nodes(Stream* stream, NodeId node1, NodeId node2) {
	// CATCH_METRICS_IMPLEM(uniformity_pair_nodes, stream);
	StreamFunctions fns	  = STREAM_FUNCS(fns, stream);
	TimesIterator times_node1 = fns.times_node_present(stream->stream_data, node1);
	TimesIterator times_node2 = fns.times_node_present(stream->stream_data, node2);
	TimesIterator times_union = TimesIterator_union(times_node1, times_node2);

	size_t t_u			 = total_time_of(times_union);
	times_node1			 = fns.times_node_present(stream->stream_data, node1);
	times_node2			 = fns.times_node_present(stream->stream_data, node2);
	TimesIterator times_intersection = TimesIterator_intersection(times_node1, times_node2);
	size_t t_i			 = total_time_of(times_intersection);

	return (double)t_i / (double)t_u;
}

double Stream_compactness(Stream* stream) {
	// CATCH_METRICS_IMPLEM(compactness, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	// Compute T' (the set of times where at least one node is present)
	// Compute V' (the set of nodes present at least once)
	size_t nb_nodes_exist	     = 0;
	TimeId first_node_apparition = SIZE_MAX;
	TimeId last_node_apparition  = 0;

	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		TimesIterator times = fns.times_node_present(stream->stream_data, node_id);
		size_t nb_times	    = 0;
		FOR_EACH_TIME(interval, times) {
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
	return (double)cardinalOfW(stream) / (double)((last_node_apparition - first_node_apparition) * nb_nodes_exist);
}

double Stream_density(Stream* stream) {
	CATCH_METRICS_IMPLEM(density, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_den	    = 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		NodesIterator other_nodes = fns.nodes_set(stream->stream_data);
		FOR_EACH_NODE(other_node_id, other_nodes) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node	 = fns.times_node_present(stream->stream_data, node_id);
			TimesIterator times_other_node	 = fns.times_node_present(stream->stream_data, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			sum_den += total_time_of(times_intersection);
		}
	}

	size_t sum_num	    = 0;
	LinksIterator links = fns.links_set(stream->stream_data);
	FOR_EACH_LINK(link_id, links) {
		TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += total_time_of(times_link);
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_link(Stream* stream, LinkId link_id) {
	StreamFunctions fns	 = STREAM_FUNCS(fns, stream);
	TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
	size_t sum_num		 = total_time_of(times_link);
	Link l			 = fns.link_by_id(stream->stream_data, link_id);
	TimesIterator t_u	 = fns.times_node_present(stream->stream_data, l.nodes[0]);
	TimesIterator t_v	 = fns.times_node_present(stream->stream_data, l.nodes[1]);
	TimesIterator t_i	 = TimesIterator_intersection(t_u, t_v);
	size_t sum_den		 = total_time_of(t_i);

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(density_of_node, stream);
	StreamFunctions fns	 = STREAM_FUNCS(fns, stream);
	LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	size_t sum_num		 = 0;
	size_t sum_den		 = 0;
	FOR_EACH_LINK(link_id, neighbours) {
		TimesIterator times_link = fns.times_link_present(stream->stream_data, link_id);
		sum_num += total_time_of(times_link);
	}

	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(other_node_id, nodes) {
		if (node_id == other_node_id) {
			continue;
		}
		TimesIterator times_node	 = fns.times_node_present(stream->stream_data, node_id);
		TimesIterator times_other_node	 = fns.times_node_present(stream->stream_data, other_node_id);
		TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
		sum_den += total_time_of(times_intersection);
	}
	// printf("sum_num = %zu, sum_den = %zu\n", sum_num, sum_den);
	return (double)sum_num / (double)sum_den;
}

double Stream_density_at_instant(Stream* stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(density_at_instant, stream);
	StreamFunctions fns	 = STREAM_FUNCS(fns, stream);
	LinksIterator links_at_t = fns.links_present_at_t(stream->stream_data, time_id);
	// printf("got links present at t\n");
	size_t et = 0;
	FOR_EACH_LINK(link_id, links_at_t) {
		// printf("link_id = %zu\n", link_id);
		et++;
	}
	NodesIterator nodes_at_t = fns.nodes_present_at_t(stream->stream_data, time_id);
	// printf("got nodes present at t\n");
	size_t vt = 0;
	FOR_EACH_NODE(node_id, nodes_at_t) {
		// printf("node_id = %zu\n", node_id);
		vt++;
	}
	// size_t et = COUNT_ITERATOR(links_at_t);
	// size_t et = COUNT_ITERATOR(links_at_t);
	// size_t vt = COUNT_ITERATOR(nodes_at_t);
	// printf("et = %zu, vt = %zu\n", et, vt);
	return (double)et / (double)(size_set_unordered_pairs_itself(vt));
}

double Stream_degree_of_node(Stream* stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(degree_of_node, stream);
	StreamFunctions fns	 = STREAM_FUNCS(fns, stream);
	LinksIterator neighbours = fns.neighbours_of_node(stream->stream_data, node_id);
	size_t sum_num		 = 0;
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
	double sum	    = 0;
	double w	    = (double)cardinalOfW(stream);
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(node_id, nodes) {
		double degree = Stream_degree_of_node(stream, node_id);
		size_t t_v    = total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += degree * ((double)t_v / w);
	}
	return sum;
}

double Stream_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = cardinalOfE(stream);
	size_t t	       = cardinalOfT(stream);
	size_t v	       = cardinalOfV(stream);
	return (double)(2 * number_of_links) / (double)(t * v);
}

double Stream_average_expected_degree(Stream* stream) {
	// CATCH_METRICS_IMPLEM(degree, stream);
	size_t number_of_links = cardinalOfE(stream);
	size_t number_of_nodes = cardinalOfW(stream);
	return (double)(2 * number_of_links) / (double)number_of_nodes;
}

double Stream_clustering_coeff_of_node(Stream* stream, NodeId node_id) {
	NodeId v	    = node_id;
	StreamFunctions fns = STREAM_FUNCS(fns, stream);

	LinksIterator neighbours_of_v = fns.neighbours_of_node(stream->stream_data, v);
	size_t sum_den		      = 0;
	size_t sum_num		      = 0;

	FOR_EACH_LINK(uv, neighbours_of_v) {
		LinksIterator neighbours_of_v_copy = fns.neighbours_of_node(stream->stream_data, v);
		FOR_EACH_LINK(vw, neighbours_of_v_copy) {
			Link vw_link = fns.link_by_id(stream->stream_data, vw);
			NodeId w     = Link_get_other_node(&vw_link, v);
			Link uv_link = fns.link_by_id(stream->stream_data, uv);
			NodeId u     = Link_get_other_node(&uv_link, v);

			if (u >= w) {
				continue;
			}

			LinkId uw = fns.link_between_nodes(stream->stream_data, u, w);
			if (uw == SIZE_MAX) {
				continue;
			}

			IntervalArrayList times_uv	 = SGA_collect_times(fns.times_link_present(stream->stream_data, uv));
			IntervalArrayList times_vw	 = SGA_collect_times(fns.times_link_present(stream->stream_data, vw));
			IntervalArrayList times_uv_vw	 = IntervalArrayList_intersection(&times_uv, &times_vw);
			IntervalArrayList times_uw	 = SGA_collect_times(fns.times_link_present(stream->stream_data, uw));
			IntervalArrayList times_uw_uv_vw = IntervalArrayList_intersection(&times_uw, &times_uv_vw);

			size_t t_uv_vw = 0;
			for (size_t i = 0; i < times_uv_vw.length; i++) {
				t_uv_vw += Interval_duration(times_uv_vw.array[i]);
			}

			size_t t_uw_uv_vw = 0;
			for (size_t i = 0; i < times_uw_uv_vw.length; i++) {
				t_uw_uv_vw += Interval_duration(times_uw_uv_vw.array[i]);
			}

			sum_den += t_uv_vw;
			sum_num += t_uw_uv_vw;

			IntervalArrayList_destroy(times_uv);
			IntervalArrayList_destroy(times_vw);
			IntervalArrayList_destroy(times_uv_vw);
			IntervalArrayList_destroy(times_uw);
			IntervalArrayList_destroy(times_uw_uv_vw);
		}
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_node_clustering_coeff(Stream* stream) {
	// CATCH_METRICS_IMPLEM(node_clustering_coeff, stream);
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	double sum	    = 0;
	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	double w	    = (double)cardinalOfW(stream);
	FOR_EACH_NODE(node_id, nodes) {
		double clustering_coeff = Stream_clustering_coeff_of_node(stream, node_id);
		size_t t_v		= total_time_of(fns.times_node_present(stream->stream_data, node_id));
		sum += clustering_coeff * ((double)t_v / w);
	}
	return sum;
}

double Stream_transitivity_ratio(Stream* stream) {
	// CATCH_METRICS_IMPLEM(transitivity_ratio, stream);

	// Compute the number of triangles
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	size_t sum_num	    = 0;
	size_t sum_den	    = 0;

	NodesIterator nodes = fns.nodes_set(stream->stream_data);
	FOR_EACH_NODE(u, nodes) {
		LinksIterator n_u = fns.neighbours_of_node(stream->stream_data, u);
		FOR_EACH_LINK(uv, n_u) {
			Link link_uv	  = fns.link_by_id(stream->stream_data, uv);
			NodeId v	  = Link_get_other_node(&link_uv, u);
			LinksIterator n_v = fns.neighbours_of_node(stream->stream_data, v);
			FOR_EACH_LINK(vw, n_v) {
				Link link_vw = fns.link_by_id(stream->stream_data, vw);
				NodeId w     = Link_get_other_node(&link_vw, v);

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

				IntervalArrayList times_uv	= SGA_collect_times(fns.times_link_present(stream->stream_data, uv));
				IntervalArrayList times_vw	= SGA_collect_times(fns.times_link_present(stream->stream_data, vw));
				IntervalArrayList times_uv_N_vw = IntervalArrayList_intersection(&times_uv, &times_vw);

				size_t time_of_triplet = 0;
				for (size_t i = 0; i < times_uv_N_vw.length; i++) {
					time_of_triplet += Interval_duration(times_uv_N_vw.array[i]);
				}
				sum_den += time_of_triplet;

				if (uw != SIZE_MAX) { // Triangle
					IntervalArrayList times_uw = SGA_collect_times(fns.times_link_present(stream->stream_data, uw));
					IntervalArrayList times_uv_N_uw	     = IntervalArrayList_intersection(&times_uv, &times_uw);
					IntervalArrayList times_uv_N_uw_N_vw = IntervalArrayList_intersection(&times_uv_N_uw, &times_vw);

					size_t time_of_triangle = 0;
					for (size_t i = 0; i < times_uv_N_uw_N_vw.length; i++) {
						time_of_triangle += Interval_duration(times_uv_N_uw_N_vw.array[i]);
					}
					sum_num += time_of_triangle;

					IntervalArrayList_destroy(times_uw);
					IntervalArrayList_destroy(times_uv_N_uw);
					IntervalArrayList_destroy(times_uv_N_uw_N_vw);
				}

				IntervalArrayList_destroy(times_uv);
				IntervalArrayList_destroy(times_vw);
				IntervalArrayList_destroy(times_uv_N_vw);
			}
		}
	}

	return (double)sum_num / (double)sum_den;
}

String Stream_to_string(const Stream* stream) {
	StreamFunctions fns = STREAM_FUNCS(fns, stream);
	String str	    = String_from_duplicate("Stream {\n\tLifespan: ");
	Interval lifespan   = fns.lifespan(stream->stream_data);
	String interval_str = Interval_to_string(&lifespan);
	String_concat_consume(&str, interval_str);
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
			Link link	  = fns.link_by_id(stream->stream_data, link_id);
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