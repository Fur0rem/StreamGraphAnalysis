#define SGA_INTERNAL

#include "delta_stream.h"
#include "../analysis/metrics.h"
#include "../stream_data_access/induced_graph.h"
#include "../stream_data_access/key_instants.h"
#include "../stream_data_access/link_access.h"
#include "../stream_data_access/node_access.h"
#include "../units.h"
#include "../utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

SGA_Stream SGA_DeltaStream_from(SGA_StreamGraph* stream_graph, SGA_Time delta) {
	DeltaStream* delta_stream	      = MALLOC(sizeof(DeltaStream));
	delta_stream->underlying_stream_graph = stream_graph;
	delta_stream->delta		      = delta;
	SGA_Stream stream		      = {.type = DELTA_STREAM, .stream_data = delta_stream};
	init_cache(&stream);
	return stream;
}

void SGA_DeltaStream_destroy(SGA_Stream stream) {
	free(stream.stream_data);
}

SGA_NodesIterator DeltaStream_nodes_set(SGA_StreamData* stream_data) {
	DeltaStream* delta_stream     = (DeltaStream*)stream_data;
	SGA_StreamGraph* stream_graph = delta_stream->underlying_stream_graph;
	return SGA_StreamGraph_nodes_set(stream_graph);
}

SGA_Interval DeltaStream_lifespan(SGA_StreamData* stream_data) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;
	return delta_stream->underlying_stream_graph->lifespan;
}

size_t DeltaStream_time_scale(SGA_StreamData* stream_data) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;
	return delta_stream->underlying_stream_graph->time_scale;
}

/**
 * @brief Data structure to iterate over the IDs of nodes/links in a DeltaStream.
 */
typedef struct {
	size_tHashset ids;	      ///< A hashset of IDs of nodes/links to iterate over
	size_t current_bucket;	      ///< The current bucket in the hashset to iterate over
	size_t current_idx_in_bucket; ///< The current index in the current bucket
} DeltaStreamIDsIteratorData;

/**
 * @brief Get the current node ID in the DeltaStream nodes iterator and move to the next one.
 */
size_t DeltaStream_nodes_present_at_t_next(SGA_NodesIterator* iterator) {
	DeltaStreamIDsIteratorData* data = (DeltaStreamIDsIteratorData*)iterator->iterator_data;

	// If we have reached the end of the hashset, return SGA_NODES_ITERATOR_END
	if (data->current_bucket >= data->ids.capacity) {
		return SGA_NODES_ITERATOR_END;
	}

	// If we have reached the end of the current bucket, move to the next bucket
	while (data->current_idx_in_bucket >= data->ids.buckets[data->current_bucket].length) {
		data->current_bucket++;
		data->current_idx_in_bucket = 0;

		// If we have reached the end of the hashset, return SGA_NODES_ITERATOR_END
		if (data->current_bucket >= data->ids.capacity) {
			return SGA_NODES_ITERATOR_END;
		}
	}

	// Get the current ID from the hashset
	size_t node_id = data->ids.buckets[data->current_bucket].array[data->current_idx_in_bucket];

	// Increment the index in the current bucket
	data->current_idx_in_bucket++;

	return node_id;
}

/**
 * @brief Destroy the DeltaStream nodes iterator.
 * This function frees the memory allocated for the iterator data.
 */
void DeltaStream_nodes_present_at_t_destroy(SGA_NodesIterator* iterator) {
	DeltaStreamIDsIteratorData* data = (DeltaStreamIDsIteratorData*)iterator->iterator_data;
	size_tHashset_destroy(data->ids);
	free(data);
}

/**
 * @brief Saturate the subtraction of two SGA_Time values, ensuring it does not go below a minimum value or underflow.
 * @param subtracted The value to be subtracted from.
 * @param to_subtract The value to subtract.
 * @param min_value The minimum value to return if the subtraction would underflow or go below this value.
 * @return The result of the subtraction, saturated to the minimum value if necessary.
 */
SGA_Time saturate_sub(SGA_Time subtracted, SGA_Time to_subtract, SGA_Time min_value) {
	bool will_number_underflow = subtracted < to_subtract;
	if (will_number_underflow) {
		return min_value;
	}
	SGA_Time result = subtracted - to_subtract;
	if (result < min_value) {
		return min_value;
	}
	return result;
}

