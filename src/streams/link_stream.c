#include <stdint.h>
#define SGA_INTERNAL

#include "../analysis/metrics.h"
#include "../interval.h"
#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/key_instants.h" // TODO: maybe rename to time_access
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"
#include "../utils.h"
#include "link_stream.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

SGA_Stream SGA_LinkStream_from(SGA_StreamGraph* stream_graph) {
	LinkStream* link_stream		     = MALLOC(sizeof(LinkStream));
	link_stream->underlying_stream_graph = stream_graph;
	SGA_Stream stream		     = {.type = LINK_STREAM, .stream_data = link_stream};
	init_cache(&stream);
	return stream;
}

void SGA_LinkStream_destroy(SGA_Stream stream) {
	free(stream.stream_data);
}

SGA_NodesIterator LinkStream_nodes_set(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_nodes_set(stream_graph);
}

SGA_LinksIterator LinkStream_links_set(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_links_set(stream_graph);
}

SGA_Interval LinkStream_lifespan(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_lifespan(stream_graph);
}

size_t LinkStream_time_scale(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return SGA_StreamGraph_time_scale(stream_graph);
}

SGA_NodesIterator LinkStream_nodes_present_at_t(SGA_StreamData* stream_data, SGA_TimeId instant) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(SGA_Interval_contains(SGA_StreamGraph_lifespan(link_stream->underlying_stream_graph), instant));

	// In a linkstream, all nodes are present at all times
	return SGA_StreamGraph_nodes_set(link_stream->underlying_stream_graph);
}

SGA_LinksIterator LinkStream_links_present_at_t(SGA_StreamData* stream_data, SGA_TimeId instant) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(SGA_Interval_contains(SGA_StreamGraph_lifespan(link_stream->underlying_stream_graph), instant));

	return SGA_StreamGraph_links_present_at(link_stream->underlying_stream_graph, instant);
}

typedef struct {
	bool has_been_called;
} TimesNodePresentIteratorData;

SGA_Interval LinkStream_TimesNodePresent_next(SGA_TimesIterator* iter) {
	TimesNodePresentIteratorData* times_iter_data = (TimesNodePresentIteratorData*)iter->iterator_data;
	LinkStream* ls				      = iter->stream_graph.stream_data;
	if (times_iter_data->has_been_called) {
		return SGA_TIMES_ITERATOR_END;
	}
	times_iter_data->has_been_called = true;
	return SGA_StreamGraph_lifespan(ls->underlying_stream_graph);
}

void LinkStream_TimesNodePresentIterator_destroy(SGA_TimesIterator* iterator) {
	free(iterator->iterator_data);
}

// Every node is always present, so it's no use to specify the node ID
SGA_TimesIterator LinkStream_times_node_present(SGA_StreamData* link_stream, __attribute__((unused)) SGA_NodeId node_id) {
	TimesNodePresentIteratorData* iterator_data = MALLOC(sizeof(TimesNodePresentIteratorData));
	SGA_Stream stream			    = {.stream_data = link_stream};
	iterator_data->has_been_called		    = false;
	SGA_TimesIterator times_iterator	    = {
		       .stream_graph  = stream,
		       .iterator_data = iterator_data,
		       .next	      = LinkStream_TimesNodePresent_next,
		       .destroy	      = LinkStream_TimesNodePresentIterator_destroy,
	   };
	return times_iterator;
}

SGA_TimesIterator LinkStream_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(link_id < link_stream->underlying_stream_graph->links.nb_links);

	return SGA_StreamGraph_times_link_present(link_stream->underlying_stream_graph, link_id);
}

SGA_Link LinkStream_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	LinkStream* link_stream = (LinkStream*)stream_data;

	ASSERT(link_id < link_stream->underlying_stream_graph->links.nb_links);

	return link_stream->underlying_stream_graph->links.links[link_id];
}

SGA_LinksIterator LinkStream_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node_id) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_neighbours_of_node(stream_graph, node_id);
}

