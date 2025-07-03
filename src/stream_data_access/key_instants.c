#define SGA_INTERNAL

#include "key_instants.h"
#include "../utils.h"
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

// SGA_Interval KeyInstantsTableIterator_next(SGA_TimesIterator* iter) {
// 	KeyInstantsTableIterator* key_instants_iter_data = iter->iterator_data;
// 	SGA_StreamGraph* stream_graph			 = iter->stream_graph.stream_data;

// 	// If you just started, return lifespan start up until the first instant
// 	if (key_instants_iter_data->current_slice == 0 && key_instants_iter_data->current_instant == 0) {
// 		printf("starting iterator, returning lifespan start\n");
// 		SGA_TimeId lifespan_start = stream_graph->lifespan.start;
// 		// Find first instant
// 		while (key_instants_iter_data->current_slice < stream_graph->key_instants.nb_slices &&
// 		       stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants == 0) {
// 			printf("current slice %zu has no instants, incrementing\n", key_instants_iter_data->current_slice);
// 			key_instants_iter_data->current_slice++;
// 			key_instants_iter_data->current_instant = 0;
// 		}
// 		if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
// 			printf("current slice %zu is beyond the end of the slices\n", key_instants_iter_data->current_slice);
// 			return SGA_TIMES_ITERATOR_END;
// 		}
// 		size_t relative_instant_start = stream_graph->key_instants.slices[key_instants_iter_data->current_slice]
// 						    .instants[key_instants_iter_data->current_instant];
// 		// Transform it to an absolute instant
// 		size_t absolute_instant = relative_instant_start + (key_instants_iter_data->current_slice * SLICE_SIZE);
// 		if (absolute_instant >= stream_graph->lifespan.end) {
// 			printf("absolute instant %zu is beyond lifespan end %zu\n", absolute_instant, stream_graph->lifespan.end);
// 			return SGA_TIMES_ITERATOR_END;
// 		}
// 		printf("returning lifespan start %zu up until first instant %zu\n", lifespan_start, absolute_instant);
// 		return SGA_Interval_from(lifespan_start, absolute_instant);
// 	}

// 	printf("current slice: %zu, current instant: %u\n", key_instants_iter_data->current_slice, key_instants_iter_data->current_instant);

// 	// get the current instant
// 	if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
// 		printf("current slice %zu is beyond the end of the slices\n", key_instants_iter_data->current_slice);
// 		return SGA_TIMES_ITERATOR_END;
// 	}

// 	if (key_instants_iter_data->current_instant >=
// 	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants) {
// 		printf("current instant %zu is beyond the end of the instants in slice %zu\n",
// 		       key_instants_iter_data->current_instant,
// 		       key_instants_iter_data->current_slice);
// 		return SGA_TIMES_ITERATOR_END;
// 	}

// 	SGA_TimeId current_event =
// 	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[key_instants_iter_data->current_instant];

// 	// Check if you're at the end of the relative instant
// 	if (key_instants_iter_data->current_instant >=
// 	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants) {
// 		printf("reached end of relative instant\n");
// 		key_instants_iter_data->current_instant = 0;
// 		key_instants_iter_data->current_slice++;
// 	}

// 	// Check if you're at the end of the slices
// 	if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
// 		printf("reached end of slices\n");
// 		return SGA_TIMES_ITERATOR_END;
// 	}

// 	// Get the relative instant start
// 	printf("%p\n%p\n%p\n", stream_graph, &stream_graph->key_instants, &stream_graph->key_instants.slices);
// 	printf("key_instants_iter_data: %p\n", key_instants_iter_data);
// 	printf("key_instants_iter_data->current_slice: %zu\n", key_instants_iter_data->current_slice);
// 	printf("%zu\n", stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants);
// 	// if (stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants == NULL) {
// 	// 	printf("Error: instants pointer for slice %zu is null\n", key_instants_iter_data->current_slice);
// 	// 	return SGA_TIMES_ITERATOR_END; // Handle the error
// 	// }

