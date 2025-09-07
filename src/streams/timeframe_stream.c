#define SGA_INTERNAL

#include "timeframe_stream.h"

#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/key_instants.h"
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"

#include <stddef.h>
#include <stdlib.h>

SGA_Stream SGA_TimeFrameStream_from(SGA_StreamGraph* stream_graph, SGA_Interval timeframe) {
	TimeFrameStream* timeframe_stream = MALLOC(sizeof(TimeFrameStream));

	*timeframe_stream = (TimeFrameStream){
	    .underlying_stream_graph = stream_graph,
	    .timeframe		     = timeframe,
	};
	SGA_Stream stream = {.type = TIMEFRAME_STREAM, .stream_data = timeframe_stream};
	init_cache(&stream);
	return stream;
}

void SGA_TimeFrameStream_destroy(SGA_Stream stream) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream.stream_data;
	free(timeframe_stream);
}

SGA_NodesIterator TimeFrameStream_nodes_set(SGA_StreamData* stream_data) {
	TimeFrameStream* snap	      = (TimeFrameStream*)stream_data;
	SGA_StreamGraph* stream_graph = snap->underlying_stream_graph;
	return SGA_StreamGraph_nodes_set(stream_graph);
}

SGA_LinksIterator TimeFrameStream_links_set(SGA_StreamData* stream_data) {
	TimeFrameStream* snap	      = (TimeFrameStream*)stream_data;
	SGA_StreamGraph* stream_graph = snap->underlying_stream_graph;
	return SGA_StreamGraph_links_set(stream_graph);
}

SGA_Interval TimeFrameStream_lifespan(SGA_StreamData* stream_data) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	return timeframe_stream->timeframe;
}

size_t TimeFrameStream_time_scale(SGA_StreamData* stream_data) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	return SGA_StreamGraph_time_scale(timeframe_stream->underlying_stream_graph);
}

SGA_NodesIterator TimeFrameStream_nodes_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	SGA_StreamGraph* stream_graph	  = timeframe_stream->underlying_stream_graph;

	ASSERT(SGA_Interval_contains(timeframe_stream->timeframe, instant));

	return SGA_StreamGraph_nodes_present_at(stream_graph, instant);
}

SGA_LinksIterator TimeFrameStream_links_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	SGA_StreamGraph* stream_graph	  = timeframe_stream->underlying_stream_graph;

	ASSERT(SGA_Interval_contains(timeframe_stream->timeframe, instant));

	return SGA_StreamGraph_links_present_at(stream_graph, instant);
}

SGA_LinksIterator TimeFrameStream_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	SGA_StreamGraph* stream_graph	  = timeframe_stream->underlying_stream_graph;

	ASSERT(node < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_neighbours_of_node(stream_graph, node);
}

/**
 * @brief Iterator data for iterating over the times at which a node/link is present.
 */
typedef struct {
	size_t current_time;   ///< The current index of the time intervals being iterated.
	size_t id_of_iterated; ///< The ID of the node or link being iterated over.
} TFS_TimesIDPresentAtIteratorData;