/**
 * @brief Saturate the addition of two SGA_Time values, ensuring it does not exceed a maximum value or overflow.
 * @param added The value to which the other value is added.
 * @param to_add The value to add.
 * @param max_value The maximum value to return if the addition would overflow or exceed this value.
 */
SGA_Time saturate_add(SGA_Time added, SGA_Time to_add, SGA_Time max_value) {
	SGA_Time result		  = added + to_add;
	bool will_number_overflow = result < added; // Check if overflow occurred
	if (will_number_overflow || result > max_value) {
		return max_value;
	}
	return result;
}

/**
 * @brief Get an iterator over the nodes present at a specific time in a DeltaStream.
 * This function collects all node IDs that are present in the delta time frame around the given instant.
 *
 * @param stream_data The DeltaStream data.
 * @param instant The time instant to check for node presence.
 * @return An iterator over the node IDs present at the specified time.
 */
SGA_NodesIterator DeltaStream_nodes_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;
	// Delta time frame, use +1 to round up in case of odd delta, because stream at x+0.5 is the same as stream at x
	SGA_Time half_delta   = (delta_stream->delta + 1) / 2;
	SGA_Interval lifespan = delta_stream->underlying_stream_graph->lifespan;

	// If the instant is not in the lifespan, return an empty iterator
	if (!SGA_Interval_contains(lifespan, instant)) {
		return SGA_NodesIterator_empty();
	}

	// Collect all node IDs that are present in the delta time frame
	size_tHashset ids = size_tHashset_new();

	// Calculate the frame of the delta time (and saturate it to the lifespan)
	SGA_Time start_time = saturate_sub(instant, half_delta, lifespan.start);
	SGA_Time end_time   = saturate_add(instant, half_delta, lifespan.end);

	// Find the beginning of the delta time frame in the key instants table
	size_t current_key_instant_idx =
	    KeyInstantsTable_find_time_index_equivalent(&delta_stream->underlying_stream_graph->key_instants, start_time);
	SGA_Time current_time =
	    KeyInstantsTable_nth_key_instant(&delta_stream->underlying_stream_graph->key_instants, current_key_instant_idx);

	// Iterate over the key instants until we reach the end time, and add all nodes present at those instants to the hashset
	while (current_time <= end_time) {
		SGA_NodesIterator nodes_present = SGA_StreamGraph_nodes_present_at(delta_stream->underlying_stream_graph, current_time);
		SGA_FOR_EACH_NODE(node, nodes_present) {
			size_tHashset_insert(&ids, node);
		}
		current_key_instant_idx++;
		current_time =
		    KeyInstantsTable_nth_key_instant(&delta_stream->underlying_stream_graph->key_instants, current_key_instant_idx);
	}
	current_time			= end_time;
	SGA_NodesIterator nodes_present = SGA_StreamGraph_nodes_present_at(delta_stream->underlying_stream_graph, current_time);
	SGA_FOR_EACH_NODE(node, nodes_present) {
		size_tHashset_insert(&ids, node);
	}

	SGA_NodesIterator nodes_iterator = {
	    .stream_graph  = (SGA_Stream){.type = DELTA_STREAM, .stream_data = delta_stream},
	    .iterator_data = MALLOC(sizeof(DeltaStreamIDsIteratorData)),
	    .next	   = DeltaStream_nodes_present_at_t_next,
	    .destroy	   = DeltaStream_nodes_present_at_t_destroy,
	};

	DeltaStreamIDsIteratorData* data = (DeltaStreamIDsIteratorData*)nodes_iterator.iterator_data;
	data->ids			 = ids;
	data->current_bucket		 = 0;
	data->current_idx_in_bucket	 = 0;

	return nodes_iterator;
}

/**
 * @brief Get an iterator over the links present at a specific time in a DeltaStream.
 * This function collects all link IDs that are present in the delta time frame around the given instant.
 *
 * @param stream_data The DeltaStream data.
 * @param instant The time instant to check for link presence.
 * @return An iterator over the link IDs present at the specified time.
 */
