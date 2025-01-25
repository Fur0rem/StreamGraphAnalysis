/**
 * @file bit_array.h
 * @brief A variable length array of bits and functions to manipulate it.
 */

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include "utils.h"
#include <stdbool.h>
#include <stddef.h>

typedef size_t BitArraySlice;

/**
 * @brief The structure of a variable length array of bits.
 */
typedef struct BitArray BitArray;
struct BitArray {
	size_t nb_bits;	     ///< The number of bits in the array.
	BitArraySlice* bits; ///< The array of bits.
};

/**
 * @name Creation and destruction functions
 * Functions to create and destroy a BitArray.
 * @{
 */

/**
 * @brief Creates a BitArray with the given number of bits.
 *
 * The bits are not initialized.
 * Any call to this function should be paired with a call to BitArray_destroy, to free the memory.
 * @param[in] nb_bits The number of bits in the array.
 * @return A BitArray with the given number of bits.
 */
BitArray BitArray_with_n_bits(size_t nb_bits);

BitArray BitArray_n_zeros(size_t nb_bits); ///< Like BitArray_with_n_bits but initializes the bits to 0.
BitArray BitArray_n_ones(size_t nb_bits);  ///< Like BitArray_with_n_bits but initializes the bits to 1.

/**
 * @brief Destroys the given BitArray.
 * @param[in] array The BitArray to destroy.
 */
void BitArray_destroy(BitArray array);

/** @} */

/**
 * @name Bit getters
 * Functions to get the value of a bit in the BitArray.
 * @{
 */

/**
 * @brief Returns whether the bit at the given index is 1.
 * @param[in] array The BitArray.
 * @param[in] index The index of the bit.
 * @return Whether the bit at the given index is 1.
 */
bool BitArray_is_one(BitArray array, size_t index);
bool BitArray_is_zero(BitArray array, size_t index); ///< Like BitArray_is_one but returns whether the bit is 0.

/** @} */

/**
 * @name Bit setters and operations
 * Allows you to do any operation on a single bit, like the bitwise operations or setting the bit to 1 or 0.
 * They all modify the BitArray in place.
 * @{
 */

/**
 * @brief Sets the bit at the given index to 1.
 * @param[in, out] array The BitArray.
 * @param[in] index The index of the bit.
 */
void BitArray_set_one(BitArray array, size_t index);
void BitArray_set_zero(BitArray array, size_t index); ///< Like BitArray_set_one but sets the bit to 0.

/**
 * @brief Sets all the bits in the BitArray to 1.
 * @param[in, out] array The BitArray.
 */
void BitArray_all_to_one(BitArray array);
void BitArray_all_to_zero(BitArray array); ///< Like BitArray_set_all_ones but sets all the bits to 0.

/**
 * @brief Performs a bitwise and with the bit at the given index.
 * @param[in, out] array The BitArray.
 * @param[in] index The index of the bit.
 * @param[in] value The value to and with the bit.
 */
void BitArray_and_bit(BitArray array, size_t index, int value);
void BitArray_or_bit(BitArray array, size_t index, int value); ///< Like BitArray_and_bit but performs a bitwise or.

/** @} */

/**
 * @name BitArray operations
 * Allows you to do any bitwise operation on two BitArrays like you would do with integers.
 * @{
 */

/**
 * @brief Performs a bitwise and between two BitArrays.
 *
 * The two BitArrays must have the same number of bits.
 * It creates a new BitArray, and therefore must be freed with BitArray_destroy afterwards.
 * @param[in] array1 The first BitArray.
 * @param[in] array2 The second BitArray.
 * @return The BitArray resulting from the bitwise and.
 */
BitArray BitArray_and_array(BitArray array1, BitArray array2);
BitArray BitArray_or_array(BitArray array1, BitArray array2); ///< Like BitArray_and_array but performs a bitwise or.
/**
 * @brief Returns the number of leading zeros in the BitArray starting from the given index.
 *
 * Leading zeros means the number of zeros before the next 1.
 * @param[in] array The BitArray.
 * @param[in] index The index to start from.
 * @return The number of leading zeros.
 */
size_t BitArray_leading_zeros_from(BitArray array, size_t index);

/** @} */

/**
 * @name Miscellanous functions
 * @{
 */

/**
 * @brief Converts the BitArray to a string.
 *
 * The string is allocated with malloc and must be freed with free.
 * @param[in] array The BitArray.
 * @return The string representation of the BitArray.
 */
DeclareToString(BitArray);

/**
 * @brief Compares two BitArrays.
 *
 * Returns true if the two BitArrays are equal, false otherwise.
 * @param[in] left The first BitArray.
 * @param[in] right The second BitArray.
 * @return Whether the two BitArrays are equal.
 */
DeclareEquals(BitArray);

/** @} */

/**
 * @brief Counts the number of 1s in the BitArray.
 * @param[in] array The BitArray.
 * @return The number of 1s in the BitArray.
 */
size_t BitArray_count_ones(const BitArray* array);

#endif // BIT_ARRAY_H