SGA_Interval TFS_TimesNodePresentAt_next(SGA_TimesIterator* iter) {
	TFS_TimesIDPresentAtIteratorData* times_iter_data = (TFS_TimesIDPresentAtIteratorData*)iter->iterator_data;
	TimeFrameStream* timeframe_stream		  = (TimeFrameStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			  = timeframe_stream->underlying_stream_graph;
	SGA_NodeId node					  = times_iter_data->id_of_iterated;
	if (times_iter_data->current_time >= stream_graph->nodes.nodes[node].presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval nth_time = stream_graph->nodes.nodes[node].presence.intervals[times_iter_data->current_time];
	nth_time	      = SGA_Interval_intersection(nth_time, timeframe_stream->timeframe);
	times_iter_data->current_time++;
	return nth_time;
}

void TFS_TimesNodePresentAtIterator_destroy(SGA_TimesIterator* iterator) {
	free(iterator->iterator_data);
}

SGA_TimesIterator TimeFrameStream_times_node_present(SGA_StreamData* stream_data, SGA_NodeId node) {
	TimeFrameStream* timeframe_stream		= (TimeFrameStream*)stream_data;
	TFS_TimesIDPresentAtIteratorData* iterator_data = MALLOC(sizeof(TFS_TimesIDPresentAtIteratorData));
	size_t nb_skips					= 0;
	while (nb_skips < timeframe_stream->underlying_stream_graph->nodes.nodes[node].presence.nb_intervals &&
	       timeframe_stream->underlying_stream_graph->nodes.nodes[node].presence.intervals[nb_skips].end <
		   timeframe_stream->timeframe.start) {
		nb_skips++;
	}
	*iterator_data = (TFS_TimesIDPresentAtIteratorData){
	    .current_time   = nb_skips,
	    .id_of_iterated = node,
	};
	SGA_Stream stream = {.type = TIMEFRAME_STREAM, .stream_data = timeframe_stream};

	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = TFS_TimesNodePresentAt_next,
	    .destroy	   = TFS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

// same for links now
SGA_Interval TFS_TimesLinkPresentAt_next(SGA_TimesIterator* iter) {
	TFS_TimesIDPresentAtIteratorData* times_iter_data = (TFS_TimesIDPresentAtIteratorData*)iter->iterator_data;
	TimeFrameStream* timeframe_stream		  = (TimeFrameStream*)iter->stream_graph.stream_data;
	SGA_StreamGraph* stream_graph			  = timeframe_stream->underlying_stream_graph;
	SGA_LinkId link					  = times_iter_data->id_of_iterated;
	if (times_iter_data->current_time >= stream_graph->links.links[link].presence.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	SGA_Interval nth_time = stream_graph->links.links[link].presence.intervals[times_iter_data->current_time];
	nth_time	      = SGA_Interval_intersection(nth_time, timeframe_stream->timeframe);
	times_iter_data->current_time++;
	return nth_time;
}

SGA_TimesIterator TimeFrameStream_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link) {
	TimeFrameStream* timeframe_stream		= (TimeFrameStream*)stream_data;
	TFS_TimesIDPresentAtIteratorData* iterator_data = MALLOC(sizeof(TFS_TimesIDPresentAtIteratorData));
	size_t nb_skips					= 0;
	while (nb_skips < timeframe_stream->underlying_stream_graph->links.links[link].presence.nb_intervals &&
	       timeframe_stream->underlying_stream_graph->links.links[link].presence.intervals[nb_skips].end <
		   timeframe_stream->timeframe.start) {
		nb_skips++;
	}
	*iterator_data = (TFS_TimesIDPresentAtIteratorData){
	    .current_time   = nb_skips,
	    .id_of_iterated = link,
	};
	SGA_Stream stream = {.type = TIMEFRAME_STREAM, .stream_data = timeframe_stream};

	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = TFS_TimesLinkPresentAt_next,
	    .destroy	   = TFS_TimesNodePresentAtIterator_destroy,
	};
	return times_iterator;
}

SGA_Link TimeFrameStream_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	return SGA_StreamGraph_link_by_id(timeframe_stream->underlying_stream_graph, link_id);
}

SGA_TimesIterator TimeFrameStream_key_instants(const SGA_StreamData* stream_data) {
	TimeFrameStream* timeframe_stream = (TimeFrameStream*)stream_data;
	SGA_StreamGraph* stream_graph	  = timeframe_stream->underlying_stream_graph;
	return SGA_StreamGraph_key_instants_between(stream_graph, timeframe_stream->timeframe);
}

const StreamFunctions TimeFrameStream_stream_functions = {
    .nodes_set		= TimeFrameStream_nodes_set,
    .links_set		= TimeFrameStream_links_set,
    .lifespan		= TimeFrameStream_lifespan,
    .time_scale		= TimeFrameStream_time_scale,
    .nodes_present_at_t = TimeFrameStream_nodes_present_at_t,
    .links_present_at_t = TimeFrameStream_links_present_at_t,
    .times_node_present = TimeFrameStream_times_node_present,
    .times_link_present = TimeFrameStream_times_link_present,
    .link_by_id		= TimeFrameStream_link_by_id,
    .neighbours_of_node = TimeFrameStream_neighbours_of_node,
};

size_t TimeFrameStream_distinct_cardinal_of_link_set(const SGA_Stream* stream) {
	SGA_StreamData* stream_data   = stream->stream_data;
	SGA_StreamGraph* stream_graph = ((TimeFrameStream*)stream_data)->underlying_stream_graph;
	return stream_graph->links.nb_links;
}

size_t TimeFrameStream_distinct_cardinal_of_node_set(const SGA_Stream* stream) {
	SGA_StreamData* stream_data   = stream->stream_data;
	SGA_StreamGraph* stream_graph = ((TimeFrameStream*)stream_data)->underlying_stream_graph;
	return stream_graph->nodes.nb_nodes;
}

const MetricsFunctions TimeFrameStream_metrics_functions = {
    .temporal_cardinal_of_node_set = NULL,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = TimeFrameStream_distinct_cardinal_of_node_set,
    .distinct_cardinal_of_link_set = TimeFrameStream_distinct_cardinal_of_link_set,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};

//////////////////////////
//// Weighted version ////
//////////////////////////

SGA_W_Stream SGA_W_TimeFrameStream_from(SGA_W_StreamGraph* stream_graph, SGA_Interval timeframe) {
	W_TimeFrameStream* timeframe_stream = MALLOC(sizeof(W_TimeFrameStream));
	*timeframe_stream		    = (W_TimeFrameStream){
			      .underlying_stream_graph = stream_graph,
			      .timeframe	       = timeframe,
	  };

	SGA_W_Stream stream = {
	    .base	 = {.type = TIMEFRAME_STREAM, .stream_data = &stream_graph->base},
	    .stream_data = timeframe_stream,
	};

	init_cache(&stream.base);

	return stream;
}

void SGA_W_TimeFrameStream_destroy(SGA_W_Stream self) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)self.stream_data;
	free(timeframe_stream);
}

