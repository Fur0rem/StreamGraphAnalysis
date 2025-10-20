#include "weight_function.h"
#include "weight_types/constant_universally.h"

SGA_WeightFunc SGA_WeightFunc_const_universally(SGA_Weight weight) {
	SGA_WeightFunc self = {
	    .tag		    = CONST_UNIVERSALLY,
	    .func.const_universally = ConstUniversally_with_weight(weight),
	};
	return self;
}

/**
 * @brief Gets the weight for a specific element at a specific time.
 * @param weight_func The weight function to get the weight from.
 * @param element_id The id of the element to get the weight for.
 * @param time The time instant to get the weight at.
 * @return The weight for the element at the given time.
 */
SGA_Weight SGA_WeightFunc_weight_at_t(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Time time) {
	switch (weight_func->tag) {
		case LERP: {
			return LerpWeightFunc_weight_at_t(&weight_func->func.lerped, element_id, time);
		}
		case CONST_UNIVERSALLY: {
			// The constant weight function applies universally, so it doesn't depend on the element_id or time.
			return weight_func->func.const_universally.weight;
		}
	}
	UNREACHABLE_CODE;
}

/**
 * @brief Gets the weight integral for a specific element over a range of time.
 * @param weight_func The weight function to get the weight integral from.
 * @param element_id The id of the element to get the weight integral for.
 * @param interval The time interval over which to compute the weight integral.
 * @return The weight integral for the element over the given time interval.
 */
SGA_Weight SGA_WeightFunc_weight_integral_between(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Interval interval) {
	switch (weight_func->tag) {
		case LERP: {
			return LerpWeightFunc_weight_integral_between(&weight_func->func.lerped, element_id, interval);
		}
		case CONST_UNIVERSALLY: {
			return ConstUniversally_weight_integral_between(&weight_func->func.const_universally, interval);
		}
	}
	UNREACHABLE_CODE;
}

/**
 * @brief Gets the maximum weight of the function.
 */
SGA_Weight SGA_WeightFunc_max(const SGA_WeightFunc* weight_func) {
	switch (weight_func->tag) {
		case LERP: {
			return LerpWeightFunc_max(&weight_func->func.lerped);
		}
		case CONST_UNIVERSALLY: {
			return weight_func->func.const_universally.weight; // The maximum is the same as the constant weight.
		}
	}
	UNREACHABLE_CODE;
}

/**
 * @brief Gets the minimum weight of the function.
 */
SGA_Weight SGA_WeightFunc_min(const SGA_WeightFunc* weight_func) {
	switch (weight_func->tag) {
		case LERP: {
			return LerpWeightFunc_min(&weight_func->func.lerped);
		}
		case CONST_UNIVERSALLY: {
			return weight_func->func.const_universally.weight; // The minimum is the same as the constant weight.
		}
	}
	UNREACHABLE_CODE;
}

void SGA_WeightFunc_normalise(SGA_WeightFunc* weight_func, SGA_Weight min, SGA_Weight max) {
	// If all weights are the same, turn into a constant function of weight 1
	if (min == max) {
		SGA_WeightFunc_destroy(*weight_func);
		*weight_func = SGA_WeightFunc_const_universally(1.0);
	}

	// Otherwise, normalise according to the type of weight function
	switch (weight_func->tag) {
		case LERP: {
			LerpWeightFunc_normalise(&weight_func->func.lerped, min, max);
			break;
		}
		case CONST_UNIVERSALLY: {
			ConstUniversally_normalise(&weight_func->func.const_universally, min, max);
			break;
		}
	}
	UNREACHABLE_CODE;
}

void SGA_WeightFunc_destroy(SGA_WeightFunc weight_func) {
	switch (weight_func.tag) {
		case LERP: {
			LerpWeightFunc_destroy(weight_func.func.lerped);
			break;
		}
		case CONST_UNIVERSALLY: {
			// No dynamic memory to free for ConstUniversally
			break;
		}
	}
}

SGA_Weight SGA_WeightFunc_max_of_elem_in_interval(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Interval interval) {
	switch (weight_func->tag) {
		case LERP: {
			SGA_Weight max = -INFINITY;
			for (SGA_Time t = interval.start; t < interval.end; t++) {
				SGA_Weight w = LerpWeightFunc_weight_at_t(&weight_func->func.lerped, element_id, t);
				if (w > max) {
					max = w;
				}
			}
			return max;
		}
		case CONST_UNIVERSALLY: {
			return weight_func->func.const_universally.weight;
		}
	}
	UNREACHABLE_CODE;
}

SGA_Weight SGA_WeightFunc_min_of_elem_in_interval(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Interval interval) {
	switch (weight_func->tag) {
		case LERP: {
			SGA_Weight min = INFINITY;
			for (SGA_Time t = interval.start; t < interval.end; t++) {
				SGA_Weight w = LerpWeightFunc_weight_at_t(&weight_func->func.lerped, element_id, t);
				if (w < min) {
					min = w;
				}
			}
			return min;
		}
		case CONST_UNIVERSALLY: {
			return weight_func->func.const_universally.weight;
		}
	}
	UNREACHABLE_CODE;
}

SGA_Weight SGA_WeightFunc_max_in_interval(const SGA_WeightFunc* weight_func, SGA_Interval interval) {
	switch (weight_func->tag) {
		case LERP: {
			SGA_Weight max = -INFINITY;
			// Loop over all elements and find the maximum weight in the interval
			for (size_t element_id = 0; element_id < weight_func->func.lerped.nb_elements; element_id++) {
				SGA_Weight elem_max = SGA_WeightFunc_max_of_elem_in_interval(weight_func, element_id, interval);
				if (elem_max > max) {
					max = elem_max;
				}
			}
			return max;
		}
		case CONST_UNIVERSALLY: {
			return weight_func->func.const_universally.weight;
		}
	}
	UNREACHABLE_CODE;
}

SGA_Weight SGA_WeightFunc_min_in_interval(const SGA_WeightFunc* weight_func, SGA_Interval interval) {
	switch (weight_func->tag) {
		case LERP: {
			SGA_Weight min = INFINITY;
			// Loop over all elements and find the minimum weight in the interval
			for (size_t element_id = 0; element_id < weight_func->func.lerped.nb_elements; element_id++) {
				SGA_Weight elem_min = SGA_WeightFunc_min_of_elem_in_interval(weight_func, element_id, interval);
				if (elem_min < min) {
					min = elem_min;
				}
			}
			return min;
		}
		case CONST_UNIVERSALLY: {
			return weight_func->func.const_universally.weight;
		}
	}
	UNREACHABLE_CODE;
}