SGA_TimesIterator LinkStream_key_instants(SGA_StreamData* stream_data) {
	LinkStream* link_stream	      = (LinkStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	return SGA_StreamGraph_key_instants(stream_graph);
}

const StreamFunctions LinkStream_stream_functions = {
    .nodes_set		= LinkStream_nodes_set,
    .links_set		= LinkStream_links_set,
    .lifespan		= LinkStream_lifespan,
    .time_scale		= LinkStream_time_scale,
    .nodes_present_at_t = LinkStream_nodes_present_at_t,
    .links_present_at_t = LinkStream_links_present_at_t,
    .times_node_present = LinkStream_times_node_present,
    .times_link_present = LinkStream_times_link_present,
    .link_by_id		= LinkStream_link_by_id,
    .neighbours_of_node = LinkStream_neighbours_of_node,
    .key_instants	= LinkStream_key_instants,
};

// Every LinkStream has a coverage of 1 by definition, since all nodes are present at all times
double LS_coverage(__attribute__((unused)) SGA_Stream* stream_data) {
	return 1.0;
}

size_t LS_cardinal_of_T(SGA_Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t t       = SGA_Interval_duration(SGA_StreamGraph_lifespan(ls->underlying_stream_graph));
	UPDATE_CACHE(stream, duration, t);
	return t;
}

size_t LS_cardinal_of_V(SGA_Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t v       = ls->underlying_stream_graph->nodes.nb_nodes;
	UPDATE_CACHE(stream, distinct_cardinal_of_node_set, v);
	return v;
}

size_t LS_cardinal_of_W(SGA_Stream* stream) {
	size_t v = LS_cardinal_of_V(stream);
	size_t t = LS_cardinal_of_T(stream);
	size_t w = v * t;
	UPDATE_CACHE(stream, temporal_cardinal_of_node_set, w);
	return w;
}

size_t LS_cardinal_of_E(SGA_Stream* stream) {
	LinkStream* ls = (LinkStream*)stream->stream_data;
	size_t e       = 0;
	for (size_t i = 0; i < ls->underlying_stream_graph->links.nb_links; i++) {
		SGA_Link link = ls->underlying_stream_graph->links.links[i];
		for (size_t j = 0; j < link.presence.nb_intervals; j++) {
			e += SGA_Interval_duration(link.presence.intervals[j]);
		}
	}
	UPDATE_CACHE(stream, temporal_cardinal_of_link_set, e);
	return e;
}

double LinkStream_density(SGA_Stream* stream) {
	size_t n = LS_cardinal_of_V(stream);
	// we multiply by 2 here because it's faster to double an integer than to double a float
	double m = (double)(2 * LS_cardinal_of_E(stream)) / (double)LS_cardinal_of_T(stream);
	return m / (double)(n * (n - 1));
}

size_t LinkStream_distinct_cardinal_of_link_set(SGA_Stream* stream) {
	LinkStream* link_stream	      = (LinkStream*)stream->stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return stream_graph->links.nb_links;
}

size_t LinkStream_distinct_cardinal_of_node_set(SGA_Stream* stream) {
	LinkStream* link_stream	      = (LinkStream*)stream->stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	return stream_graph->nodes.nb_nodes;
}

const MetricsFunctions LinkStream_metrics_functions = {
    .coverage			   = (double (*)(const SGA_Stream*))LS_coverage,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = (size_t (*)(const SGA_Stream*))LS_cardinal_of_V,
    .temporal_cardinal_of_node_set = (size_t (*)(const SGA_Stream*))LS_cardinal_of_W,
    .node_duration		   = NULL,
    .density			   = (double (*)(const SGA_Stream*))LinkStream_density,
};

//////////////////////////
//// Weighted version ////
//////////////////////////

/**
 * @brief Creates a weighted Stream of a LinkStream from a weighted StreamGraph (i.e. all nodes are present at all times)
 * @param[in] stream_graph The weighted StreamGraph
 * @param[in] node_weights_fill The weights used for the extensions of nodes. Since turning a stream graph into a link stream creates node
 * presences, it is the weight function used for what got created, used only when the nodes were not already present there.
 * @return The link stream as a weighted stream
 */
SGA_W_Stream SGA_W_LinkStream_from(SGA_W_StreamGraph* stream_graph, SGA_WeightFunc node_weights_fill) {
	W_LinkStream* link_stream	     = MALLOC(sizeof(W_LinkStream));
	link_stream->underlying_stream_graph = stream_graph;
	link_stream->extended_nodes_weights  = node_weights_fill;

	SGA_W_Stream stream = {
	    .base =
		{
		    .type	 = LINK_STREAM,
		    .stream_data = &stream_graph->base,
		},
	    .stream_data = link_stream,
	};

	init_cache(&stream.base);

	return stream;
}

void SGA_W_LinkStream_destroy(SGA_W_Stream self) {
	W_LinkStream* link_stream = (W_LinkStream*)self.stream_data;
	SGA_WeightFunc_destroy(link_stream->extended_nodes_weights);
	free(link_stream);
}

bool is_node_present_at(SGA_NodeId node_id, SGA_Time time, SGA_W_StreamGraph* stream_graph) {
	SGA_Node* node = &stream_graph->base.nodes.nodes[node_id];
	// FIXME: Do binary search instead for better performance
	for (size_t i = 0; i < node->presence.nb_intervals; i++) {
		if (SGA_Interval_contains(node->presence.intervals[i], time)) {
			return true;
		}
	}
	return false;
}

SGA_Weight SGA_W_LinkStream_node_weight_at_t(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Time time) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;
	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, time));

	// If the node was already present originally, query the base weight function
	if (is_node_present_at(node, time, stream_graph)) {
		return SGA_W_StreamGraph_node_weight_at_t(stream_graph, node, time);
	}
	// Otherwise, query the extended weight function
	else {
		return SGA_WeightFunc_weight_at_t(&stream_graph->node_weights, node, time);
	}
}

