/**
 * @file weighted/weight_types/constant_universally.h
 * @brief A constant weight function that is universally applied to all elements at all times in a weighted stream.
 */

#ifndef WEIGHTED_WEIGHT_TYPES_CONSTANT_UNIVERSALLY_H
#define WEIGHTED_WEIGHT_TYPES_CONSTANT_UNIVERSALLY_H

#include "../../interval.h"
#include "../../units.h"

/**
 * @brief A structure representing a constant weight function that is universally applied to all elements at all times.
 * This structure is used to define a weight function that has a single constant weight for all elements and all time instants.
 */
typedef struct ConstUniversally {
	SGA_Weight weight; ///< The constant weight that is applied to all elements at all times.
} ConstUniversally;

/**
 * @brief Creates a new constant weight function with the specified weight.
 * @param weight The constant weight to be applied universally.
 * @return A ConstUniversally structure initialized with the given weight.
 */
ConstUniversally ConstUniversally_with_weight(SGA_Weight weight);

/**
 * @brief Gets the weight integral of the function over a time interval.
 * @param self The ConstUniversally structure to get the weight integral from.
 * @param interval The time interval over which to compute the weight integral.
 * @return The weight integral over the given time interval, which is simply the weight multiplied by the duration of the interval.
 */
SGA_Weight ConstUniversally_weight_integral_between(const ConstUniversally* self, SGA_Interval interval);

/**
 * @brief Gets the maximum weight of the function.
 * @param self The ConstUniversally structure to get the maximum weight from.
 * @return The maximum weight, which is the same as the constant weight.
 */
SGA_Weight ConstUniversally_max(const ConstUniversally* self);

/**
 * @brief Gets the minimum weight of the function.
 * @param self The ConstUniversally structure to get the minimum weight from.
 * @return The minimum weight, which is the same as the constant weight.
 */
SGA_Weight ConstUniversally_min(const ConstUniversally* self);

/**
 * @brief Normalises the weight of the function to 1.0.
 * @param self The ConstUniversally structure to normalise.
 */
void ConstUniversally_normalise(ConstUniversally* self);

#endif // WEIGHTED_WEIGHT_TYPES_CONSTANT_UNIVERSALLY_H