// 	while (key_instants_iter_data->current_slice < stream_graph->key_instants.nb_slices &&
// 	       stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants == 0) {
// 		printf("current slice %zu has no instants, incrementing\n", key_instants_iter_data->current_slice);
// 		key_instants_iter_data->current_slice++;
// 		key_instants_iter_data->current_instant = 0;
// 	}
// 	if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
// 		printf("current slice %zu is beyond the end of the slices\n", key_instants_iter_data->current_slice);
// 		return SGA_TIMES_ITERATOR_END;
// 	}

// 	size_t relative_instant_start =
// 	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[key_instants_iter_data->current_instant];
// 	// Transform it to an absolute instant
// 	size_t absolute_instant = relative_instant_start + (key_instants_iter_data->current_slice * SLICE_SIZE);

// 	if (absolute_instant >= stream_graph->lifespan.end) {
// 		printf("absolute instant %zu is beyond lifespan end %zu\n", absolute_instant, stream_graph->lifespan.end);
// 		return SGA_TIMES_ITERATOR_END;
// 	}

// 	// Get the relative instant end
// 	size_t next_instant = key_instants_iter_data->current_instant + 1;
// 	size_t next_slice   = key_instants_iter_data->current_slice;
// 	if (next_instant >= stream_graph->key_instants.slices[next_slice].nb_instants) {
// 		next_instant = 0;
// 		next_slice++;
// 	}
// 	size_t relative_instant_end;
// 	size_t absolute_instant_end;

// 	printf("here, next_instant: %zu, next_slice: %zu\n", next_instant, next_slice);

// 	if (next_slice >= stream_graph->key_instants.nb_slices) {
// 		printf("next slice is beyond the end of the slices\n");
// 		relative_instant_end = SLICE_SIZE;
// 		absolute_instant_end = stream_graph->lifespan.end;
// 	}
// 	else {
// 		// printf("next slice is within the slices\n");

// 		// // Check if the instants pointer is null
// 		// if (stream_graph->key_instants.slices[next_slice].instants == NULL) {
// 		// 	printf("%zu\n", stream_graph->key_instants.slices[next_slice].nb_instants);
// 		// 	printf("Error: instants pointer for slice %zu is null\n", next_slice);
// 		// 	return SGA_TIMES_ITERATOR_END; // Handle the error
// 		// }

// 		// While there are no instants in the next slice, increment the next slice
// 		while (next_slice < stream_graph->key_instants.nb_slices &&
// 		       stream_graph->key_instants.slices[next_slice].nb_instants == 0) {
// 			printf("next slice %zu has no instants, incrementing\n", next_slice);
// 			next_slice++;
// 			next_instant = 0;
// 		}
// 		key_instants_iter_data->current_slice = next_slice;
// 		// If you reached the end of the slices, set the end to the lifespan end
// 		if (next_slice >= stream_graph->key_instants.nb_slices) {
// 			printf("next slice %zu is beyond the end of the slices\n", next_slice);
// 			// printf("next slice is beyond the end of the slices\n");
// 			relative_instant_end = SLICE_SIZE;
// 			absolute_instant_end = stream_graph->lifespan.end;
// 			return SGA_Interval_from(absolute_instant, absolute_instant_end);
// 			// printf("setting absolute_instant_end to %zu\n", absolute_instant_end);
// 		}
// 		else {
// 			printf("next slice is within the slices\n");
// 		}

// 		relative_instant_end = stream_graph->key_instants.slices[next_slice].instants[next_instant];
// 		absolute_instant_end = relative_instant_end + (next_slice * SLICE_SIZE);
// 	}

// 	// Increment the current instant
// 	key_instants_iter_data->current_instant++;

// 	printf("returning %zu %zu\n", absolute_instant, absolute_instant_end);

// 	return SGA_Interval_from(absolute_instant, absolute_instant_end);
// }

