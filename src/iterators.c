#include "iterators.h"
#include "interval.h"
#include "units.h"

size_t total_time_of(TimesIterator times) {
	size_t total_time = 0;
	FOR_EACH_TIME(interval, times) {
		total_time += Interval_duration(interval);
	}
	return total_time;
}

typedef struct {
	IntervalsSet intervals;
	size_t current_interval;
} IntervalsIteratorData;

Interval IntervalsIterator_next(TimesIterator* iter) {
	IntervalsIteratorData* data = (IntervalsIteratorData*)iter->iterator_data;
	if (data->current_interval >= data->intervals.nb_intervals) {
		return Interval_from(SIZE_MAX, SIZE_MAX);
	}
	return data->intervals.intervals[data->current_interval++];
}

void IntervalsIterator_destroy(TimesIterator* iter) {
	IntervalsIteratorData* data = (IntervalsIteratorData*)iter->iterator_data;
	IntervalsSet_destroy(data->intervals);
	free(data);
}

// TODO : very unoptimized implementation
TimesIterator TimesIterator_union(TimesIterator a, TimesIterator b) {
	// Build an array of intervals
	IntervalArrayList intervals = IntervalArrayList_new();
	FOR_EACH_TIME(interval, a) {
		IntervalArrayList_push(&intervals, interval);
	}
	IntervalsSet intervals_set_a = IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_a.intervals[i] = intervals.array[i];
	}
	IntervalArrayList_destroy(intervals);

	intervals = IntervalArrayList_new();
	FOR_EACH_TIME(interval, b) {
		IntervalArrayList_push(&intervals, interval);
	}
	IntervalsSet intervals_set_b = IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_b.intervals[i] = intervals.array[i];
	}

	IntervalsSet unioned = IntervalsSet_union(intervals_set_a, intervals_set_b);

	IntervalsIteratorData* data = MALLOC(sizeof(IntervalsIteratorData));
	data->intervals				= unioned;
	data->current_interval		= 0;

	TimesIterator times = {
		.stream_graph  = a.stream_graph,
		.iterator_data = data,
		.next		   = IntervalsIterator_next,
		.destroy	   = IntervalsIterator_destroy,
	};

	IntervalArrayList_destroy(intervals);
	IntervalsSet_destroy(intervals_set_a);
	IntervalsSet_destroy(intervals_set_b);

	return times;
}

// FIXME: apparently this leaks memory
TimesIterator TimesIterator_intersection(TimesIterator a, TimesIterator b) {
	// Build an array of intervals
	IntervalArrayList intervals = IntervalArrayList_new();
	FOR_EACH_TIME(interval, a) {
		IntervalArrayList_push(&intervals, interval);
	}
	IntervalsSet intervals_set_a = IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_a.intervals[i] = intervals.array[i];
	}
	IntervalArrayList_destroy(intervals);

	intervals = IntervalArrayList_new();
	FOR_EACH_TIME(interval, b) {
		IntervalArrayList_push(&intervals, interval);
	}
	IntervalsSet intervals_set_b = IntervalsSet_alloc(intervals.length);
	for (size_t i = 0; i < intervals.length; i++) {
		intervals_set_b.intervals[i] = intervals.array[i];
	}

	IntervalsSet intersected = IntervalsSet_intersection(intervals_set_a, intervals_set_b);

	TimesIterator times = {
		.stream_graph  = a.stream_graph,
		.iterator_data = MALLOC(sizeof(IntervalsIteratorData)),
		.next		   = IntervalsIterator_next,
		.destroy	   = IntervalsIterator_destroy,
	};

	IntervalsIteratorData* data = (IntervalsIteratorData*)times.iterator_data;
	data->intervals				= intersected;
	data->current_interval		= 0;

	IntervalArrayList_destroy(intervals);
	IntervalsSet_destroy(intervals_set_a);
	IntervalsSet_destroy(intervals_set_b);

	return times;
}

size_t count_nodes(NodesIterator nodes) {
	return _COUNT_ITERATOR(NodeId, node, nodes, node != SIZE_MAX);
}

size_t count_links(LinksIterator links) {
	return _COUNT_ITERATOR(LinkId, link, links, link != SIZE_MAX);
}

size_t count_times(TimesIterator times) {
	return _COUNT_ITERATOR(Interval, interval, times, interval.start != SIZE_MAX);
}

IntervalArrayList SGA_collect_times(TimesIterator times) {
	IntervalArrayList intervals = IntervalArrayList_new();
	FOR_EACH_TIME(interval, times) {
		IntervalArrayList_push(&intervals, interval);
	}
	return intervals;
}

NodeIdArrayList SGA_collect_node_ids(NodesIterator nodes) {
	NodeIdArrayList node_ids = NodeIdArrayList_new();
	FOR_EACH_NODE(node, nodes) {
		NodeIdArrayList_push(&node_ids, node);
	}
	return node_ids;
}

LinkIdArrayList SGA_collect_link_ids(LinksIterator links) {
	LinkIdArrayList link_ids = LinkIdArrayList_new();
	FOR_EACH_LINK(link, links) {
		LinkIdArrayList_push(&link_ids, link);
	}
	return link_ids;
}

typedef struct {
	NodesIterator* iter;
	bool (*predicate)(NodeId);
} FilteredNodesIteratorData;

NodeId NodesIterator_filtered_next(NodesIterator* iter) {
	FilteredNodesIteratorData* data = (FilteredNodesIteratorData*)iter->iterator_data;
	NodeId node						= data->iter->next(data->iter);
	while (node != NODES_ITERATOR_END) {
		if (data->predicate(node)) {
			return node;
		}
		node = data->iter->next(data->iter);
	}
	return NODES_ITERATOR_END;
}

void NodesIterator_filtered_destroy(NodesIterator* iter) {
	FilteredNodesIteratorData* data = (FilteredNodesIteratorData*)iter->iterator_data;
	data->iter->destroy(data->iter);
	free(data);
}

NodesIterator NodesIterator_filtered(NodesIterator iter, bool (*predicate)(NodeId)) {
	FilteredNodesIteratorData* data = MALLOC(sizeof(FilteredNodesIteratorData));
	data->iter						= &iter;
	data->predicate					= predicate;

	NodesIterator filtered = {
		.stream_graph  = iter.stream_graph,
		.iterator_data = data,
		.next		   = NodesIterator_filtered_next,
		.destroy	   = NodesIterator_filtered_destroy,
	};

	return filtered;
}