SGA_LinksIterator DeltaStream_links_present_at_t(SGA_StreamData* stream_data, SGA_Time instant) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;
	// Delta time frame, use +1 to round up in case of odd delta, because stream at x+0.5 is the same as stream at x
	SGA_Time half_delta   = (delta_stream->delta + 1) / 2;
	SGA_Interval lifespan = delta_stream->underlying_stream_graph->lifespan;

	// If the instant is not in the lifespan, return an empty iterator
	if (!SGA_Interval_contains(lifespan, instant)) {
		return SGA_LinksIterator_empty();
	}

	// Collect all link IDs that are present in the delta time frame
	size_tHashset ids = size_tHashset_new();

	// Calculate the start time of the delta time frame (0 if half_delta is before the start of the lifespan)
	bool will_underflow	    = instant < half_delta;
	SGA_Time start_time	    = will_underflow ? 0 : instant - half_delta;
	bool starts_before_lifespan = SGA_Interval_contains(lifespan, start_time);
	start_time		    = starts_before_lifespan ? start_time : lifespan.start;

	// Calculate the end time of the delta time frame (end of lifespan if half_delta is after the end of the lifespan)
	bool will_overflow = instant + half_delta > lifespan.end;
	SGA_Time end_time  = will_overflow ? lifespan.end : instant + half_delta;

	// Iterate over all links and check if they are present in the delta time frame
	size_t current_key_instant_idx =
	    KeyInstantsTable_find_time_index_equivalent(&delta_stream->underlying_stream_graph->key_instants, start_time);
	SGA_Time current_time =
	    KeyInstantsTable_nth_key_instant(&delta_stream->underlying_stream_graph->key_instants, current_key_instant_idx);

	// Iterate over the key instants until we reach the end time, and add all links present at those instants to the hashset
	while (current_time <= end_time) {
		SGA_LinksIterator links_present = SGA_StreamGraph_links_present_at(delta_stream->underlying_stream_graph, current_time);
		SGA_FOR_EACH_NODE(link, links_present) {
			size_tHashset_insert(&ids, link);
		}
		current_key_instant_idx++;
		current_time =
		    KeyInstantsTable_nth_key_instant(&delta_stream->underlying_stream_graph->key_instants, current_key_instant_idx);
	}
	current_time			= end_time;
	SGA_LinksIterator links_present = SGA_StreamGraph_links_present_at(delta_stream->underlying_stream_graph, current_time);
	SGA_FOR_EACH_NODE(link, links_present) {
		size_tHashset_insert(&ids, link);
	}

	// NodesIterator and LinksIterator are hopefully interchangeable, so we can use the same iterator structure
	SGA_LinksIterator links_iterator = {
	    .stream_graph  = (SGA_Stream){.type = DELTA_STREAM, .stream_data = delta_stream},
	    .iterator_data = MALLOC(sizeof(DeltaStreamIDsIteratorData)),
	    .next	   = (SGA_LinkId(*)(SGA_LinksIterator*))DeltaStream_nodes_present_at_t_next,
	    .destroy	   = (void (*)(SGA_LinksIterator*))DeltaStream_nodes_present_at_t_destroy,
	};

	DeltaStreamIDsIteratorData* data = (DeltaStreamIDsIteratorData*)links_iterator.iterator_data;
	data->ids			 = ids;
	data->current_bucket		 = 0;
	data->current_idx_in_bucket	 = 0;

	return links_iterator;
}

typedef struct {
	size_t current_time;	    ///< The current index of the time intervals being iterated.
	SGA_IntervalsSet intervals; ///< The array list of intervals for the node or link being iterated.
} DeltaStreamTimesIdPresentAtIteratorData;

SGA_Interval DeltaStream_TimesIdPresentAt_next(SGA_TimesIterator* iter) {
	DeltaStreamTimesIdPresentAtIteratorData* times_iter_data = (DeltaStreamTimesIdPresentAtIteratorData*)iter->iterator_data;
	SGA_IntervalsSet intervals_set				 = times_iter_data->intervals;

	if (times_iter_data->current_time >= intervals_set.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}

	SGA_Interval nth_time = intervals_set.intervals[times_iter_data->current_time];
	times_iter_data->current_time++;

	return nth_time;
}