SGA_Weight SGA_W_TimeFrameStream_node_weight_at_t(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Time time) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains(timeframe_stream->timeframe, time));

	return SGA_W_StreamGraph_node_weight_at_t(stream_graph, node, time);
}

SGA_Weight SGA_W_TimeFrameStream_link_weight_at_t(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Time time) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains(timeframe_stream->timeframe, time));

	return SGA_W_StreamGraph_link_weight_at_t(stream_graph, link, time);
}

SGA_Weight SGA_W_TimeFrameStream_weight_integral_of_node_between(const SGA_W_Stream* stream, SGA_NodeId node, SGA_Interval interval) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	ASSERT(node < stream_graph->base.nodes.nb_nodes);
	ASSERT(SGA_Interval_contains_interval(timeframe_stream->timeframe, interval));

	return SGA_W_StreamGraph_weight_integral_of_node_between(stream_graph, node, interval);
}

SGA_Weight SGA_W_TimeFrameStream_weight_integral_of_link_between(const SGA_W_Stream* stream, SGA_LinkId link, SGA_Interval interval) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	ASSERT(link < stream_graph->base.links.nb_links);
	ASSERT(SGA_Interval_contains_interval(timeframe_stream->timeframe, interval));

	return SGA_W_StreamGraph_weight_integral_of_link_between(stream_graph, link, interval);
}

SGA_Weight SGA_W_TimeFrameStream_max_node_weight(const SGA_W_Stream* stream) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	return SGA_WeightFunc_max_in_interval(&stream_graph->node_weights, timeframe_stream->timeframe);
}

SGA_Weight SGA_W_TimeFrameStream_min_node_weight(const SGA_W_Stream* stream) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	return SGA_WeightFunc_min_in_interval(&stream_graph->node_weights, timeframe_stream->timeframe);
}

void SGA_W_TimeFrameStream_normalise_node_weights(SGA_W_Stream* stream) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	SGA_Weight min = SGA_W_TimeFrameStream_min_node_weight(stream);
	SGA_Weight max = SGA_W_TimeFrameStream_max_node_weight(stream);
	SGA_WeightFunc_normalise(&stream_graph->node_weights, min, max);
}

SGA_Weight SGA_W_TimeFrameStream_max_link_weight(const SGA_W_Stream* stream) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	return SGA_WeightFunc_max_in_interval(&stream_graph->link_weights, timeframe_stream->timeframe);
}

SGA_Weight SGA_W_TimeFrameStream_min_link_weight(const SGA_W_Stream* stream) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	return SGA_WeightFunc_min_in_interval(&stream_graph->link_weights, timeframe_stream->timeframe);
}

void SGA_W_TimeFrameStream_normalise_link_weights(SGA_W_Stream* stream) {
	W_TimeFrameStream* timeframe_stream = (W_TimeFrameStream*)stream->stream_data;
	SGA_W_StreamGraph* stream_graph	    = timeframe_stream->underlying_stream_graph;

	SGA_Weight min = SGA_W_TimeFrameStream_min_link_weight(stream);
	SGA_Weight max = SGA_W_TimeFrameStream_max_link_weight(stream);
	SGA_WeightFunc_normalise(&stream_graph->link_weights, min, max);
}

const WeightedStreamFunctions TimeFrameStream_weighted_stream_functions = {
    .node_weight_at_t		     = SGA_W_TimeFrameStream_node_weight_at_t,
    .weight_integral_of_node_between = SGA_W_TimeFrameStream_weight_integral_of_node_between,
    .link_weight_at_t		     = SGA_W_TimeFrameStream_link_weight_at_t,
    .weight_integral_of_link_between = SGA_W_TimeFrameStream_weight_integral_of_link_between,
    .max_node_weight		     = SGA_W_TimeFrameStream_max_node_weight,
    .min_node_weight		     = SGA_W_TimeFrameStream_min_node_weight,
    .normalise_node_weights	     = SGA_W_TimeFrameStream_normalise_node_weights,
    .max_link_weight		     = SGA_W_TimeFrameStream_max_link_weight,
    .min_link_weight		     = SGA_W_TimeFrameStream_min_link_weight,
    .normalise_link_weights	     = SGA_W_TimeFrameStream_normalise_link_weights,
};