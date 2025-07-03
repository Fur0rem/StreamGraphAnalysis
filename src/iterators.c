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
	return _COUNT_ITERATOR(SGA_NodeId, node, nodes, node != SGA_NODES_ITERATOR_END);
}

size_t SGA_count_links(SGA_LinksIterator links) {
	return _COUNT_ITERATOR(SGA_LinkId, link, links, link != SGA_LINKS_ITERATOR_END);
}

size_t SGA_count_intervals(SGA_TimesIterator times) {
	return _COUNT_ITERATOR(SGA_Interval, interval, times, !SGA_Interval_equals(&interval, &SGA_TIMES_ITERATOR_END));
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

SGA_NodeId empty_nodes_next(__attribute__((unused)) SGA_NodesIterator* iter) {
	return SGA_NODES_ITERATOR_END;
}

void empty_nodes_destroy(__attribute__((unused)) SGA_NodesIterator* iter) {
	// Do nothing
}

SGA_NodesIterator SGA_NodesIterator_empty() {
	return (SGA_NodesIterator){
	    .next    = empty_nodes_next,
	    .destroy = empty_nodes_destroy,
	};
}

SGA_LinkId empty_links_next(__attribute__((unused)) SGA_LinksIterator* iter) {
	return SGA_LINKS_ITERATOR_END;
}

void empty_links_destroy(__attribute__((unused)) SGA_LinksIterator* iter) {
	// Do nothing
}

SGA_LinksIterator SGA_LinksIterator_empty() {
	return (SGA_LinksIterator){
	    .next    = empty_links_next,
	    .destroy = empty_links_destroy,
	};
}

SGA_Interval empty_times_next(__attribute__((unused)) SGA_TimesIterator* iter) {
	return SGA_TIMES_ITERATOR_END;
}

void empty_times_destroy(__attribute__((unused)) SGA_TimesIterator* iter) {
	// Do nothing
}

SGA_TimesIterator SGA_TimesIterator_empty() {
	return (SGA_TimesIterator){
	    .next    = empty_times_next,
	    .destroy = empty_times_destroy,
	};
}