void DeltaStream_TimesIdPresentAtIterator_destroy(SGA_TimesIterator* iterator) {
	DeltaStreamTimesIdPresentAtIteratorData* times_iter_data = (DeltaStreamTimesIdPresentAtIteratorData*)iterator->iterator_data;
	SGA_IntervalsSet_destroy(times_iter_data->intervals);
	free(times_iter_data);
}

SGA_TimesIterator DeltaStream_times_node_present(SGA_StreamData* stream_data, SGA_NodeId node_id) {
	// printf("Entering DeltaStream_times_node_present\n");
	DeltaStream* delta_stream     = (DeltaStream*)stream_data;
	SGA_StreamGraph* stream_graph = delta_stream->underlying_stream_graph;
	SGA_Time half_delta	      = (delta_stream->delta + 1) / 2;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	SGA_IntervalArrayList intervals = SGA_IntervalArrayList_new();
	SGA_Node node			= stream_graph->nodes.nodes[node_id];
	for (size_t i = 0; i < node.presence.nb_intervals; i++) {
		// Get the interval
		SGA_Interval interval = node.presence.intervals[i];

		// Add the delta time frame to the interval
		interval.start = saturate_sub(interval.start, half_delta, stream_graph->lifespan.start);
		interval.end   = saturate_add(interval.end, half_delta, stream_graph->lifespan.end);

		// Add the interval to the list
		SGA_IntervalArrayList_push(&intervals, interval);
	}

	// Merge the intervals to remove overlaps
	SGA_IntervalsSet intervals_set = SGA_IntervalsSet_from_arraylist(intervals);
	SGA_IntervalsSet_merge(&intervals_set);

	// Create the iterator data
	DeltaStreamTimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(DeltaStreamTimesIdPresentAtIteratorData));
	*iterator_data					       = (DeltaStreamTimesIdPresentAtIteratorData){
						    .current_time = 0,
						    .intervals	  = intervals_set,
	};
	SGA_Stream stream		 = {.type = DELTA_STREAM, .stream_data = delta_stream};
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = DeltaStream_TimesIdPresentAt_next,
	    .destroy	   = DeltaStream_TimesIdPresentAtIterator_destroy,
	};
	return times_iterator;
}

SGA_TimesIterator DeltaStream_times_link_present(SGA_StreamData* stream_data, SGA_LinkId link_id) {
	DeltaStream* delta_stream     = (DeltaStream*)stream_data;
	SGA_StreamGraph* stream_graph = delta_stream->underlying_stream_graph;
	SGA_Time half_delta	      = (delta_stream->delta + 1) / 2;

	ASSERT(link_id < stream_graph->links.nb_links);

	SGA_IntervalArrayList intervals = SGA_IntervalArrayList_new();
	SGA_Link link			= stream_graph->links.links[link_id];
	for (size_t i = 0; i < link.presence.nb_intervals; i++) {
		// Get the interval
		SGA_Interval interval = link.presence.intervals[i];

		// Add the delta time frame to the interval
		interval.start = saturate_sub(interval.start, half_delta, stream_graph->lifespan.start);
		interval.end   = saturate_add(interval.end, half_delta, stream_graph->lifespan.end);

		// Add the interval to the list
		SGA_IntervalArrayList_push(&intervals, interval);
	}

	// Merge the intervals to remove overlaps
	SGA_IntervalsSet intervals_set = SGA_IntervalsSet_from_arraylist(intervals);
	SGA_IntervalsSet_merge(&intervals_set);

	// Create the iterator data
	DeltaStreamTimesIdPresentAtIteratorData* iterator_data = MALLOC(sizeof(DeltaStreamTimesIdPresentAtIteratorData));
	*iterator_data					       = (DeltaStreamTimesIdPresentAtIteratorData){
						    .current_time = 0,
						    .intervals	  = intervals_set,
	};
	SGA_Stream stream		 = {.type = DELTA_STREAM, .stream_data = delta_stream};
	SGA_TimesIterator times_iterator = {
	    .stream_graph  = stream,
	    .iterator_data = iterator_data,
	    .next	   = DeltaStream_TimesIdPresentAt_next,
	    .destroy	   = DeltaStream_TimesIdPresentAtIterator_destroy,
	};
	return times_iterator;
}

