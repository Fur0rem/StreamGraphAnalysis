/**
 * @file weighted/weight_types/constant_universally.c
 * @brief A constant weight function that is universally applied to all elements at all times in a weighted stream.
 */

#define SGA_INTERNAL

#include "constant_universally.h"

/**
 * @brief Creates a new constant weight function with the specified weight.
 * @param weight The constant weight to be applied universally.
 * @return A ConstUniversally structure initialized with the given weight.
 */
ConstUniversally ConstUniversally_with_weight(SGA_Weight weight) {
	ConstUniversally const_universally = {
	    .weight = weight,
	};
	return const_universally;
}

SGA_Weight ConstUniversally_weight_integral_between(const ConstUniversally* self, SGA_Interval interval) {
	// Integral of a constant c over interval I=[a, b] is c * |I|
	return self->weight * (SGA_Weight)SGA_Interval_duration(interval);
}

SGA_Weight ConstUniversally_max(const ConstUniversally* self) {
	return self->weight;
}

SGA_Weight ConstUniversally_min(const ConstUniversally* self) {
	return self->weight;
}

void ConstUniversally_normalise(ConstUniversally* self, SGA_Weight min, SGA_Weight max) {
	if (max == min) {
		self->weight = 1.0; // If all weights are the same, map them to 1.
	}
	else {
		self->weight = (self->weight - min) / (max - min);
	}
}