#define SGA_INTERNAL

#include "../utils.h"
#include "key_instants.h"
#include <stdbool.h>
#include <stddef.h>

// typedef uint8_t RelativeInstant;
// #	define RELATIVE_INSTANT_MAX ((RelativeInstant)~0)
// #	define SLICE_SIZE	    (((size_t)RELATIVE_INSTANT_MAX) + 1)

// typedef struct {
// 	size_t nb_instants;
// 	RelativeInstant* instants;
// } InstantsSlice;

// typedef struct {
// 	size_t current_slice;
// 	size_t current_instant;
// } KeyInstantsTableIterator;

SGA_Interval KeyInstantsTableIterator_next(SGA_TimesIterator* iter) {
	KeyInstantsTableIterator* key_instants_iter_data = iter->iterator_data;
	SGA_StreamGraph* stream_graph			 = iter->stream_graph.stream_data;

	// Check if you're at the end of the relative instant
	if (key_instants_iter_data->current_instant >=
	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants) {
		key_instants_iter_data->current_instant = 0;
		key_instants_iter_data->current_slice++;
	}

	// Check if you're at the end of the slices
	if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
		return SGA_TIMES_ITERATOR_END;
	}

	// Get the relative instant start
	size_t relative_instant_start =
	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[key_instants_iter_data->current_instant];
	// Transform it to an absolute instant
	size_t absolute_instant = relative_instant_start + (key_instants_iter_data->current_slice * SLICE_SIZE);

	if (absolute_instant >= stream_graph->lifespan.end) {
		return SGA_TIMES_ITERATOR_END;
	}

	// Get the relative instant end
	size_t next_instant = key_instants_iter_data->current_instant + 1;
	size_t next_slice   = key_instants_iter_data->current_slice;
	if (next_instant >= stream_graph->key_instants.slices[next_slice].nb_instants) {
		next_instant = 0;
		next_slice++;
	}
	size_t relative_instant_end;
	size_t absolute_instant_end;

	if (next_slice >= stream_graph->key_instants.nb_slices) {
		relative_instant_end = SLICE_SIZE;
		absolute_instant_end = stream_graph->lifespan.end;
	}
	else {
		relative_instant_end = stream_graph->key_instants.slices[next_slice].instants[next_instant];
		absolute_instant_end = relative_instant_end + (next_slice * SLICE_SIZE);
	}

	// Increment the current instant
	key_instants_iter_data->current_instant++;

	printf("returning %zu %zu\n", absolute_instant, absolute_instant_end);

	return SGA_Interval_from(absolute_instant, absolute_instant_end);
}

void KeyInstantsTableIterator_destroy(SGA_TimesIterator* iter) {
	free(iter->iterator_data);
}

SGA_TimesIterator SGA_StreamGraph_key_instants(SGA_StreamGraph* stream_graph) {
	SGA_Stream stream				 = {.stream_data = stream_graph};
	KeyInstantsTableIterator* key_instants_iter_data = MALLOC(sizeof(KeyInstantsTableIterator));
	*key_instants_iter_data				 = (KeyInstantsTableIterator){
				     .current_slice   = 0,
				     .current_instant = 0,
	 };

	SGA_TimesIterator key_instants_iter = {
	    .stream_graph  = stream,
	    .iterator_data = key_instants_iter_data,
	    .next	   = KeyInstantsTableIterator_next,
	    .destroy	   = KeyInstantsTableIterator_destroy,
	};

	return key_instants_iter;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
	size_t current_instant;
	size_t current_slice;
	SGA_Interval interval;
} KeyInstantsTableBetweenIterator;

SGA_Interval KeyInstantsTableIterator_next_between(SGA_TimesIterator* iter) {
	KeyInstantsTableBetweenIterator* key_instants_iter_data = iter->iterator_data;
	SGA_Interval interval					= key_instants_iter_data->interval;
	SGA_StreamGraph* stream_graph				= iter->stream_graph.stream_data;

	// Check if you're at the end of the relative instant
	if (key_instants_iter_data->current_instant >=
	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants) {
		key_instants_iter_data->current_instant = 0;
		key_instants_iter_data->current_slice++;
	}

	// Check if you're at the end of the slices or the interval
	if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
		printf("reached end of slices\n");
		return SGA_TIMES_ITERATOR_END;
	}

	// Get the relative instant start
	size_t relative_instant_start =
	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[key_instants_iter_data->current_instant];
	// Transform it to an absolute instant
	size_t absolute_instant = relative_instant_start + (key_instants_iter_data->current_slice * SLICE_SIZE);
	// Truncate the instant to the interval
	absolute_instant = MAX(absolute_instant, interval.start);

	// Check if the instant is in the interval
	if (!SGA_Interval_contains(interval, absolute_instant)) {
		return SGA_TIMES_ITERATOR_END;
	}

	// Get the relative instant end
	size_t next_instant = key_instants_iter_data->current_instant + 1;
	size_t next_slice   = key_instants_iter_data->current_slice;
	if (next_instant >= stream_graph->key_instants.slices[next_slice].nb_instants) {
		next_instant = 0;
		next_slice++;
	}
	size_t relative_instant_end;
	size_t absolute_instant_end;

	if (next_slice >= stream_graph->key_instants.nb_slices) {
		relative_instant_end = SLICE_SIZE;
		absolute_instant_end = stream_graph->lifespan.end;
	}
	else {
		relative_instant_end = stream_graph->key_instants.slices[next_slice].instants[next_instant];
		absolute_instant_end = relative_instant_end + (next_slice * SLICE_SIZE);
	}

	// Truncate the instant end to the interval
	absolute_instant_end = MIN(absolute_instant_end, interval.end);

	// Increment the current instant
	key_instants_iter_data->current_instant++;

	return SGA_Interval_from(absolute_instant, absolute_instant_end);
}

SGA_TimesIterator SGA_StreamGraph_key_instants_between(SGA_StreamGraph* stream_graph, SGA_Interval interval) {
	SGA_Stream stream					= {.stream_data = stream_graph};
	KeyInstantsTableBetweenIterator* key_instants_iter_data = MALLOC(sizeof(KeyInstantsTableBetweenIterator));

	// Find where you need to start
	size_t index		= KeyInstantsTable_find_time_index_equivalent(&stream_graph->key_instants, interval.start);
	*key_instants_iter_data = (KeyInstantsTableBetweenIterator){
	    .current_slice   = index / SLICE_SIZE,
	    .current_instant = index % SLICE_SIZE,
	    .interval	     = interval,
	};

	SGA_TimesIterator key_instants_iter = {
	    .stream_graph  = stream,
	    .iterator_data = key_instants_iter_data,
	    .next	   = KeyInstantsTableIterator_next_between,
	    .destroy	   = KeyInstantsTableIterator_destroy,
	};

	return key_instants_iter;
}