SGA_Weight SGA_W_LinkStream_weight_integral_of_node_between(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Interval interval) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains_interval(stream_graph->base.lifespan, interval));

	// Compute the integral by splitting the interval into sub-integrals where the weight function doesn't switch between base/extended
	// TODO: optimize by not looping over all intervals every time
	SGA_Weight total_weight = 0;
	SGA_Time current_time	= interval.start;
	while (current_time < interval.end) {
		// Find the next time where the node presence changes
		SGA_Time next_switch_time = interval.end;
		SGA_Node* n		  = &stream_graph->base.nodes.nodes[node];
		for (size_t i = 0; i < n->presence.nb_intervals; i++) {
			SGA_Interval pres = n->presence.intervals[i];
			if (pres.start > current_time && pres.start < next_switch_time) {
				next_switch_time = pres.start;
			}
			if (pres.end > current_time && pres.end < next_switch_time) {
				next_switch_time = pres.end;
			}
		}

		// Compute the integral on the sub-interval
		SGA_Interval sub_interval = {
		    .start = current_time,
		    .end   = next_switch_time,
		};
		if (is_node_present_at(node, current_time, stream_graph)) {
			total_weight += SGA_W_StreamGraph_weight_integral_of_node_between(stream_graph, node, sub_interval);
		}
		else {
			total_weight += SGA_WeightFunc_weight_integral_between(&stream_graph->node_weights, node, sub_interval);
		}

		// Go to the next sub-interval
		current_time = next_switch_time;
	}

	return total_weight;
}

SGA_Weight SGA_W_LinkStream_link_weight_at_t(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Time time) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains(stream_graph->base.lifespan, time));

	return SGA_W_StreamGraph_link_weight_at_t(stream_graph, link, time);
}

SGA_Weight SGA_W_LinkStream_weight_integral_of_link_between(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Interval interval) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains_interval(stream_graph->base.lifespan, interval));

	return SGA_W_StreamGraph_weight_integral_of_link_between(stream_graph, link, interval);
}

/**
 * @brief Finds the extremum (maximum or minimum) node weight in the LinkStream, considering both the base weights and the extended weights.
 * @param stream The weighted LinkStream to analyze.
 * @param sign +1 to find the maximum weight, -1 to find the minimum weight. (because min(f(x)) = -max(-f(x)))
 */
