/**
 * @file src/weighted/weight_types/lerp.c
 * @brief A linearly interpolated weight function for weighted streams.
 */

#define SGA_INTERNAL

#include "lerp.h"

LerpWeightFunc LerpWeightFunc_new(size_t nb_elements, size_t nb_times) {
	LerpWeightFunc lerp = {
	    .nb_elements = nb_elements,
	    .nb_times	 = nb_times,
	    .weights	 = (SGA_Weight**)malloc(nb_elements * sizeof(SGA_Weight*)),
	};
	for (size_t i = 0; i < nb_elements; i++) {
		lerp.weights[i] = (SGA_Weight*)malloc((nb_times + 1) * sizeof(SGA_Weight)); // (nb_times + 1) because end inclusive
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

void LerpWeightFunc_normalise(LerpWeightFunc* self, SGA_Weight min, SGA_Weight max) {
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < self->nb_times; j++) {
			self->weights[i][j] = (self->weights[i][j] - min) / (max - min);
		}
	}
}

SGA_Weight lerp(SGA_Time t1, SGA_Weight w1, SGA_Time t2, SGA_Weight w2, SGA_Time t) {
	DEV_ASSERT(t1 != t2);
	if (t <= t1) {
		return w1;
	}
	if (t >= t2) {
		return w2;
	}
	double ratio = (double)(t - t1) / (double)(t2 - t1);
	return w1 + (ratio * (w2 - w1));
}

LerpWeightFunc LerpWeightFunc_from_parsed(ParsedLerpWeightFunction parsed, size_t nb_elements, size_t nb_times, bool is_node,
					  LinkIdMapHashset* link_id_map) {
	LerpWeightFunc fn = LerpWeightFunc_new(nb_elements, nb_times);

	for (size_t elem_idx = 0; elem_idx < parsed.weights_per_elem.length; elem_idx++) {
		size_t elem_id;
		ParsedLerpWeight elem_intervals = parsed.weights_per_elem.array[elem_idx];
		SGA_NodeOrLink elem		= elem_intervals.elem;
		if (is_node) {
			elem_id = elem.node;
		}
		else {
			LinkIdMap key = LinkIdMap_key_only(elem.link.nodes[0], elem.link.nodes[1]);
			elem_id	      = LinkIdMapHashset_find(*link_id_map, key)->id;
		}

		for (size_t interval_idx = 0; interval_idx < elem_intervals.associated_weights.length; interval_idx++) {
			LerpWeightPointArrayList elem_points = elem_intervals.associated_weights.array[interval_idx];
			for (size_t point_idx = 0; point_idx < elem_points.length - 1; point_idx++) {
				LerpWeightPoint start_point = elem_points.array[point_idx];
				LerpWeightPoint end_point   = elem_points.array[point_idx + 1];

				for (SGA_Time t = start_point.time_instant; t <= end_point.time_instant; t++) {
					fn.weights[elem_id][t] = lerp(start_point.time_instant,
								      start_point.associated_weight,
								      end_point.time_instant,
								      end_point.associated_weight,
								      t);
				}
			}
		}
	}
	return fn;
}