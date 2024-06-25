#include "metrics.h"
#include "induced_graph.h"
#include "interval.h"
#include "iterators.h"
#include "stream.h"
#include "stream/chunk_stream.h"
#include "stream/full_stream_graph.h"
#include "stream/link_stream.h"
#include "stream_functions.h"
#include "stream_graph.h"
#include "units.h"
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
	switch (stream.type) {                                                                                             \
		case FULL_STREAM_GRAPH: {                                                                                      \
			if (FullStreamGraph_metrics_functions.function != NULL) {                                                  \
				return FullStreamGraph_metrics_functions.function(stream.stream);                                      \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case LINK_STREAM: {                                                                                            \
			if (LinkStream_metrics_functions.function != NULL) {                                                       \
				return LinkStream_metrics_functions.function(stream.stream);                                           \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
		case CHUNK_STREAM: {                                                                                           \
			if (ChunkStream_metrics_functions.function != NULL) {                                                      \
				return ChunkStream_metrics_functions.function(stream.stream);                                          \
			}                                                                                                          \
			break;                                                                                                     \
		}                                                                                                              \
	}                                                                                                                  \
	printf("COULD NOT CATCH " #function "\n");

// TODO : rename the functions to be more explicit
// TODO : rewrite them to be cleaner
size_t cardinalOfT(Stream stream) {
	CATCH_METRICS_IMPLEM(cardinalOfT, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	Interval lifespan = stream_functions.lifespan(stream.stream);
	return Interval_size(lifespan);
}

size_t cardinalOfV(Stream stream) {
	CATCH_METRICS_IMPLEM(cardinalOfV, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	size_t count = 0;
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		count++;
	}
	return count;
}

size_t cardinalOfE(Stream stream) {
	// CATCH_METRICS_IMPLEM(cardinalOfE, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator links = stream_functions.links_set(stream.stream);
	size_t count = 0;
	FOR_EACH(LinkId, link_id, links, link_id != SIZE_MAX) {
		TimesIterator times = stream_functions.times_link_present(stream.stream, link_id);
		count += total_time_of(times);
	}
	return count;
}

size_t cardinalOfW(Stream stream) {
	CATCH_METRICS_IMPLEM(cardinalOfW, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	size_t count = 0;
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		TimesIterator times = stream_functions.times_node_present(nodes.stream_graph.stream, node_id);
		count += total_time_of(times);
	}
	return count;
}

double Stream_coverage(Stream stream) {
	CATCH_METRICS_IMPLEM(coverage, stream);
	size_t w = cardinalOfW(stream);
	size_t t = cardinalOfT(stream);
	size_t v = cardinalOfV(stream);

	return (double)w / (double)(t * v);
}

double Stream_node_duration(Stream stream) {
	CATCH_METRICS_IMPLEM(node_duration, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t w = cardinalOfW(stream);
	size_t v = cardinalOfV(stream);
	size_t scaling = stream_functions.scaling(stream.stream);
	return (double)w / (double)(v * scaling);
}

double Stream_contribution_of_node(Stream stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_node, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times = stream_functions.times_node_present(stream.stream, node_id);
	size_t t_v = total_time_of(times);
	size_t t = cardinalOfT(stream);
	return (double)t_v / (double)t;
}

double Stream_contribution_of_link(Stream stream, LinkId link_id) {
	// CATCH_METRICS_IMPLEM(contribution_of_link, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times = stream_functions.times_link_present(stream.stream, link_id);
	size_t t_v = total_time_of(times);
	size_t t = cardinalOfT(stream);
	return (double)t_v / (double)(t);
}

double Stream_number_of_nodes(Stream stream) {
	// CATCH_METRICS_IMPLEM(number_of_nodes, stream);
	size_t w = cardinalOfW(stream);
	size_t t = cardinalOfT(stream);
	return (double)w / (double)t;
}

double Stream_number_of_links(Stream stream) {
	// CATCH_METRICS_IMPLEM(number_of_links, stream);
	size_t e = cardinalOfE(stream);
	size_t t = cardinalOfT(stream);
	return (double)e / (double)t;
}

double Stream_node_contribution_at_time(Stream stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(node_contribution_at_time, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes = stream_functions.nodes_present_at_t(stream.stream, time_id);
	size_t v_t = COUNT_ITERATOR(nodes);
	size_t scaling = stream_functions.scaling(stream.stream);
	size_t v = cardinalOfV(stream);
	return (double)v_t / (double)(v * scaling);
}

size_t size_set_unordered_pairs_itself(size_t n) {
	return n * (n - 1) / 2;
}

double Stream_link_contribution_at_time(Stream stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(link_contribution_at_time, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator links = stream_functions.links_present_at_t(stream.stream, time_id);
	size_t e_t = COUNT_ITERATOR(links);
	size_t scaling = stream_functions.scaling(stream.stream);
	size_t v = cardinalOfV(stream);
	size_t vxv = size_set_unordered_pairs_itself(v);
	return (double)e_t / (double)(vxv * scaling);
}

double Stream_link_duration(Stream stream) {
	// CATCH_METRICS_IMPLEM(link_duration, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t e = cardinalOfE(stream);
	size_t v = cardinalOfV(stream);
	size_t scaling = stream_functions.scaling(stream.stream);
	size_t vxv = size_set_unordered_pairs_itself(v);
	return (double)e / (double)(vxv * scaling);
}

double Stream_uniformity(Stream stream) {
	// CATCH_METRICS_IMPLEM(uniformity, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t sum_num = 0;
	size_t sum_den = 0;
	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	FOR_EACH(NodeId, node_id, nodes, node_id != SIZE_MAX) {
		NodesIterator other_pair = stream_functions.nodes_set(stream.stream);
		FOR_EACH(NodeId, other_node_id, other_pair, other_node_id != SIZE_MAX) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node = stream_functions.times_node_present(stream.stream, node_id);
			TimesIterator times_other_node = stream_functions.times_node_present(stream.stream, other_node_id);
			TimesIterator times_union = TimesIterator_union(times_node, times_other_node);

			size_t t_u = total_time_of(times_union);
			times_node = stream_functions.times_node_present(stream.stream, node_id);
			times_other_node = stream_functions.times_node_present(stream.stream, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			size_t t_i = total_time_of(times_intersection);

			sum_num += t_i;
			sum_den += t_u;
		}
	}
	return (double)sum_num / (double)sum_den;
}

double Stream_uniformity_pair_nodes(Stream stream, NodeId node1, NodeId node2) {
	// CATCH_METRICS_IMPLEM(uniformity_pair_nodes, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times_node1 = stream_functions.times_node_present(stream.stream, node1);
	TimesIterator times_node2 = stream_functions.times_node_present(stream.stream, node2);
	TimesIterator times_union = TimesIterator_union(times_node1, times_node2);

	size_t t_u = total_time_of(times_union);
	times_node1 = stream_functions.times_node_present(stream.stream, node1);
	times_node2 = stream_functions.times_node_present(stream.stream, node2);
	TimesIterator times_intersection = TimesIterator_intersection(times_node1, times_node2);
	size_t t_i = total_time_of(times_intersection);

	return (double)t_i / (double)t_u;
}

double Stream_density(Stream stream) {
	CATCH_METRICS_IMPLEM(density, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	size_t sum_den = 0;
	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	FOR_EACH_NODE(nodes, node_id) {
		NodesIterator other_nodes = stream_functions.nodes_set(stream.stream);
		FOR_EACH_NODE(other_nodes, other_node_id) {
			if (node_id >= other_node_id) {
				continue;
			}
			TimesIterator times_node = stream_functions.times_node_present(stream.stream, node_id);
			TimesIterator times_other_node = stream_functions.times_node_present(stream.stream, other_node_id);
			TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
			sum_den += total_time_of(times_intersection);
		}
	}

	size_t sum_num = 0;
	LinksIterator links = stream_functions.links_set(stream.stream);
	FOR_EACH_LINK(links, link_id) {
		TimesIterator times_link = stream_functions.times_link_present(stream.stream, link_id);
		sum_num += total_time_of(times_link);
	}

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_link(Stream stream, LinkId link_id) {
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	TimesIterator times_link = stream_functions.times_link_present(stream.stream, link_id);
	size_t sum_num = total_time_of(times_link);
	Link l = stream_functions.nth_link(stream.stream, link_id);
	TimesIterator t_u = stream_functions.times_node_present(stream.stream, l.nodes[0]);
	TimesIterator t_v = stream_functions.times_node_present(stream.stream, l.nodes[1]);
	TimesIterator t_i = TimesIterator_intersection(t_u, t_v);
	size_t sum_den = total_time_of(t_i);

	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_node(Stream stream, NodeId node_id) {
	// CATCH_METRICS_IMPLEM(density_of_node, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	LinksIterator neighbours = stream_functions.neighbours_of_node(stream.stream, node_id);
	size_t sum_num = 0;
	size_t sum_den = 0;
	FOR_EACH_LINK(neighbours, link_id) {
		TimesIterator times_link = stream_functions.times_link_present(stream.stream, link_id);
		sum_num += total_time_of(times_link);
	}

	NodesIterator nodes = stream_functions.nodes_set(stream.stream);
	FOR_EACH_NODE(nodes, other_node_id) {
		if (node_id == other_node_id) {
			continue;
		}
		TimesIterator times_node = stream_functions.times_node_present(stream.stream, node_id);
		TimesIterator times_other_node = stream_functions.times_node_present(stream.stream, other_node_id);
		TimesIterator times_intersection = TimesIterator_intersection(times_node, times_other_node);
		sum_den += total_time_of(times_intersection);
	}
	printf("sum_num = %zu, sum_den = %zu\n", sum_num, sum_den);
	return (double)sum_num / (double)sum_den;
}

double Stream_density_of_time(Stream stream, TimeId time_id) {
	// CATCH_METRICS_IMPLEM(density_of_time, stream);
	StreamFunctions stream_functions = STREAM_FUNCS(stream_functions, stream);
	NodesIterator nodes_at_t = stream_functions.nodes_present_at_t(stream.stream, time_id);
	LinksIterator links_at_t = stream_functions.links_present_at_t(stream.stream, time_id);
	// size_t et = COUNT_ITERATOR(links_at_t);
	size_t et = COUNT_ITERATOR(links_at_t);
	size_t vt = COUNT_ITERATOR(nodes_at_t);
	printf("et = %zu, vt = %zu\n", et, vt);
	return (double)et / (double)(size_set_unordered_pairs_itself(vt));
}