SGA_Weight SGA_W_LinkStream_extremum_node_weight(const SGA_W_Stream* stream, SGA_Weight sign) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	SGA_Weight max_base = SGA_W_StreamGraph_max_node_weight(stream_graph) * sign;

	// Look for the max in the extended weights, where the nodes were not originally present
	switch (stream_graph->node_weights.tag) {
		// For an universally constant function, this is just the constant value
		// We just have to check that it is actually applied at some point
		case CONST_UNIVERSALLY: {
			ConstUniversally* const_func = &stream_graph->node_weights.func.const_universally;

			// OPTIMISATION: if the base weight is greater than the constant, we can return it directly
			// Since we can assume that the base stream graph is not empty
			if (const_func->weight * sign <= max_base) {
				return max_base * sign;
			}

			// Check if there is at least one node that is not always present
			for (SGA_NodeId node = 0; node < stream_graph->base.nodes.nb_nodes; node++) {
				SGA_Node* n = &stream_graph->base.nodes.nodes[node];
				if (n->presence.nb_intervals == 1 &&
				    SGA_Interval_equals(&n->presence.intervals[0], &stream_graph->base.lifespan)) {
					return const_func->weight * sign;
				}
			}

			// All nodes are always present, so the extended weight function is never used
			return max_base * sign;
		}
		// For a lerp function, we can't just query the max of the extended weight function, because it might at a time where the
		// base weight function should be used.
		case LERP: {
			SGA_Weight max_extended = -INFINITY * sign;
			for (SGA_NodeId node = 0; node < stream_graph->base.nodes.nb_nodes; node++) {
				SGA_Node* n = &stream_graph->base.nodes.nodes[node];

				// Look for gaps in the presence of the node
				SGA_Time current_time = stream_graph->base.lifespan.start;
				for (size_t i = 0; i <= n->presence.nb_intervals; i++) {
					SGA_Time next_time;
					if (i == n->presence.nb_intervals) {
						next_time = stream_graph->base.lifespan.end;
					}
					else {
						next_time = n->presence.intervals[i].start;
					}

					// Check if there is a gap, and therefore that the extended weight function applies
					if (current_time < next_time) {
						SGA_Weight max_in_gap =
						    SGA_WeightFunc_max_in_interval(
							&stream_graph->node_weights, node, SGA_Interval_from(current_time, next_time)) *
						    sign;
						if (max_in_gap > max_extended) {
							max_extended = max_in_gap;
						}
					}

					// Move to the end of the current presence interval
					if (i < n->presence.nb_intervals) {
						current_time = n->presence.intervals[i].end;
					}
				}
			}

			// Return the maximum between the base and extended weights
			return fmax(max_base, max_extended) * sign;
		}

		default: {
			UNREACHABLE_CODE;
		}
	}
}

SGA_Weight SGA_W_LinkStream_max_node_weight(const SGA_W_Stream* stream) {
	return SGA_W_LinkStream_extremum_node_weight(stream, +1);
}

SGA_Weight SGA_W_LinkStream_min_node_weight(const SGA_W_Stream* stream) {
	return SGA_W_LinkStream_extremum_node_weight(stream, -1);
}

void SGA_W_LinkStream_normalise_node_weights(SGA_W_Stream* stream) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	SGA_Weight min = SGA_W_LinkStream_min_node_weight(stream);
	SGA_Weight max = SGA_W_LinkStream_max_node_weight(stream);
	SGA_WeightFunc_normalise(&stream_graph->node_weights, min, max);
}

SGA_Weight SGA_W_LinkStream_max_link_weight(const SGA_W_Stream* stream) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	return SGA_W_StreamGraph_max_link_weight(stream_graph);
}

SGA_Weight SGA_W_LinkStream_min_link_weight(const SGA_W_Stream* stream) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	return SGA_W_StreamGraph_min_link_weight(stream_graph);
}

void SGA_W_LinkStream_normalise_link_weights(SGA_W_Stream* stream) {
	W_LinkStream* link_stream	= (W_LinkStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	SGA_Weight min = SGA_W_LinkStream_min_link_weight(stream);
	SGA_Weight max = SGA_W_LinkStream_max_link_weight(stream);
	SGA_WeightFunc_normalise(&stream_graph->link_weights, min, max);
}

const WeightedStreamFunctions LinkStream_weighted_stream_functions = {
    .node_weight_at_t		     = SGA_W_LinkStream_node_weight_at_t,
    .weight_integral_of_node_between = SGA_W_LinkStream_weight_integral_of_node_between,
    .link_weight_at_t		     = SGA_W_LinkStream_link_weight_at_t,
    .weight_integral_of_link_between = SGA_W_LinkStream_weight_integral_of_link_between,
    .max_node_weight		     = SGA_W_LinkStream_max_node_weight,
    .min_node_weight		     = SGA_W_LinkStream_min_node_weight,
    .normalise_node_weights	     = SGA_W_LinkStream_normalise_node_weights,
    .max_link_weight		     = SGA_W_LinkStream_max_link_weight,
    .min_link_weight		     = SGA_W_LinkStream_min_link_weight,
    .normalise_link_weights	     = SGA_W_LinkStream_normalise_link_weights,
};