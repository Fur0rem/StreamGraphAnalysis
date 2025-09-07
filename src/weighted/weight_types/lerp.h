/**
 * @file src/weighted/weight_types/lerp.h
 * @brief A linearly interpolated weight function for weighted streams.
 */

#ifndef WEIGHTED_WEIGHT_TYPES_LERP_H
#define WEIGHTED_WEIGHT_TYPES_LERP_H

#include "../../interval.h"
#include "../../units.h"

/**
 * @brief A structure representing a linearly interpolated weight function.
 * This structure is used to define a weight function that for each element, has a weight associated with each time instant,
 * and does linear interpolation between these weights.
 */
typedef struct LerpWeightFunc {
	SGA_Weight**
	    weights; ///< A 2D array of weights, where each row corresponds to an element and each column corresponds to a time instant.
	size_t nb_elements; ///< The number of elements in the weight function.
	size_t nb_times;    ///< The number of time instants in the weight function.
} LerpWeightFunc;

/**
 * @brief Allocates a new LerpWeightFunc structure. Doesn't initialise the weights.
 *
 * @param nb_elements The number of elements in the weight function.
 * @param nb_times The number of time instants in the weight function.
 * @return A newly allocated LerpWeightFunc structure, with uninitialised weights.
 */
LerpWeightFunc LerpWeightFunc_new(size_t nb_elements, size_t nb_times);

/**
 * @brief Deallocates a LerpWeightFunc structure.
 *
 * @param self The LerpWeightFunc structure to deallocate.
 */
void LerpWeightFunc_destroy(LerpWeightFunc self);

/**
 * @brief Gets the weight for a specific element at a specific time.
 * @param self The LerpWeightFunc structure to get the weight from.
 * @param element_id The id of the element to get the weight for.
 * @param time The time instant to get the weight at.
 * @return The weight for the element at the given time.
 */
SGA_Weight LerpWeightFunc_weight_at_t(const LerpWeightFunc* self, size_t element_id, SGA_Time time);

/**
 * @brief Gets the weight integral for a specific element over a range of time.
 * @param self The LerpWeightFunc structure to get the weight integral from.
 * @param element_id The id of the element to get the weight integral for.
 * @param interval The time interval over which to compute the weight integral.
 * @return The weight integral for the element over the given time interval.
 */
SGA_Weight LerpWeightFunc_weight_integral_between(const LerpWeightFunc* self, size_t element_id, SGA_Interval interval);

/**
 * @brief Gets the maximum weight of the function.
 * @param self The LerpWeightFunc structure to get the maximum weight from.
 * @return The maximum weight of the function.
 */
SGA_Weight LerpWeightFunc_max(const LerpWeightFunc* self);

/**
 * @brief Gets the minimum weight of the function.
 * @param self The LerpWeightFunc structure to get the minimum weight from.
 * @return The minimum weight of the function.
 */
SGA_Weight LerpWeightFunc_min(const LerpWeightFunc* self);

/**
 * @brief Normalises the weights of the function to the range [0, 1].
 * @param self The LerpWeightFunc structure to normalise.
 * @param min The minimum weight to map to 0.
 * @param max The maximum weight to map to 1.
 */
void LerpWeightFunc_normalise(LerpWeightFunc* self, SGA_Weight min, SGA_Weight max);

#endif // WEIGHTED_WEIGHT_TYPES_LERP_H