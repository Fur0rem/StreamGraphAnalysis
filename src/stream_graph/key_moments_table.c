#include "key_moments_table.h"
#include "../utils.h"
#include <stddef.h>

size_t SGA_KeyMomentsTable_nth_key_moment(KeyMomentsTable* kmt, size_t n) {
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		if (n < kmt->slices[i].nb_moments) {
			return kmt->slices[i].moments[n] + (i * SLICE_SIZE);
		}
		n -= kmt->slices[i].nb_moments;
	}
	return SIZE_MAX;
}

void SGA_KeyMomentsTable_push_in_order(KeyMomentsTable* kmt, size_t key_moment) {
	size_t slice = key_moment / SLICE_SIZE;
	size_t relative_moment = key_moment % SLICE_SIZE;
	if (slice != kmt->fill_info.current_slice) {
		kmt->fill_info.current_slice = slice;
		kmt->fill_info.current_moment = 0;
	}
	kmt->slices[slice].moments[kmt->fill_info.current_moment] = relative_moment;
	kmt->fill_info.current_moment++;
}

KeyMomentsTable SGA_KeyMomentsTable_alloc(size_t nb_slices) {
	KeyMomentsTable kmt;
	kmt.nb_slices = nb_slices;
	kmt.slices = (MomentsSlice*)MALLOC(nb_slices * sizeof(MomentsSlice));
	kmt.fill_info.current_slice = 0;
	kmt.fill_info.current_moment = 0;
	return kmt;
}

void SGA_KeyMomentsTable_alloc_slice(KeyMomentsTable* kmt, size_t slice, size_t nb_moments) {
	kmt->slices[slice].nb_moments = nb_moments;
	kmt->slices[slice].moments = (RelativeMoment*)MALLOC(nb_moments * sizeof(RelativeMoment));
}

size_t SGA_KeyMomentsTable_first_moment(KeyMomentsTable* kmt) {
	size_t first_slice_idx = 0;
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		if (kmt->slices[i].nb_moments > 0) {
			first_slice_idx = i;
			break;
		}
	}
	size_t first_moment =
		kmt->slices[first_slice_idx].moments[0] + (first_slice_idx * (RelativeMoment)~0);
	return first_moment;
}

size_t SGA_KeyMomentsTable_last_moment(KeyMomentsTable* kmt) {
	size_t last_slice_idx = kmt->nb_slices - 1;
	size_t last_moment_idx = kmt->slices[last_slice_idx].nb_moments - 1;
	size_t last_moment = kmt->slices[last_slice_idx].moments[last_moment_idx] +
						 (last_slice_idx * (RelativeMoment)~0);
	return last_moment;
}

void SGA_KeyMomentsTable_destroy(KeyMomentsTable* kmt) {
	for (size_t i = 0; i < kmt->nb_slices; i++) {
		free(kmt->slices[i].moments);
	}
	free(kmt->slices);
}