#define SGA_INTERNAL

#include "key_instants_table.h"
#include "../units.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>

SGA_Time KeyInstantsTable_nth_key_instant(KeyInstantsTable* kmt, SGA_TimeId n) {
	// TODO : add debug for if use in not initliased table
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		// printf("slice %zu, nb_instants %zu\n", i, kmt->slices[i].nb_instants);
		// printf("n %zu\n", n);
		// printf("n %zu\n", n);
		// printf("kmt->slices %p\n", kmt->slices);
		// printf("kmt->slices[i].nb_instants %zu\n", kmt->slices[i].nb_instants);
		if (n < kmt->slices[i].nb_instants) {
			// printf("returning %zu\n", kmt->slices[i].instants[n] + (i * SLICE_SIZE));
			return kmt->slices[i].instants[n] + (i * SLICE_SIZE);
		}
		n -= kmt->slices[i].nb_instants;
	}
	return SIZE_MAX;
}

void KeyInstantsTable_push_in_order(KeyInstantsTable* kmt, SGA_Time key_instant) {
	SGA_TimeId slice		 = key_instant / SLICE_SIZE;
	RelativeInstant relative_instant = key_instant % SLICE_SIZE;
	if (slice != kmt->fill_info.current_slice) {
		kmt->fill_info.current_slice   = slice;
		kmt->fill_info.current_instant = 0;
	}
	kmt->slices[slice].instants[kmt->fill_info.current_instant] = relative_instant;
	kmt->fill_info.current_instant++;
}

KeyInstantsTable KeyInstantsTable_alloc(size_t nb_slices) {
	KeyInstantsTable kmt = {
	    .nb_slices = nb_slices,
	    .slices    = (InstantsSlice*)MALLOC(nb_slices * sizeof(InstantsSlice)),
	    .fill_info =
		{
		    .current_slice   = 0,
		    .current_instant = 0,
		},
	};
	return kmt;
}

void KeyInstantsTable_alloc_slice(KeyInstantsTable* kmt, SGA_TimeId slice, RelativeInstant nb_instants) {
	kmt->slices[slice].nb_instants = nb_instants;
	// kmt->slices[slice].instants	  = (RelativeInstant*)MALLOC(nb_instants * sizeof(RelativeInstant));
	if (nb_instants == 0) {
		kmt->slices[slice].instants = NULL;
	}
	else {
		kmt->slices[slice].instants = (RelativeInstant*)MALLOC(nb_instants * sizeof(RelativeInstant));
	}
}

KeyInstantsTable KeyInstantsTable_from_list(SGA_TimeArrayList* key_instants) {
	// Allocate the table
	KeyInstantsTable kmt = KeyInstantsTable_alloc((key_instants->length / SLICE_SIZE) + 1);
	// Fill the table

	// For each window of instants whose timestamps are separated by SLICE_SIZE, count how many instants there are to allocate the
	// slices
	size_tArrayList nb_instants_per_slice = size_tArrayList_with_capacity(kmt.nb_slices);
	for (size_t i = 0; i < kmt.nb_slices; i++) {
		size_tArrayList_push(&nb_instants_per_slice, 0);
	}
	for (size_t i = 0; i < key_instants->length; i++) {
		// Get in which slice it should be in
		SGA_Time key_instant = key_instants->array[i];
		SGA_TimeId slice     = key_instant / SLICE_SIZE;
		// Check bounds
		ASSERT(slice < kmt.nb_slices);

		// Increment the number of instants in the slice
		nb_instants_per_slice.array[slice]++;
	}

	// Allocate the slices
	for (size_t i = 0; i < kmt.nb_slices; i++) {
		KeyInstantsTable_alloc_slice(&kmt, i, nb_instants_per_slice.array[i]);
	}

	// Not needed anymore
	size_tArrayList_destroy(nb_instants_per_slice);

	// Fill the slices with the instants
	for (size_t i = 0; i < key_instants->length; i++) {
		SGA_Time key_instant		 = key_instants->array[i];
		SGA_TimeId slice		 = key_instant / SLICE_SIZE;
		RelativeInstant relative_instant = key_instant % SLICE_SIZE;

		ASSERT(slice < kmt.nb_slices);
		ASSERT(kmt.slices[slice].nb_instants > 0);

		kmt.slices[slice].instants[kmt.fill_info.current_instant] = relative_instant;
		kmt.fill_info.current_instant++;
	}

	return kmt;
}

