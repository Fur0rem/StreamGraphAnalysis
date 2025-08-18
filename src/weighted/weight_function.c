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

/**
 * @brief Normalises the weights of the function to the range [0, 1].
 * @param weight_func The weight function to normalise.
 */
void SGA_WeightFunc_normalise(SGA_WeightFunc* weight_func) {
	switch (weight_func->tag) {
		case LERP: {
			LerpWeightFunc_normalise(&weight_func->func.lerped);
			break;
		}
		case CONST_UNIVERSALLY: {
			ConstUniversally_normalise(&weight_func->func.const_universally);
			break;
		}
	}
	UNREACHABLE_CODE;
}