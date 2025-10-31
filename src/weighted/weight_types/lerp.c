/**
 * @file src/weighted/weight_types/lerp.c
 * @brief A linearly interpolated weight function for weighted streams.
 */

#define SGA_INTERNAL

#include "lerp.h"
#include "../../stream.h"
#include <stddef.h>

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

LerpWeightedElement LerpWeightedElement_from(LerpWeightPointArrayListArrayList* weighted_intervals, SGA_IntervalsSet* intervals_set) {
	size_t nb_intervals = weighted_intervals->length;

	// First, count total number of weight points
	size_t total_weight_points = 0;
	for (size_t i = 0; i < nb_intervals; i++) {
		SGA_Time first_time_in_interval = LerpWeightPointArrayList_first(&weighted_intervals->array[i])->time_instant;
		SGA_Time last_time_in_interval	= LerpWeightPointArrayList_last(&weighted_intervals->array[i])->time_instant;
		total_weight_points += (last_time_in_interval - first_time_in_interval) + 1;
	}

	LerpWeightedElement element = {
	    .weights	       = MALLOC(total_weight_points * sizeof(SGA_Weight)),
	    .covered_intervals = intervals_set,
	};
	size_t weight_idx	  = 0;
	SGA_Time last_time_filled = SGA_Time_max();
	for (size_t interval_idx = 0; interval_idx < weighted_intervals->length; interval_idx++) {
		// Fill in weights for this interval
		LerpWeightPointArrayList interval_points = weighted_intervals->array[interval_idx];

		for (size_t point_idx = 0; point_idx < interval_points.length - 1; point_idx++) {
			// Interpolate weights between start_point and end_point (end inclusive)
			LerpWeightPoint start_point = interval_points.array[point_idx];
			LerpWeightPoint end_point   = interval_points.array[point_idx + 1];
			for (SGA_Time t = start_point.time_instant; t <= end_point.time_instant; t++) {
				// Skip if already filled (=> 2 WeightPoints are contiguous in time)
				if (t == last_time_filled) {
					continue;
				}
				last_time_filled = t;

				element.weights[weight_idx] = lerp(start_point.time_instant,
								   start_point.associated_weight,
								   end_point.time_instant,
								   end_point.associated_weight,
								   t);
				weight_idx++;
			}
		}

		// Add covered interval
		SGA_Interval covered_interval = {
		    .start = LerpWeightPointArrayList_first(&interval_points)->time_instant,
		    .end   = LerpWeightPointArrayList_last(&interval_points)->time_instant,
		};
		element.covered_intervals->intervals[interval_idx] = covered_interval;
	}

	DEV_ASSERT(weight_idx == total_weight_points);

	return element;
}

size_t nb_weights_in_lerp_element(const LerpWeightedElement* element) {
	size_t nb_weights = 0;
	for (size_t i = 0; i < element->covered_intervals->nb_intervals; i++) {
		SGA_Interval interval = element->covered_intervals->intervals[i];
		nb_weights += SGA_Interval_duration(interval) + 1; // +1 because end inclusive
	}
	return nb_weights;
}

void LerpWeightFunc_destroy(LerpWeightFunc self) {
	for (size_t i = 0; i < self.nb_elements; i++) {
		free(self.elements[i].weights);
	}
	free(self.elements);
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
	SGA_IntervalsSet* element_intervals = self->elements[element_id].covered_intervals;
	ASSERT(SGA_IntervalsSet_contains_sorted(*element_intervals, time));

	// Find the correct time index
	size_t time_index = 0;
	for (size_t i = 0; i < element_intervals->nb_intervals; i++) {
		SGA_Interval interval = element_intervals->intervals[i];

		// If contains time, compute offset and break
		if (time >= interval.start && time <= interval.end) {
			time_index += (time - interval.start);
			break;
		}
		// Else, skip this interval
		else {
			time_index += SGA_Interval_duration(interval) + 1; // +1 because end inclusive
		}
	}

	return self->elements[element_id].weights[time_index];
}

SGA_Weight LerpWeightFunc_weight_integral_between(const LerpWeightFunc* self, size_t element_id, SGA_Interval interval) {
	ASSERT(element_id < self->nb_elements);
	ASSERT(SGA_IntervalsSet_contains_sorted(*self->elements[element_id].covered_intervals, interval.start));
	ASSERT(SGA_IntervalsSet_contains_sorted(*self->elements[element_id].covered_intervals, interval.end - 1));

	SGA_Weight integral = 0.0;
	for (size_t t = interval.start; t < interval.end; t++) {
		SGA_Weight current = LerpWeightFunc_weight_at_t(self, element_id, t);
		SGA_Weight next	   = LerpWeightFunc_weight_at_t(self, element_id, t + 1);
		// Trapezoidal rule for integration
		integral += (current + next) / 2.0;
	}
	return integral;
}

SGA_Weight LerpWeightFunc_max(const LerpWeightFunc* self) {
	SGA_Weight max_weight = 0.0;
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < nb_weights_in_lerp_element(&self->elements[i]); j++) {
			if (self->elements[i].weights[j] > max_weight) {
				max_weight = self->elements[i].weights[j];
			}
		}
	}
	return max_weight;
}

SGA_Weight LerpWeightFunc_min(const LerpWeightFunc* self) {
	SGA_Weight min_weight = INFINITY;
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < nb_weights_in_lerp_element(&self->elements[i]); j++) {
			if (self->elements[i].weights[j] < min_weight) {
				min_weight = self->elements[i].weights[j];
			}
		}
	}
	return min_weight;
}

void LerpWeightFunc_normalise(LerpWeightFunc* self, SGA_Weight min, SGA_Weight max) {
	for (size_t i = 0; i < self->nb_elements; i++) {
		for (size_t j = 0; j < nb_weights_in_lerp_element(&self->elements[i]); j++) {
			self->elements[i].weights[j] = (self->elements[i].weights[j] - min) / (max - min);
		}
	}
}

LerpWeightFunc LerpWeightFunc_from_parsed(ParsedLerpWeightFunction parsed, SGA_StreamGraph* base, bool is_node,
					  LinkIdMapHashset* link_id_map) {
	size_t nb_elements = parsed.weights_per_elem.length;
	LerpWeightFunc fn  = {
	     .nb_elements = nb_elements,
	     .elements	  = MALLOC(nb_elements * sizeof(LerpWeightedElement)),
	 };

	for (size_t elem_idx = 0; elem_idx < parsed.weights_per_elem.length; elem_idx++) {
		size_t elem_id;
		SGA_IntervalsSet* elem_intervals;
		ParsedLerpWeight elem_intervals_weights = parsed.weights_per_elem.array[elem_idx];
		SGA_NodeOrLink elem			= elem_intervals_weights.elem;
		if (is_node) {
			elem_id	       = elem.node;
			elem_intervals = &base->nodes.nodes[elem_id].presence;
		}
		else {
			LinkIdMap key  = LinkIdMap_key_only(elem.link.nodes[0], elem.link.nodes[1]);
			elem_id	       = LinkIdMapHashset_find(*link_id_map, key)->id;
			elem_intervals = &base->links.links[elem_id].presence;
		}
		fn.elements[elem_idx] = LerpWeightedElement_from(&elem_intervals_weights.associated_weights, elem_intervals);
	}
	return fn;
}