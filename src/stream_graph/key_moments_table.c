#define SGA_INTERNAL

#include "key_moments_table.h"
#include "../units.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>

SGA_Time KeyMomentsTable_nth_key_moment(KeyMomentsTable* kmt, SGA_TimeId n) {
	// TODO : add debug for if use in not initliased table
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		// printf("slice %zu, nb_moments %zu\n", i, kmt->slices[i].nb_moments);
		// printf("n %zu\n", n);
		// printf("n %zu\n", n);
		// printf("kmt->slices %p\n", kmt->slices);
		// printf("kmt->slices[i].nb_moments %zu\n", kmt->slices[i].nb_moments);
		if (n < kmt->slices[i].nb_moments) {
			// printf("returning %zu\n", kmt->slices[i].moments[n] + (i * SLICE_SIZE));
			return kmt->slices[i].moments[n] + (i * SLICE_SIZE);
		}
		n -= kmt->slices[i].nb_moments;
	}
	return SIZE_MAX;
}

void KeyMomentsTable_push_in_order(KeyMomentsTable* kmt, SGA_Time key_moment) {
	SGA_TimeId slice	       = key_moment / SLICE_SIZE;
	RelativeMoment relative_moment = key_moment % SLICE_SIZE;
	if (slice != kmt->fill_info.current_slice) {
		kmt->fill_info.current_slice  = slice;
		kmt->fill_info.current_moment = 0;
	}
	kmt->slices[slice].moments[kmt->fill_info.current_moment] = relative_moment;
	kmt->fill_info.current_moment++;
}

KeyMomentsTable KeyMomentsTable_alloc(size_t nb_slices) {
	KeyMomentsTable kmt = {
	    .nb_slices = nb_slices,
	    .slices    = (MomentsSlice*)MALLOC(nb_slices * sizeof(MomentsSlice)),
	    .fill_info =
		{
		    .current_slice  = 0,
		    .current_moment = 0,
		},
	};
	return kmt;
}

void KeyMomentsTable_alloc_slice(KeyMomentsTable* kmt, SGA_TimeId slice, RelativeMoment nb_moments) {
	kmt->slices[slice].nb_moments = nb_moments;
	// kmt->slices[slice].moments	  = (RelativeMoment*)MALLOC(nb_moments * sizeof(RelativeMoment));
	if (nb_moments == 0) {
		kmt->slices[slice].moments = NULL;
	}
	else {
		kmt->slices[slice].moments = (RelativeMoment*)MALLOC(nb_moments * sizeof(RelativeMoment));
	}
}

SGA_Time KeyMomentsTable_first_moment(KeyMomentsTable* kmt) {
	SGA_TimeId first_slice_idx = 0;
	for (SGA_TimeId i = 0; i < kmt->nb_slices; i++) {
		if (kmt->slices[i].nb_moments > 0) {
			first_slice_idx = i;
			break;
		}
	}
	SGA_TimeId first_moment = kmt->slices[first_slice_idx].moments[0] + (first_slice_idx * SLICE_SIZE);
	return first_moment;
}

SGA_Time KeyMomentsTable_last_moment(KeyMomentsTable* kmt) {
	SGA_TimeId last_slice_idx      = kmt->nb_slices - 1;
	RelativeMoment last_moment_idx = kmt->slices[last_slice_idx].nb_moments - 1;
	SGA_Time last_moment	       = kmt->slices[last_slice_idx].moments[last_moment_idx] + (last_slice_idx * SLICE_SIZE);
	return last_moment;
}
void KeyMomentsTable_destroy(KeyMomentsTable kmt) {
	for (size_t i = 0; i < kmt.nb_slices; i++) {
		if (kmt.slices[i].moments != NULL) {
			free(kmt.slices[i].moments);
		}
	}
	free(kmt.slices);
}

// TODO : Change the slices to contain the number of total moments before rather than only the number of moments in the
// slice for faster search.
// Assumes the times are sorted, returns the index of a certain time if all of them were in a single array
SGA_TimeId KeyMomentsTable_find_time_index_if_pushed(KeyMomentsTable* kmt, SGA_Time t) {
	// First we find which slice the time is in
	SGA_TimeId slice = t / SLICE_SIZE;
	// Add the number of moments in the previous slices
	SGA_TimeId index = 0;
	for (SGA_TimeId i = 0; i < slice; i++) {
		if (i >= kmt->nb_slices) {
			return index;
		}
		index += kmt->slices[i].nb_moments;
	}
	// Then we find the index of the time in the slice
	RelativeMoment relative_time = t % SLICE_SIZE;
	// Recherche dichotomique
	size_t left  = 0;
	size_t right = kmt->slices[slice].nb_moments;
	while (left < right) {
		size_t mid = (left + right) / 2;

		if (kmt->slices[slice].moments[mid] == relative_time) {
			return index + mid;
		}

		if (kmt->slices[slice].moments[mid] < relative_time) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}

	// return the index where the time should be inserted
	return index + left;
}

// Returns the index of the the moment which has equivalent events to time t
SGA_TimeId KeyMomentsTable_find_time_index_equivalent(KeyMomentsTable* kmt, SGA_Time t) {
	// First we find which slice the time is in
	SGA_TimeId slice = t / SLICE_SIZE;
	// Add the number of moments in the previous slices
	SGA_TimeId index = 0;
	for (SGA_TimeId i = 0; i < slice; i++) {
		if (i >= kmt->nb_slices) {
			return index;
		}
		index += kmt->slices[i].nb_moments;
	}
	// Then we find the index of the time in the slice
	RelativeMoment relative_time = t % SLICE_SIZE;
	// Recherche dichotomique
	size_t left  = 0;
	size_t right = kmt->slices[slice].nb_moments;
	while (left < right) {
		size_t mid = (left + right) / 2;

		if (kmt->slices[slice].moments[mid] == relative_time) {
			return index + mid;
		}

		if (kmt->slices[slice].moments[mid] < relative_time) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}

	// return the index where the time should be equivalent

	// If later that the last moment of the slice
	if (left >= kmt->slices[slice].nb_moments) {
		return index + kmt->slices[slice].nb_moments - 1;
	}
	// If earlier than the moment found
	if (relative_time < kmt->slices[slice].moments[left]) {
		return index + left - 1;
	}
	else { // (relative_time == kmt->slices[slice].moments[left])
		return index + left;
	}
}

void print_key_moments_table(KeyMomentsTable* kmt) {
	printf("Key moments table\n");
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		printf("Slice %zu\n", i);
		for (size_t j = 0; j < kmt->slices[i].nb_moments; j++) {
			printf("%zu ", kmt->slices[i].moments[j] + (i * SLICE_SIZE));
		}
		printf("\n");
	}
}