SGA_Time KeyInstantsTable_first_instant(KeyInstantsTable* kmt) {
	SGA_TimeId first_slice_idx = 0;
	for (SGA_TimeId i = 0; i < kmt->nb_slices; i++) {
		if (kmt->slices[i].nb_instants > 0) {
			first_slice_idx = i;
			break;
		}
	}
	SGA_TimeId first_instant = kmt->slices[first_slice_idx].instants[0] + (first_slice_idx * SLICE_SIZE);
	return first_instant;
}

SGA_Time KeyInstantsTable_last_instant(KeyInstantsTable* kmt) {
	SGA_TimeId last_slice_idx	 = kmt->nb_slices - 1;
	RelativeInstant last_instant_idx = kmt->slices[last_slice_idx].nb_instants - 1;
	SGA_Time last_instant		 = kmt->slices[last_slice_idx].instants[last_instant_idx] + (last_slice_idx * SLICE_SIZE);
	return last_instant;
}
void KeyInstantsTable_destroy(KeyInstantsTable kmt) {
	for (size_t i = 0; i < kmt.nb_slices; i++) {
		if (kmt.slices[i].instants != NULL) {
			free(kmt.slices[i].instants);
		}
	}
	free(kmt.slices);
}

// TODO : Change the slices to contain the number of total instants before rather than only the number of instants in the
// slice for faster search.
// Assumes the times are sorted, returns the index of a certain time if all of them were in a single array
SGA_TimeId KeyInstantsTable_find_time_index_if_pushed(KeyInstantsTable* kmt, SGA_Time t) {
	// First we find which slice the time is in
	SGA_TimeId slice = t / SLICE_SIZE;
	// Add the number of instants in the previous slices
	SGA_TimeId index = 0;
	for (SGA_TimeId i = 0; i < slice; i++) {
		if (i >= kmt->nb_slices) {
			return index;
		}
		index += kmt->slices[i].nb_instants;
	}
	// Then we find the index of the time in the slice
	RelativeInstant relative_time = t % SLICE_SIZE;
	// Recherche dichotomique
	size_t left  = 0;
	size_t right = kmt->slices[slice].nb_instants;
	while (left < right) {
		size_t mid = (left + right) / 2;

		if (kmt->slices[slice].instants[mid] == relative_time) {
			return index + mid;
		}

		if (kmt->slices[slice].instants[mid] < relative_time) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}

	// return the index where the time should be inserted
	return index + left;
}

// Returns the index of the the instant which has equivalent events to time t
SGA_TimeId KeyInstantsTable_find_time_index_equivalent(KeyInstantsTable* kmt, SGA_Time t) {
	// First we find which slice the time is in
	SGA_TimeId slice = t / SLICE_SIZE;
	// Add the number of instants in the previous slices
	SGA_TimeId index = 0;
	for (SGA_TimeId i = 0; i < slice; i++) {
		if (i >= kmt->nb_slices) {
			return index;
		}
		index += kmt->slices[i].nb_instants;
	}
	// Then we find the index of the time in the slice
	RelativeInstant relative_time = t % SLICE_SIZE;
	// Recherche dichotomique
	size_t left  = 0;
	size_t right = kmt->slices[slice].nb_instants;
	while (left < right) {
		size_t mid = (left + right) / 2;

		if (kmt->slices[slice].instants[mid] == relative_time) {
			return index + mid;
		}

		if (kmt->slices[slice].instants[mid] < relative_time) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}

	// return the index where the time should be equivalent

	// If later that the last instant of the slice
	if (left >= kmt->slices[slice].nb_instants) {
		return index + kmt->slices[slice].nb_instants - 1;
	}
	// If earlier than the instant found
	if (relative_time < kmt->slices[slice].instants[left]) {
		return index + left - 1;
	}
	else { // (relative_time == kmt->slices[slice].instants[left])
		return index + left;
	}
}

void print_key_instants_table(KeyInstantsTable* kmt) {
	printf("Key instants table\n");
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		printf("Slice %zu\n", i);
		for (size_t j = 0; j < kmt->slices[i].nb_instants; j++) {
			printf("%zu ", kmt->slices[i].instants[j] + (i * SLICE_SIZE));
		}
		printf("\n");
	}
}

size_t KeyInstantsTable_in_which_slice_is(KeyInstantsTable* kmt, SGA_Time t) {
	SGA_TimeId slice = t / SLICE_SIZE;
	if (slice >= kmt->nb_slices) {
		return SIZE_MAX; // Out of bounds
	}
	return slice;
}