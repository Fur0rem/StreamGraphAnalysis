/**
 * @file src/weighted/weight_types/lerp.c
 * @brief A linearly interpolated weight function for weighted streams.
 */

#include "lerp.h"

LerpWeightFunc LerpWeightFunc_new(size_t nb_elements, size_t nb_times) {
	LerpWeightFunc lerp = {
	    .nb_elements = nb_elements,
	    .nb_times	 = nb_times,
	    .weights	 = (SGA_Weight**)malloc(nb_elements * sizeof(SGA_Weight*)),
	};
	for (size_t i = 0; i < nb_elements; i++) {
		lerp.weights[i] = (SGA_Weight*)malloc(nb_times * sizeof(SGA_Weight));
	}
	return lerp;
}

/**
 * @brief Deallocates a LerpWeightFunc structure.
 *
 * @param self The LerpWeightFunc structure to deallocate.
 */
void LerpWeightFunc_destroy(LerpWeightFunc self) {
	for (size_t i = 0; i < self.nb_elements; i++) {
		free(self.weights[i]);
	}
	free(self.weights);
}

/**
 * @brief Gets the weight for a specific element at a specific time.
 * @param self The LerpWeightFunc structure to get the weight from.
 * @param element_id The id of the element to get the weight for.
 * @param time The time instant to get the weight at.
 * @return The weight for the element at the given time.
 */
SGA_Weight LerpWeightFunc_weight_at_t(const LerpWeightFunc* self, size_t element_id, SGA_Time time) {
	ASSERT(element_id < self->nb_elements);
	ASSERT(time < self->nb_times);
	return self->weights[element_id][time];
}

SGA_Weight LerpWeightFunc_weight_integral_between(const LerpWeightFunc* self, size_t element_id, SGA_Interval interval) {
	ASSERT(element_id < self->nb_elements);
	ASSERT(SGA_Interval_contains_interval((SGA_Interval){.start = 0, .end = self->nb_times - 1}, interval));

	SGA_Weight integral = 0.0;
	for (size_t t = interval.start; t < interval.end; t++) {
		SGA_Weight current = self->weights[element_id][t];
		SGA_Weight next	   = self->weights[element_id][t + 1];
		// Trapezoidal rule for integration
		integral += (current + next) / 2.0;
	}
	return integral;
}

SGA_Weight LerpWeightFunc_max(const LerpWeightFunc* self) {
	SGA_Weight max_weight = 0.0;
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < self->nb_times; j++) {
			if (self->weights[i][j] > max_weight) {
				max_weight = self->weights[i][j];
			}
		}
	}
	return max_weight;
}

SGA_Weight LerpWeightFunc_min(const LerpWeightFunc* self) {
	SGA_Weight min_weight = INFINITY;
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < self->nb_times; j++) {
			if (self->weights[i][j] < min_weight) {
				min_weight = self->weights[i][j];
			}
		}
	}
	return min_weight;
}

void LerpWeightFunc_normalise(LerpWeightFunc* self) {
	SGA_Weight min = LerpWeightFunc_min(self);
	SGA_Weight max = LerpWeightFunc_max(self);
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < self->nb_times; j++) {
			self->weights[i][j] = (self->weights[i][j] - min) / (max - min);
		}
	}
}