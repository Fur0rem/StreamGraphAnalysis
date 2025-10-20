/**
 * @file src/weighted/weight_types/lerp.h
 * @brief A linearly interpolated weight function for weighted streams.
 */

#ifndef WEIGHTED_WEIGHT_TYPES_LERP_H
#define WEIGHTED_WEIGHT_TYPES_LERP_H

#include "../../interval.h"
#include "../../parsing/parse_weights.h"
#include "../../units.h"

/**
 * @brief A structure representing the weights for a single element in a lerp weight function.
 * It is a linearized list of lists, where each list contains the weights for each time instant in an interval.
 */
typedef struct LerpWeightedElement {
	SGA_Weight* weights;		    ///< An array of weights for each time instant for this element.
	SGA_IntervalsSet covered_intervals; ///< The set of intervals covered by this element's weights.
} LerpWeightedElement;

/**
 * @brief A structure representing a linearly interpolated weight function.
 * This structure is used to define a weight function that for each element, has a weight associated with each time instant,
 * and does linear interpolation between these weights.
 */
typedef struct LerpWeightFunc {
	LerpWeightedElement* elements; ///< An array of elements, each containing weights and covered intervals.
	size_t nb_elements;	       ///< The number of elements in the weight function.
} LerpWeightFunc;

/**
 * @brief Deallocates a LerpWeightFunc structure.
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

#ifdef SGA_INTERNAL

/**
 * @brief Creates a LerpWeightedElement from a list of lists of LerpWeightPoints for each interval.
 * @param weighted_intervals The list of lists of LerpWeightPoints for each interval.
 * @return A LerpWeightedElement containing the weights and covered intervals, linearized and ready to use.
 */
LerpWeightedElement LerpWeightedElement_from(LerpWeightPointArrayListArrayList* weighted_intervals);

LerpWeightFunc LerpWeightFunc_from_parsed(ParsedLerpWeightFunction parsed, bool is_node, LinkIdMapHashset* link_id_map);
#endif // SGA_INTERNAL

#endif // WEIGHTED_WEIGHT_TYPES_LERP_H