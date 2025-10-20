#ifndef SGA_WEIGHTED_WEIGHT_FUNCTION_H
#define SGA_WEIGHTED_WEIGHT_FUNCTION_H

#include "weight_types/constant_universally.h"
#include "weight_types/lerp.h"
#include <stdint.h>

typedef struct SGA_WeightFunc {
	WeightFuncTag tag; ///< The tag indicating which type of weight function this is.
	union {
		LerpWeightFunc lerped;		    ///< The linearly interpolated weight function.
		ConstUniversally const_universally; ///< The universally constant weight function.
	} func;					    ///< The actual weight function data, depending on the tag.
} SGA_WeightFunc;

/**
 * @brief Creates a universally constant weight function
 * @param weight The universal weight to apply
 * @return A weight func of kind ConstUniversally
 */
SGA_WeightFunc SGA_WeightFunc_const_universally(SGA_Weight weight);

/**
 * @brief Gets the weight for a specific element at a specific time.
 * @param weight_func The weight function to get the weight from.
 * @param element_id The id of the element to get the weight for.
 * @param time The time instant to get the weight at.
 * @return The weight for the element at the given time.
 */
SGA_Weight SGA_WeightFunc_weight_at_t(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Time time);

/**
 * @brief Gets the weight integral for a specific element over a range of time.
 * @param weight_func The weight function to get the weight integral from.
 * @param element_id The id of the element to get the weight integral for.
 * @param interval The time interval over which to compute the weight integral.
 * @return The weight integral for the element over the given time interval.
 */
SGA_Weight SGA_WeightFunc_weight_integral_between(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Interval interval);

/**
 * @brief Gets the maximum weight of the function.
 */
SGA_Weight SGA_WeightFunc_max(const SGA_WeightFunc* weight_func);

/**
 * @brief Gets the minimum weight of the function.
 */
SGA_Weight SGA_WeightFunc_min(const SGA_WeightFunc* weight_func);

/**
 * @brief Normalises the weights of the function to the range [0, 1].
 * @param weight_func The weight function to normalise.
 * @param min The minimum weight to map to 0.
 * @param max The maximum weight to map to 1.
 */
void SGA_WeightFunc_normalise(SGA_WeightFunc* weight_func, SGA_Weight min, SGA_Weight max);

/**
 * @brief Destroys a weight function, freeing any allocated memory.
 * @param[in] weight_func The weight function to destroy. Not usable after this call.
 */
void SGA_WeightFunc_destroy(SGA_WeightFunc weight_func);

/**
 * @brief Gets the maximum weight in a given interval for a specific element.
 * @param weight_func The weight function to query.
 * @param element_id The id of the element to query.
 * @param interval The time interval to query.
 * @return The maximum weight for the element in the given time interval.
 */
SGA_Weight SGA_WeightFunc_max_of_elem_in_interval(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Interval interval);

/**
 * @brief Gets the minimum weight in a given interval for a specific element.
 * @param weight_func The weight function to query.
 * @param element_id The id of the element to query.
 * @param interval The time interval to query.
 * @return The minimum weight for the element in the given time interval.
 */
SGA_Weight SGA_WeightFunc_min_of_elem_in_interval(const SGA_WeightFunc* weight_func, size_t element_id, SGA_Interval interval);

/**
 * @brief Gets the maximum weight in a given interval for all elements.
 * @param weight_func The weight function to query.
 * @param interval The time interval to consider.
 * @return The maximum weight for all elements in the given time interval.
 */
SGA_Weight SGA_WeightFunc_max_in_interval(const SGA_WeightFunc* weight_func, SGA_Interval interval);

/**
 * @brief Gets the minimum weight in a given interval for all elements.
 * @param weight_func The weight function to query.
 * @param interval The time interval to consider.
 * @return The minimum weight for all elements in the given time interval.
 */
SGA_Weight SGA_WeightFunc_min_in_interval(const SGA_WeightFunc* weight_func, SGA_Interval interval);

#ifdef SGA_INTERNAL

SGA_WeightFunc SGA_WeightFunc_from_parsed(ParsedWeightFunction parsed, size_t nb_elements, SGA_Interval lifespan, bool is_node,
					  LinkIdMapHashset* link_id_map);

#endif // SGA_INTERNAL

#endif // SGA_WEIGHTED_WEIGHT_FUNCTION_H