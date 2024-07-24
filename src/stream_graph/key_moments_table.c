#include "key_moments_table.h"
#include "../units.h"
#include "../utils.h"
#include <stddef.h>
#include <stdio.h>

size_t KeyMomentsTable_nth_key_moment(KeyMomentsTable* kmt, size_t n) {
	// TODO : add debug for if use in not initliased table
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		// printf("slice %zu, nb_moments %zu\n", i, kmt->slices[i].nb_moments);
		// printf("n %zu\n", n);
		if (n < kmt->slices[i].nb_moments) {
			return kmt->slices[i].moments[n] + (i * SLICE_SIZE);
		}
		n -= kmt->slices[i].nb_moments;
	}
	return SIZE_MAX;
}

void KeyMomentsTable_push_in_order(KeyMomentsTable* kmt, size_t key_moment) {
	size_t slice = key_moment / SLICE_SIZE;
	size_t relative_moment = key_moment % SLICE_SIZE;
	if (slice != kmt->fill_info.current_slice) {
		kmt->fill_info.current_slice = slice;
		kmt->fill_info.current_moment = 0;
	}
	kmt->slices[slice].moments[kmt->fill_info.current_moment] = relative_moment;
	kmt->fill_info.current_moment++;
}

KeyMomentsTable KeyMomentsTable_alloc(size_t nb_slices) {
	KeyMomentsTable kmt;
	kmt.nb_slices = nb_slices;
	kmt.slices = (MomentsSlice*)MALLOC(nb_slices * sizeof(MomentsSlice));
	kmt.fill_info.current_slice = 0;
	kmt.fill_info.current_moment = 0;
	return kmt;
}

void KeyMomentsTable_alloc_slice(KeyMomentsTable* kmt, size_t slice, size_t nb_moments) {
	kmt->slices[slice].nb_moments = nb_moments;
	kmt->slices[slice].moments = (RelativeMoment*)MALLOC(nb_moments * sizeof(RelativeMoment));
}

size_t KeyMomentsTable_first_moment(KeyMomentsTable* kmt) {
	size_t first_slice_idx = 0;
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		if (kmt->slices[i].nb_moments > 0) {
			first_slice_idx = i;
			break;
		}
	}
	size_t first_moment = kmt->slices[first_slice_idx].moments[0] + (first_slice_idx * SLICE_SIZE);
	return first_moment;
}

size_t KeyMomentsTable_last_moment(KeyMomentsTable* kmt) {
	size_t last_slice_idx = kmt->nb_slices - 1;
	size_t last_moment_idx = kmt->slices[last_slice_idx].nb_moments - 1;
	size_t last_moment = kmt->slices[last_slice_idx].moments[last_moment_idx] + (last_slice_idx * SLICE_SIZE);
	return last_moment;
}

// DEFAULT_COMPARE(size_t2)
// DEFAULT_TO_STRING(size_t2, "%zu")

// size_t2Vector KeyMomentsTable_all_moments(KeyMomentsTable* kmt) {
// 	size_t2Vector moments = size_t2Vector_with_capacity(100);
// 	for (size_t i = 0; i < kmt->nb_slices; i++) {
// 		for (size_t j = 0; j < kmt->slices[i].nb_moments; j++) {
// 			size_t2Vector_push(&moments, kmt->slices[i].moments[j] + (i * SLICE_SIZE));
// 		}
// 	}
// 	return moments;
// }

void KeyMomentsTable_destroy(KeyMomentsTable kmt) {
	for (size_t i = 0; i < kmt.nb_slices; i++) {
		free(kmt.slices[i].moments);
	}
	free(kmt.slices);
}

// TODO : Change the slices to contain the number of total moments before rather than only the number of moments in the
// slice for faster search.
// Assumes the times are sorted, returns the index of a certain time if all of them were in a single array
size_t KeyMomentsTable_find_time_index(KeyMomentsTable* kmt, TimeId t) {
	// First we find which slice the time is in
	size_t slice = t / SLICE_SIZE;
	// Add the number of moments in the previous slices
	size_t index = 0;
	for (size_t i = 0; i < slice; i++) {
		if (i >= kmt->nb_slices) {
			return index;
		}
		index += kmt->slices[i].nb_moments;
	}
	// Then we find the index of the time in the slice
	size_t relative_time = t % SLICE_SIZE;
	// Recherche dichotomique
	size_t left = 0;
	printf("kmt : %p\n", kmt);
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