SGA_Link DeltaStream_link_by_id(SGA_StreamData* stream_data, size_t link_id) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;

	ASSERT(link_id < delta_stream->underlying_stream_graph->links.nb_links);

	return delta_stream->underlying_stream_graph->links.links[link_id];
}

SGA_LinksIterator DeltaStream_neighbours_of_node(SGA_StreamData* stream_data, SGA_NodeId node_id) {
	DeltaStream* delta_stream     = (DeltaStream*)stream_data;
	SGA_StreamGraph* stream_graph = delta_stream->underlying_stream_graph;

	ASSERT(node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_neighbours_of_node(stream_graph, node_id);
}

SGA_Node DeltaStream_node_by_id(SGA_StreamData* stream_data, size_t node_id) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;

	ASSERT(node_id < delta_stream->underlying_stream_graph->nodes.nb_nodes);

	return delta_stream->underlying_stream_graph->nodes.nodes[node_id];
}

SGA_LinksIterator DeltaStream_links_set(SGA_StreamData* stream_data) {
	DeltaStream* delta_stream = (DeltaStream*)stream_data;
	return SGA_StreamGraph_links_set(delta_stream->underlying_stream_graph);
}

SGA_LinkId DeltaStream_link_between_nodes(SGA_StreamData* stream_data, SGA_NodeId node_id, SGA_NodeId other_node_id) {
	DeltaStream* delta_stream     = (DeltaStream*)stream_data;
	SGA_StreamGraph* stream_graph = delta_stream->underlying_stream_graph;

	ASSERT(node_id != other_node_id);
	ASSERT(node_id < stream_graph->nodes.nb_nodes);
	ASSERT(other_node_id < stream_graph->nodes.nb_nodes);

	return SGA_StreamGraph_link_between_nodes(stream_graph, node_id, other_node_id);
}

SGA_TimesIterator DeltaStream_key_instants(SGA_StreamData* stream_data) {
	DeltaStream* link_stream      = (DeltaStream*)stream_data;
	SGA_StreamGraph* stream_graph = link_stream->underlying_stream_graph;

	return SGA_StreamGraph_key_instants(stream_graph);
}

const StreamFunctions DeltaStream_stream_functions = {
    .nodes_set		= DeltaStream_nodes_set,
    .links_set		= DeltaStream_links_set,
    .lifespan		= DeltaStream_lifespan,
    .time_scale		= DeltaStream_time_scale,
    .nodes_present_at_t = DeltaStream_nodes_present_at_t,
    .links_present_at_t = DeltaStream_links_present_at_t,
    .times_node_present = DeltaStream_times_node_present,
    .times_link_present = DeltaStream_times_link_present,
    .link_by_id		= DeltaStream_link_by_id,
    .node_by_id		= DeltaStream_node_by_id,
    .neighbours_of_node = DeltaStream_neighbours_of_node,
    .link_between_nodes = DeltaStream_link_between_nodes,
    .key_instants	= DeltaStream_key_instants,
};

size_t DeltaStream_distinct_cardinal_of_node_set(SGA_Stream* stream) {
	SGA_StreamData* stream_data = stream->stream_data;
	DeltaStream* delta_stream   = (DeltaStream*)stream_data;
	return delta_stream->underlying_stream_graph->nodes.nb_nodes;
}

size_t DeltaStream_distinct_cardinal_of_link_set(SGA_Stream* stream) {
	SGA_StreamData* stream_data = stream->stream_data;
	DeltaStream* delta_stream   = (DeltaStream*)stream_data;
	return delta_stream->underlying_stream_graph->links.nb_links;
}

const MetricsFunctions DeltaStream_metrics_functions = {
    .temporal_cardinal_of_node_set = NULL,
    .duration			   = NULL,
    .distinct_cardinal_of_node_set = (size_t (*)(const SGA_Stream*))DeltaStream_distinct_cardinal_of_node_set,
    .distinct_cardinal_of_link_set = (size_t (*)(const SGA_Stream*))DeltaStream_distinct_cardinal_of_link_set,
    .coverage			   = NULL,
    .node_duration		   = NULL,
};