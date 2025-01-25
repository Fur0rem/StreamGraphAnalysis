#define SGA_INTERNAL

#include "key_moments.h"
#include "../utils.h"
#include <stdbool.h>
#include <stddef.h>

// typedef uint8_t RelativeMoment;
// #	define RELATIVE_MOMENT_MAX ((RelativeMoment)~0)
// #	define SLICE_SIZE	    (((size_t)RELATIVE_MOMENT_MAX) + 1)

// typedef struct {
// 	size_t nb_moments;
// 	RelativeMoment* moments;
// } MomentsSlice;

// typedef struct {
// 	size_t current_slice;
// 	size_t current_moment;
// } KeyMomentsTableIterator;

Interval KeyMomentsTableIterator_next(TimesIterator* iter) {
	KeyMomentsTableIterator* key_moments_iter_data = iter->iterator_data;
	SGA_StreamGraph* stream_graph		       = iter->stream_graph.stream_data;

	// Check if you're at the end of the relative moment
	if (key_moments_iter_data->current_moment >= stream_graph->key_moments.slices[key_moments_iter_data->current_slice].nb_moments) {
		key_moments_iter_data->current_moment = 0;
		key_moments_iter_data->current_slice++;
	}

	// Check if you're at the end of the slices
	if (key_moments_iter_data->current_slice >= stream_graph->key_moments.nb_slices) {
		return TIMES_ITERATOR_END;
	}

	// Get the relative moment start
	size_t relative_moment_start =
	    stream_graph->key_moments.slices[key_moments_iter_data->current_slice].moments[key_moments_iter_data->current_moment];
	// Transform it to an absolute moment
	size_t absolute_moment = relative_moment_start + (key_moments_iter_data->current_slice * SLICE_SIZE);

	if (absolute_moment >= stream_graph->lifespan.end) {
		return TIMES_ITERATOR_END;
	}

	// Get the relative moment end
	size_t next_moment = key_moments_iter_data->current_moment + 1;
	size_t next_slice  = key_moments_iter_data->current_slice;
	if (next_moment >= stream_graph->key_moments.slices[next_slice].nb_moments) {
		next_moment = 0;
		next_slice++;
	}
	size_t relative_moment_end;
	size_t absolute_moment_end;

	if (next_slice >= stream_graph->key_moments.nb_slices) {
		relative_moment_end = SLICE_SIZE;
		absolute_moment_end = stream_graph->lifespan.end;
	}
	else {
		relative_moment_end = stream_graph->key_moments.slices[next_slice].moments[next_moment];
		absolute_moment_end = relative_moment_end + (next_slice * SLICE_SIZE);
	}

	// Increment the current moment
	key_moments_iter_data->current_moment++;

	printf("returning %zu %zu\n", absolute_moment, absolute_moment_end);

	return Interval_from(absolute_moment, absolute_moment_end);
}

void KeyMomentsTableIterator_destroy(TimesIterator* iter) {
	free(iter->iterator_data);
}

TimesIterator SGA_StreamGraph_key_moments(SGA_StreamGraph* stream_graph) {
	SGA_Stream stream			       = {.stream_data = stream_graph};
	KeyMomentsTableIterator* key_moments_iter_data = MALLOC(sizeof(KeyMomentsTableIterator));
	*key_moments_iter_data			       = (KeyMomentsTableIterator){
				    .current_slice  = 0,
				    .current_moment = 0,
	};

	TimesIterator key_moments_iter = {
	    .stream_graph  = stream,
	    .iterator_data = key_moments_iter_data,
	    .next	   = KeyMomentsTableIterator_next,
	    .destroy	   = KeyMomentsTableIterator_destroy,
	};

	return key_moments_iter;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
	size_t current_moment;
	size_t current_slice;
	Interval interval;
} KeyMomentsTableBetweenIterator;

Interval KeyMomentsTableIterator_next_between(TimesIterator* iter) {
	KeyMomentsTableBetweenIterator* key_moments_iter_data = iter->iterator_data;
	Interval interval				      = key_moments_iter_data->interval;
	SGA_StreamGraph* stream_graph			      = iter->stream_graph.stream_data;

	// Check if you're at the end of the relative moment
	if (key_moments_iter_data->current_moment >= stream_graph->key_moments.slices[key_moments_iter_data->current_slice].nb_moments) {
		key_moments_iter_data->current_moment = 0;
		key_moments_iter_data->current_slice++;
	}

	// Check if you're at the end of the slices or the interval
	if (key_moments_iter_data->current_slice >= stream_graph->key_moments.nb_slices) {
		printf("reached end of slices\n");
		return TIMES_ITERATOR_END;
	}

	// Get the relative moment start
	size_t relative_moment_start =
	    stream_graph->key_moments.slices[key_moments_iter_data->current_slice].moments[key_moments_iter_data->current_moment];
	// Transform it to an absolute moment
	size_t absolute_moment = relative_moment_start + (key_moments_iter_data->current_slice * SLICE_SIZE);
	// Truncate the moment to the interval
	absolute_moment = MAX(absolute_moment, interval.start);

	// Check if the moment is in the interval
	if (!Interval_contains(interval, absolute_moment)) {
		return TIMES_ITERATOR_END;
	}

	// Get the relative moment end
	size_t next_moment = key_moments_iter_data->current_moment + 1;
	size_t next_slice  = key_moments_iter_data->current_slice;
	if (next_moment >= stream_graph->key_moments.slices[next_slice].nb_moments) {
		next_moment = 0;
		next_slice++;
	}
	size_t relative_moment_end;
	size_t absolute_moment_end;

	if (next_slice >= stream_graph->key_moments.nb_slices) {
		relative_moment_end = SLICE_SIZE;
		absolute_moment_end = stream_graph->lifespan.end;
	}
	else {
		relative_moment_end = stream_graph->key_moments.slices[next_slice].moments[next_moment];
		absolute_moment_end = relative_moment_end + (next_slice * SLICE_SIZE);
	}

	// Truncate the moment end to the interval
	absolute_moment_end = MIN(absolute_moment_end, interval.end);

	// Increment the current moment
	key_moments_iter_data->current_moment++;

	return Interval_from(absolute_moment, absolute_moment_end);
}

TimesIterator SGA_StreamGraph_key_moments_between(SGA_StreamGraph* stream_graph, Interval interval) {
	SGA_Stream stream				      = {.stream_data = stream_graph};
	KeyMomentsTableBetweenIterator* key_moments_iter_data = MALLOC(sizeof(KeyMomentsTableBetweenIterator));

	// Find where you need to start
	size_t index	       = KeyMomentsTable_find_time_index(&stream_graph->key_moments, interval.start);
	*key_moments_iter_data = (KeyMomentsTableBetweenIterator){
	    .current_slice  = index / SLICE_SIZE,
	    .current_moment = index % SLICE_SIZE,
	    .interval	    = interval,
	};

	TimesIterator key_moments_iter = {
	    .stream_graph  = stream,
	    .iterator_data = key_moments_iter_data,
	    .next	   = KeyMomentsTableIterator_next_between,
	    .destroy	   = KeyMomentsTableIterator_destroy,
	};

	return key_moments_iter;
}
