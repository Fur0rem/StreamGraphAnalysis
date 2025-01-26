#define SGA_INTERNAL

#include "iterators.h"
#include "interval.h"
#include "units.h"

size_t SGA_total_time_of(SGA_TimesIterator times) {
	size_t total_time = 0;
	SGA_FOR_EACH_TIME(interval, times) {
		total_time += SGA_Interval_duration(interval);
	}
	return total_time;
}

typedef struct {
	SGA_IntervalsSet intervals;
	size_t current_interval;
} SGA_IntervalsIteratorData;

SGA_Interval SGA_IntervalsIterator_next(SGA_TimesIterator* iter) {
	SGA_IntervalsIteratorData* data = (SGA_IntervalsIteratorData*)iter->iterator_data;
	if (data->current_interval >= data->intervals.nb_intervals) {
		return SGA_TIMES_ITERATOR_END;
	}
	return data->intervals.intervals[data->current_interval++];
}

void SGA_IntervalsIterator_destroy(SGA_TimesIterator* iter) {
	SGA_IntervalsIteratorData* data = (SGA_IntervalsIteratorData*)iter->iterator_data;
	SGA_IntervalsSet_destroy(data->intervals);
	free(data);
}

// TODO : very unoptimized implementation
SGA_TimesIterator SGA_TimesIterator_union(SGA_TimesIterator a, SGA_TimesIterator b) {
	// Build an array of intervals
	SGA_IntervalArrayList intervals = SGA_IntervalArrayList_new();
	SGA_FOR_EACH_TIME(interval, a) {
		SGA_IntervalArrayList_push(&intervals, interval);
	}
	SGA_IntervalsSet intervals_set_a = SGA_IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_a.intervals[i] = intervals.array[i];
	}
	SGA_IntervalArrayList_destroy(intervals);

	intervals = SGA_IntervalArrayList_new();
	SGA_FOR_EACH_TIME(interval, b) {
		SGA_IntervalArrayList_push(&intervals, interval);
	}
	SGA_IntervalsSet intervals_set_b = SGA_IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_b.intervals[i] = intervals.array[i];
	}

	SGA_IntervalsSet unioned = SGA_IntervalsSet_union(intervals_set_a, intervals_set_b);

	SGA_IntervalsIteratorData* data = MALLOC(sizeof(SGA_IntervalsIteratorData));
	data->intervals			= unioned;
	data->current_interval		= 0;

	SGA_TimesIterator times = {
	    .stream_graph  = a.stream_graph,
	    .iterator_data = data,
	    .next	   = SGA_IntervalsIterator_next,
	    .destroy	   = SGA_IntervalsIterator_destroy,
	};

	SGA_IntervalArrayList_destroy(intervals);
	SGA_IntervalsSet_destroy(intervals_set_a);
	SGA_IntervalsSet_destroy(intervals_set_b);

	return times;
}

// FIXME: apparently this leaks memory
SGA_TimesIterator SGA_TimesIterator_intersection(SGA_TimesIterator a, SGA_TimesIterator b) {
	// Build an array of intervals
	SGA_IntervalArrayList intervals = SGA_IntervalArrayList_new();
	SGA_FOR_EACH_TIME(interval, a) {
		SGA_IntervalArrayList_push(&intervals, interval);
	}
	SGA_IntervalsSet intervals_set_a = SGA_IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_a.intervals[i] = intervals.array[i];
	}
	SGA_IntervalArrayList_destroy(intervals);

	intervals = SGA_IntervalArrayList_new();
	SGA_FOR_EACH_TIME(interval, b) {
		SGA_IntervalArrayList_push(&intervals, interval);
	}
	SGA_IntervalsSet intervals_set_b = SGA_IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_b.intervals[i] = intervals.array[i];
	}

	SGA_IntervalsSet intersected = SGA_IntervalsSet_intersection(intervals_set_a, intervals_set_b);

	SGA_TimesIterator times = {
	    .stream_graph  = a.stream_graph,
	    .iterator_data = MALLOC(sizeof(SGA_IntervalsIteratorData)),
	    .next	   = SGA_IntervalsIterator_next,
	    .destroy	   = SGA_IntervalsIterator_destroy,
	};

	SGA_IntervalsIteratorData* data = (SGA_IntervalsIteratorData*)times.iterator_data;
	data->intervals			= intersected;
	data->current_interval		= 0;

	SGA_IntervalArrayList_destroy(intervals);
	SGA_IntervalsSet_destroy(intervals_set_a);
	SGA_IntervalsSet_destroy(intervals_set_b);

	return times;
}

size_t SGA_count_nodes(SGA_NodesIterator nodes) {
	return COUNT_ITERATOR(nodes);
}

size_t SGA_count_links(SGA_LinksIterator links) {
	return COUNT_ITERATOR(links);
}

size_t SGA_count_intervals(SGA_TimesIterator times) {
	return COUNT_ITERATOR(times);
}

SGA_IntervalArrayList SGA_collect_times(SGA_TimesIterator times) {
	SGA_IntervalArrayList intervals = SGA_IntervalArrayList_new();
	SGA_FOR_EACH_TIME(interval, times) {
		SGA_IntervalArrayList_push(&intervals, interval);
	}
	return intervals;
}

SGA_NodeIdArrayList SGA_collect_node_ids(SGA_NodesIterator nodes) {
	SGA_NodeIdArrayList node_ids = SGA_NodeIdArrayList_new();
	SGA_FOR_EACH_NODE(node, nodes) {
		SGA_NodeIdArrayList_push(&node_ids, node);
	}
	return node_ids;
}

SGA_LinkIdArrayList SGA_collect_link_ids(SGA_LinksIterator links) {
	SGA_LinkIdArrayList link_ids = SGA_LinkIdArrayList_new();
	SGA_FOR_EACH_LINK(link, links) {
		SGA_LinkIdArrayList_push(&link_ids, link);
	}
	return link_ids;
}

// FIXME: So I made this but never used it apparently
// typedef struct {
// 	SGA_NodesIterator* iter;
// 	bool (*predicate)(SGA_NodeId);
// } FilteredSGA_NodesIteratorData;

// SGA_NodeId SGA_NodesIterator_filtered_next(SGA_NodesIterator* iter) {
// 	FilteredSGA_NodesIteratorData* data = (FilteredSGA_NodesIteratorData*)iter->iterator_data;
// 	SGA_NodeId node			    = data->iter->next(data->iter);
// 	while (node != SGA_NODES_ITERATOR_END) {
// 		if (data->predicate(node)) {
// 			return node;
// 		}
// 		node = data->iter->next(data->iter);
// 	}
// 	return SGA_NODES_ITERATOR_END;
// }

// void SGA_NodesIterator_filtered_destroy(SGA_NodesIterator* iter) {
// 	FilteredSGA_NodesIteratorData* data = (FilteredSGA_NodesIteratorData*)iter->iterator_data;
// 	data->iter->destroy(data->iter);
// 	free(data);
// }

// SGA_NodesIterator SGA_NodesIterator_filtered(SGA_NodesIterator iter, bool (*predicate)(SGA_NodeId)) {
// 	FilteredSGA_NodesIteratorData* data = MALLOC(sizeof(FilteredSGA_NodesIteratorData));
// 	data->iter			    = &iter;
// 	data->predicate			    = predicate;

// 	SGA_NodesIterator filtered = {
// 	    .stream_graph  = iter.stream_graph,
// 	    .iterator_data = data,
// 	    .next	   = SGA_NodesIterator_filtered_next,
// 	    .destroy	   = SGA_NodesIterator_filtered_destroy,
// 	};

// 	return filtered;
// }