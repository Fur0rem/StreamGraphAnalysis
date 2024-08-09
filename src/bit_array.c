#include "bit_array.h"
#include "utils.h"
#include <stddef.h>
#include <stdio.h>

// Using an int because it is the biggest type that can be used with bitwise shifting operations that doesn't fuck up
const size_t MASK_SIZE = sizeof(int);

size_t nb_bytes(size_t nb_bits) {
	return (nb_bits / MASK_SIZE) + 1;
}

BitArray BitArray_with_n_bits(size_t nb_bits) {
	BitArray bit_array = (BitArray){.nb_bits = nb_bits, .bits = MALLOC(nb_bytes(nb_bits) * MASK_SIZE)};
	return bit_array;
}

BitArray BitArray_n_zeros(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < nb_bytes(nb_bits); i++) {
		bit_array.bits[i] = 0;
	}
	return bit_array;
}

BitArray BitArray_n_ones(size_t nb_bits) {
	BitArray bit_array = BitArray_with_n_bits(nb_bits);
	for (size_t i = 0; i < nb_bytes(nb_bits); i++) {
		bit_array.bits[i] = ~0;
	}
	return bit_array;
}

const size_t BYTE_SIZE = 8;
size_t byte_index(size_t index) {
	return (index / (MASK_SIZE * BYTE_SIZE));
}

size_t bit_index(size_t index) {
	return index % (MASK_SIZE * BYTE_SIZE);
}

bool BitArray_is_one(BitArray array, size_t index) {
	return (array.bits[byte_index(index)] & (1 << bit_index(index))) != 0;
}

bool BitArray_is_zero(BitArray array, size_t index) {
	return !BitArray_is_one(array, index);
}

void BitArray_set_one(BitArray array, size_t index) {
	array.bits[byte_index(index)] |= (1 << bit_index(index));
}

void BitArray_set_zero(BitArray array, size_t index) {
	array.bits[byte_index(index)] &= ~(1 << bit_index(index));
}

void BitArray_and_bit(BitArray array, size_t index, int value) {
	array.bits[byte_index(index)] &= value ? (1 << bit_index(index)) : ~(1 << bit_index(index));
}

void BitArray_or_bit(BitArray array, size_t index, int value) {
	array.bits[byte_index(index)] |= value ? (1 << bit_index(index)) : ~(1 << bit_index(index));
}

BitArray BitArray_and_array(BitArray array1, BitArray array2) {
	BitArray result = BitArray_with_n_bits(array1.nb_bits);
	for (size_t i = 0; i < nb_bytes(array1.nb_bits); i++) {
		result.bits[i] = array1.bits[i] & array2.bits[i];
	}
	return result;
}

BitArray BitArray_or_array(BitArray array1, BitArray array2) {
	BitArray result = BitArray_with_n_bits(array1.nb_bits);
	for (size_t i = 0; i < nb_bytes(array1.nb_bits); i++) {
		result.bits[i] = array1.bits[i] | array2.bits[i];
	}
	return result;
}

void BitArray_destroy(BitArray array) {
	free(array.bits);
}

char* BitArray_to_string(BitArray array) {
	size_t n_bits = array.nb_bits;
	char* str = MALLOC((n_bits + 1) * sizeof(char));
	for (size_t i = 0; i < n_bits; i++) {
		str[i] = BitArray_is_one(array, i) ? '1' : '0';
	}
	str[n_bits] = '\0';
	return str;
}

// Returns the number of leading zeros in the bit array starting from the given index
// TODO : optimise this function with clz (count leading zeros) instruction
size_t BitArray_leading_zeros_from(BitArray array, size_t index) {
	/*while (index < array.nb_bits) {
		size_t byte = byte_index(index);
		size_t bit = bit_index(index);
		size_t leading_zeros = __builtin_clz(array.bits[byte] << bit);
		if (leading_zeros < BYTE_SIZE - bit) {
			return leading_zeros;
		}
		index += BYTE_SIZE - bit;
	}
	return 0;*/
	size_t leading_zeros = 0;
	while (index < array.nb_bits) {
		if (BitArray_is_one(array, index)) {
			return leading_zeros;
		}
		leading_zeros++;
		index++;
	}
	return leading_zeros;
}

void BitArray_all_to_one(BitArray array) {
	for (size_t i = 0; i < nb_bytes(array.nb_bits); i++) {
		array.bits[i] = ~0;
	}
}

void BitArray_all_to_zero(BitArray array) {
	for (size_t i = 0; i < nb_bytes(array.nb_bits); i++) {
		array.bits[i] = 0;
	}
}