SGA_Interval KeyInstantsTableIterator_next(SGA_TimesIterator* iter) {
	KeyInstantsTableIterator* key_instants_iter_data = iter->iterator_data;
	SGA_StreamGraph* stream_graph			 = iter->stream_graph.stream_data;

	// Check if we have ended the iterator (last event is the end of the lifespan)
	if (key_instants_iter_data->last_event >= stream_graph->lifespan.end) {
		// printf("4 : returning END\n");
		return SGA_TIMES_ITERATOR_END;
	}

	// If we can still get something from the current slice, return it
	if (key_instants_iter_data->current_slice < stream_graph->key_instants.nb_slices &&
	    key_instants_iter_data->current_instant <
		stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants) {
		SGA_TimeId last_event = key_instants_iter_data->last_event;

		size_t relative_instant_start = stream_graph->key_instants.slices[key_instants_iter_data->current_slice]
						    .instants[key_instants_iter_data->current_instant];
		size_t absolute_instant		   = relative_instant_start + (key_instants_iter_data->current_slice * SLICE_SIZE);
		key_instants_iter_data->last_event = absolute_instant;
		key_instants_iter_data->current_instant++;

		// printf("3 : returning %zu %zu\n", last_event, absolute_instant);
		return SGA_Interval_from(last_event, absolute_instant);
	}

	// We reached the end of the current slice, so we need to go to the next slice
	key_instants_iter_data->current_instant = 0;
	key_instants_iter_data->current_slice++;

	// If current slice is empty, go to the next slice
	while (key_instants_iter_data->current_slice < stream_graph->key_instants.nb_slices &&
	       stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants == 0) {
		key_instants_iter_data->current_slice++;
		key_instants_iter_data->current_instant = 0;
	}

	// If we reached the end of the slices, return end of lifespan
	if (key_instants_iter_data->current_slice >= stream_graph->key_instants.nb_slices) {
		SGA_Interval to_return = SGA_Interval_from(key_instants_iter_data->last_event, stream_graph->lifespan.end);
		// End the iterator
		key_instants_iter_data->last_event = stream_graph->lifespan.end;
		// printf("2 : reached end of slices, returning %zu %zu\n", to_return.start, to_return.end);
		return to_return;
	}

	// printf("DEBUG: current_slice = %zu, total nb_slices = %zu\n",
	//        key_instants_iter_data->current_slice,
	//        stream_graph->key_instants.nb_slices);
	// printf("DEBUG: current_instant = %u, total nb_instants = %zu\n",
	//        key_instants_iter_data->current_instant,
	//        stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants);
	// printf("XXX: current instant = %u\n", key_instants_iter_data->current_instant);
	// printf("XXX: current slice nb instants = %zu\n",
	//        stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants);
	// if (stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants > 0) {
	// print all instants in the current slice
	// printf("DEBUG: instants in current slice:\n");
	// for (size_t i = 0; i < stream_graph->key_instants.slices[key_instants_iter_data->current_slice].nb_instants; i++) {
	// printf("E: %u ", stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[i]);
	// }
	// printf("\n");
	// }
	// Otherwise, we can return the next instant
	// Debugging information
	// printf("DEBUG: current_slice = %zu, current_instant = %u\n",
	//        key_instants_iter_data->current_slice,
	//        key_instants_iter_data->current_instant);
	// printf("DEBUG: relative_instant_start = %u\n",
	//        stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[key_instants_iter_data->current_instant]);
	// printf("DEBUG: SLICE_SIZE = %zu\n", SLICE_SIZE);

	// Calculate absolute_instant
	size_t relative_instant_start =
	    stream_graph->key_instants.slices[key_instants_iter_data->current_slice].instants[key_instants_iter_data->current_instant];
	size_t absolute_instant = relative_instant_start + (key_instants_iter_data->current_slice * SLICE_SIZE);
	// printf("DEBUG: absolute_instant = %zu\n", absolute_instant);

	// Update last_event and return interval
	SGA_TimeId last_event		   = key_instants_iter_data->last_event;
	key_instants_iter_data->last_event = absolute_instant;
	key_instants_iter_data->current_instant++;
	// printf("1 : returning %zu %zu\n", last_event, absolute_instant);
	return SGA_Interval_from(last_event, absolute_instant);
}

void KeyInstantsTableIterator_destroy(SGA_TimesIterator* iter) {
	free(iter->iterator_data);
}

SGA_TimesIterator SGA_StreamGraph_key_instants(SGA_StreamGraph* stream_graph) {
	SGA_Stream stream				 = {.stream_data = stream_graph};
	KeyInstantsTableIterator* key_instants_iter_data = MALLOC(sizeof(KeyInstantsTableIterator));
	*key_instants_iter_data				 = (KeyInstantsTableIterator){
				     .last_event      = stream_graph->lifespan.start,
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
		// printf("reached end of slices\n");
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
